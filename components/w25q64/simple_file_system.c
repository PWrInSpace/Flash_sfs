#include "simple_file_system.h"

#include <memory.h>

#include "esp_log.h"
#define TAG "HEH"

static uint8_t file_prefix[FILE_PREFIX_SIZE] = {0xFF, 0xFF, 0x53, 0x46, 0x53};

bool sfs_init(sfs_t *sfs, sfs_config_t *config) {
    if (sfs == NULL) {
        return false;
    }

    (void) memset(sfs, SFS_EMPTY_VALUE, sizeof(sfs_t));
    sfs->file.start_address = SFS_EMPTY_VALUE;
    sfs->file.end_address = SFS_EMPTY_VALUE;

    sfs->erase_fnc = config->erase_fnc;
    sfs->read_fnc = config->read_fnc;
    sfs->write_fnc = config->write_fnc;

    return true;
}

static bool uint8_cmpr(uint8_t *buf1, uint8_t *buf2, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (buf1[i] != buf2[i]) {
            return false;
        }
    }

    return true;
}

static bool check_file_name(uint8_t *file_name, size_t file_name_size) {
    if (file_name_size != MAX_FILE_NAME_SIZE) {
        return false;
    }

    if (file_name[MAX_FILE_NAME_SIZE - 1] != '\0') {
        return false;
    }

    return true;
}

inline static void set_file_start_address(sfs_file_t *file, uint32_t start_address) {
    file->start_address = start_address;
}

static sfs_err_t get_file_name(sfs_t *sfs, sfs_file_t *file) {
    uint8_t file_info_buffer[13] = {0};
    int ret = sfs->read_fnc(file->start_address, file_info_buffer, sizeof(file_info_buffer));

    if (ret != sizeof(file_info_buffer)) {
        return SFS_FLASH_READ;
    }

    if (uint8_cmpr(file_prefix, file_info_buffer, sizeof(file_prefix)) == false) {
        return SFS_INVALID_PREFIX;
    }


    if (check_file_name(file_info_buffer + sizeof(file_prefix),
        sizeof(file_info_buffer) - sizeof(file_prefix)) == false) {
            return SFS_INVALID_FILE_NAME;
    }

    (void) memcpy(file->name, file_info_buffer + sizeof(file_prefix), sizeof(file->name));

    return SFS_OK;
}

static sfs_err_t get_data_len(sfs_t *sfs, uint32_t address, uint32_t *len) {
    uint8_t len_buffer[DATA_LEN_SIZE];
    int ret = sfs->read_fnc(address, len_buffer, sizeof(len_buffer));

    if (ret != sizeof(len_buffer)) {
        return SFS_FLASH_READ;
    }

    *len = len_buffer[1];
    *len |= (len_buffer[0] << 8);

    return SFS_OK;
}

static sfs_err_t get_end_of_file_chunk(sfs_t *sfs, sfs_file_t *file) {
    sfs_err_t ret = SFS_OK;
    uint32_t data_len = 0;
    uint32_t address = file->start_address + FILE_INFO_SIZE;
    bool end_of_data = false;

    while (end_of_data == false) {
        ret = get_data_len(sfs, address, &data_len);
        if (ret != SFS_OK) {
            return ret;
        }

        if (data_len == NO_MORE_DATA) {
            ESP_LOGI(TAG, "No more data");
            end_of_data = true;
        } else {
            address += data_len;
            ESP_LOGI(TAG, "Data len %d", (int)data_len);
        }
    }

    file->end_address = address;

    return SFS_OK;
}

static sfs_err_t open_file_by_address(sfs_t *sfs, sfs_file_t *file, uint32_t address) {
    sfs_err_t ret = SFS_OK;
    
    set_file_start_address(file, address);

    ret = get_file_name(sfs, file);
            
    if (ret != SFS_OK) {
        ESP_LOGI(TAG, "FILE INFO ERROR %d", ret);
        return ret;
    }
    
    ret = get_end_of_file_chunk(sfs, file);
    if (ret != SFS_OK) {
        ESP_LOGE(TAG, "EOD ERROR %d", ret);
        return ret;
    }

    return ret;
}

static sfs_err_t open_user_file(sfs_t *sfs) {
    sfs_err_t ret = SFS_OK;
    sfs_file_t file = SFS_FILE_INIT_DEFAULT();
    uint32_t address = 0;

    // Open files by addres until invalid prefix occures -> end of valid data
    while (ret != SFS_INVALID_PREFIX) {
        ret = open_file_by_address(sfs, &file, address);
        if (ret != SFS_INVALID_PREFIX && ret != SFS_OK) {
            return ret;
        }
        
        address = file.end_address;
    }

    // check if rest of flash is empty
    ESP_LOGI(TAG, "END ADDRESS %d", (int) address);
    return SFS_OK;
}

bool sfs_open(sfs_t *sfs, char *file_name, uint8_t file_name_size) {
    if (sfs == NULL || file_name == NULL) {
        return false;
    }

    if (file_name_size > MAX_FILE_NAME_SIZE) {
        return false;
    }

    (void) memcpy(sfs->file.name, file_name, file_name_size);
    sfs->file.name[MAX_FILE_NAME_SIZE - 1] = '\0';

    if (open_user_file(sfs) != SFS_OK) {
        return false;
    }

    return true;
}


// static sfs_err_t find_end_of_data(sfs_t *sfs, uint32_t *eof_address) {
//     sfs_err_t ret = SFS_OK;
//     uint32_t addr = 0;
//     uint32_t data_len = 0;
//     bool is_data = false;   // first data in flash should be file info
//     bool no_more_data = false;

//     // add flash size to condition
//     while (no_more_data == false) {
//         if (is_data == false) {
//             // check file info
//             ret = check_file_info(sfs, addr);
            
//             if (ret != SFS_OK) {
//                 ESP_LOGI(TAG, "END_OF_DATA %d", (int)addr);
//                 // END of data
//                 no_more_data = true;
//                 // return ret;
//             } else {
//                 addr += FILE_INFO_SIZE;
//                 is_data = true;
//             }
//         } else {
//             // go through data
//             ret = get_data_len(sfs, addr, &data_len);
    
//             if (ret != SFS_OK) {
//                 return ret;
//             }

//             if (data_len == NO_MORE_DATA) {
//                 addr += DATA_LEN_SIZE; // Move addr to first free memory byte
//                 ESP_LOGI(TAG, "No more data");
//                 is_data = false;
//             } else {
//                 addr += data_len;
//                 ESP_LOGI(TAG, "Data len %d", (int)data_len);
//             }
//         }
//     }

//     *eof_address = addr;

//     return SFS_OK;
// }

// static bool open_file(sfs_t *sfs) {
//     if (sfs == NULL) {
//         return false;
//     }

//     uint32_t end_of_data_address = 0;
//     find_end_of_data(sfs, &end_of_data_address);
//     ESP_LOGI(TAG, "END OF DATA ADDRESS %d", (int) end_of_data_address);

//     return true;
// }