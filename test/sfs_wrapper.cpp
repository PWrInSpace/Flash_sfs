#include "sfs_wrapper.h"

static struct {
    flash_mock_t memory;
    sfs_t file_system;
} flash_t;

int sfs_write(uint32_t address, uint8_t *buffer, uint32_t size) {
    uint32_t sector = address / flash_t.memory.sector_size_bytes;
    address = address % flash_t.memory.sector_size_bytes;
    return flash_mock_write(&flash_t.memory, sector, address, buffer, size);
}

int sfs_read(uint32_t address, uint8_t *buffer, uint32_t size) {
    return flash_mock_read(&flash_t.memory, address, buffer, size);
}

bool sfs_erase(uint32_t sector) {
    return flash_mock_erase_sector(&flash_t.memory, sector);
}

static bool mock_init(void) {
    sfs_config_t cfg;
    cfg.flash_size_mb = 8;
    cfg.flash_sector_kb = 4;
    cfg.erase_fnc = sfs_erase;
    cfg.read_fnc = sfs_read;
    cfg.write_fnc = sfs_write;
    
    if (flash_mock_init(&flash_t.memory, SIZE_16MB, 4) == false) {
        return false;
    }

    if (sfs_init(&flash_t.file_system, &cfg) != SFS_OK) {
        return false;
    }

    return true;
}

static bool mock_deinit(void) {
    if (flash_mock_deinit(&flash_t.memory) == false) {
        return false;
    }

    return true;
}

void FlashTest::SetUp() {
    mock_init();

    file_system = &flash_t.file_system;
    memory = &flash_t.memory;
}

void FlashTest::TearDown() {
    mock_deinit();
}

bool FlashTest::checkSectorFileName(uint32_t sector, char* file_name) {
    uint32_t secotr_size = this->file_system->flash_sector_bits;
    uint8_t *file_sector_ptr = &this->memory->memory[secotr_size * sector]; 

    if (memcmp(file_sector_ptr, file_prefix, sizeof(file_prefix)) != 0) {
        return false;
    }

    // Convert file name
    uint8_t name[MAX_FILE_NAME_SIZE];
    (void) memset(name, FLASH_NO_DATA, sizeof(name));
    (void) memcpy(name, file_name, strlen(file_name));
    name[MAX_FILE_NAME_SIZE - 1] = '\0';

    // Move pointer to file name
    if (memcmp(&file_sector_ptr[sizeof(file_prefix)], name, MAX_FILE_NAME_SIZE) != 0) {
        return false;
    }

    return true;
}

void FlashTest::dump256(uint32_t sector, uint32_t address) {
    uint32_t sector_size = this->file_system->flash_sector_bits;
    uint8_t *file_sector_ptr = &this->memory->memory[sector_size * sector] + address; 

    for (int i = 0; i < 256; ++i) {
        if (i % 0xF == 0) {
            std::cout << std::endl << std::hex << i / 0xF << "\t| ";
        }
    
        printf("0x%2.X \t", file_sector_ptr[i]);
    }
    std::cout << std::endl;
}

bool FlashTest::checkSFSNextFreeSector(int32_t sector) {
    return this->file_system->next_free_sector == sector;
}

bool FlashTest::checkFileStartAddress(sfs_file_t *file, uint32_t sector) {
    uint32_t secotr_size = this->file_system->flash_sector_bits;
    if (file->start_address != secotr_size * sector) {
        return false;
    }

    return true;
}

bool FlashTest::checkFileEndAddress(sfs_file_t *file, uint32_t sector, uint32_t address) {
    uint32_t sector_size = this->file_system->flash_sector_bits;
    if (file->end_address != ((sector_size * sector) + address)) {
        std::cout << "FILE END ADDRES: " << file->end_address << std::endl;
        return false;
    }

    return true;
}

bool FlashTest::setMemory(uint32_t sector, uint32_t address, uint8_t val, uint32_t size) {
    uint32_t sector_size = this->file_system->flash_sector_bits;
    uint8_t *file_sector_pointer = &this->memory->memory[sector_size * sector] + address;
    if (size + address > sector_size) {
        return false;
    }

    (void) memset(file_sector_pointer, val, size);   
    
    return true;
}
