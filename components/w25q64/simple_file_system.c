#include "simple_file_system.h"

#include <memory.h>
#include <string.h>

#include "esp_log.h"
#define TAG "HEH"

static uint8_t file_prefix[FILE_PREFIX_SIZE] = {0xFF, 0xFF, 0x53, 0x46, 0x53};

bool sfs_init(sfs_t *sfs, sfs_config_t *config) {
    if (sfs == NULL) {
        return false;
    }

    (void) memset(sfs, SFS_EMPTY_VALUE, sizeof(sfs_t));
    sfs->erase_fnc = config->erase_fnc;
    sfs->read_fnc = config->read_fnc;
    sfs->write_fnc = config->write_fnc;

    sfs->flash_size_bits = MB_TO_BITS(config->flash_size_mb);
    sfs->flash_sector_bits = KB_TO_BITS(config->flash_sector_kb);

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

// static bool check_file_name(uint8_t *file_name, size_t file_name_size) {
//     if (file_name_size != MAX_FILE_NAME_SIZE) {
//         return false;
//     }

//     if (file_name[MAX_FILE_NAME_SIZE - 1] != '\0') {
//         return false;
//     }

//     return true;
// }

sfs_err_t set_file_name(sfs_file_t *file, char *file_name) {
    if (file == NULL || file_name == NULL) {
        return SFS_NULL_POINTER;
    }

    if (strlen(file_name) > MAX_FILE_NAME_SIZE) {
        return SFS_INVALID_FILE_NAME;
    }

    (void) memset(file->name, 0xFF, sizeof(file->name));
    (void) memcpy(file->name, file_name, strlen(file_name));
    file->name[MAX_FILE_NAME_SIZE - 1] = '\0';

    return SFS_OK;
}

uint32_t sector_to_address(sfs_t *sfs, uint32_t sector) {
    return sector * sfs->flash_sector_bits;
}

sfs_err_t sector_belongs_to_file(sfs_t *sfs, sfs_file_t *file, uint32_t sector, bool *belongs) {
    uint8_t sector_file_name[MAX_FILE_NAME_SIZE] = {0};
    *belongs = false;
    
    int ret_size = sfs->read_fnc(sector_to_address(sfs, sector) + FILE_PREFIX_SIZE,
                                sector_file_name, sizeof(sector_file_name));
    if (ret_size != sizeof(sector_file_name)) {
        return SFS_FLASH_READ;
    }
    
    if (uint8_cmpr(sector_file_name, file->name, sizeof(sector_file_name)) == true) {
        *belongs = true;
    }

    return SFS_OK;
}

sfs_err_t file_first_last_sector(sfs_t *sfs, sfs_file_t *file, int32_t *first, int32_t *last) {
    sfs_err_t ret = SFS_OK;
    uint32_t number_of_sectors = sfs->flash_size_bits / sfs->flash_sector_bits;
    bool belongs_to_file = false;

    for (uint32_t sector = 0; sector < number_of_sectors; ++sector) {
        ret = sector_belongs_to_file(sfs, file, sector, &belongs_to_file);
        if (ret != SFS_OK) {
            return ret;
        }

        if (belongs_to_file == true) {
            if (*first < 0) {
                *first = sector;
            }

            *last = sector;
        }
    }

    return SFS_OK;
}

sfs_err_t read_file_info_from_sectors(sfs_t *sfs, sfs_file_t *file) {
    sfs_err_t ret = SFS_OK;
    int32_t file_first_sector = -1;
    int32_t file_last_sector = -1;
    
    ret = file_first_last_sector(sfs, file, &file_first_sector, &file_last_sector);
    ESP_LOGI(TAG, "First sector %d\t Last sector %d", (int)file_first_sector, (int)file_last_sector);
    if (ret != SFS_OK) {
        return ret;
    }

    // TO DO sector not found -> create new file in free sector

    // TO DO find end of datya in last sector

    return SFS_OK;
}

sfs_err_t sfs_open(sfs_t *sfs, sfs_file_t *file, char *file_name) {
    sfs_err_t ret = SFS_OK;
    
    ret = set_file_name(file, file_name);
    if (ret != SFS_OK) {
        return ret;
    }

    ret = read_file_info_from_sectors(sfs, file);
    if (ret != SFS_OK) {
        return ret;
    }

    return SFS_OK;
} 



// static sfs_err_t get_data_len(sfs_t *sfs, uint32_t address, uint32_t *len) {
//     uint8_t len_buffer[DATA_LEN_SIZE];
//     int ret = sfs->read_fnc(address, len_buffer, sizeof(len_buffer));

//     if (ret != sizeof(len_buffer)) {
//         return SFS_FLASH_READ;
//     }

//     *len = len_buffer[1];
//     *len |= (len_buffer[0] << 8);

//     return SFS_OK;
// }