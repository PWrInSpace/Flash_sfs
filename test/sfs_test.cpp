#include <gtest/gtest.h>
#include <iostream>

extern "C" {
    #include "flash_mock/flash_mock.h"
    #include "sfs/simple_file_system.h"
}

static flash_mock_t flash;

static int sfs_write(uint32_t address, uint8_t *buffer, uint32_t size) {
    uint32_t sector = address / flash.sector_size_bytes;
    address = sector % address;
    return flash_mock_write(&flash, sector, address, buffer, size);
}

static int sfs_read(uint32_t address, uint8_t *buffer, uint32_t size) {
    return flash_mock_read(&flash, address, buffer, size);
}

static bool sfs_erase(uint32_t sector) {
    return flash_mock_erase_sector(&flash, sector);
}

static sfs_t sfs;


TEST(SFS, Connect_sfs_with_flash_mock) {
    static sfs_config_t cfg;
    cfg.flash_size_mb = 8;
    cfg.flash_sector_kb = 4;
    cfg.erase_fnc = sfs_erase;
    cfg.read_fnc = sfs_read;
    cfg.write_fnc = sfs_write;
    
    EXPECT_EQ(true, flash_mock_init(&flash, SIZE_16MB, 4));
    EXPECT_EQ(SFS_OK, sfs_init(&sfs, &cfg));
}

TEST(SFS, Open_invalid_file_name) {
    char file_name[MAX_FILE_NAME_SIZE + 2];
    memset(file_name, 0x54, sizeof(file_name));
    file_name[sizeof(file_name) - 1] = '\0';
    
    sfs_file_t file;
    EXPECT_EQ(SFS_INVALID_FILE_NAME, sfs_open(&sfs, &file, file_name));
}

TEST(SFS, Open_null_file_name) {
    sfs_file_t file;
    EXPECT_EQ(SFS_NULL_POINTER, sfs_open(&sfs, &file, NULL));
}

TEST(SFS, Open_valid_file_name) {
    static sfs_config_t cfg;
    cfg.flash_size_mb = 8;
    cfg.flash_sector_kb = 4;
    cfg.erase_fnc = sfs_erase;
    cfg.read_fnc = sfs_read;
    cfg.write_fnc = sfs_write;
    
    EXPECT_EQ(SFS_OK, sfs_init(&sfs, &cfg));

    char file_name[] = "test";
    sfs_file_t file;
    EXPECT_EQ(true, flash_mock_init(&flash, SIZE_16MB, 4));
    EXPECT_EQ(SFS_OK, sfs_open(&sfs, &file, file_name));
}