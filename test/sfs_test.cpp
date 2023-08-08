#include <gtest/gtest.h>

extern "C" {
    #include "flash_mock/flash_mock.h"
    #include "sfs/simple_file_system.h"
}

static flash_mock_t flash;

int sfs_write(uint32_t address, uint8_t *buffer, uint32_t size) {
    uint32_t sector = address / flash.sector_size_bytes;
    address = sector % address;
    return flash_mock_write(&flash, sector, address, buffer, size);
}

int sfs_read(uint32_t address, uint8_t *buffer, uint32_t size) {
    uint32_t sector = address / flash.sector_size_bytes;
    address = sector % address;
    return flash_mock_write(&flash, sector, address, buffer, size);
}

bool sfs_erase(uint32_t sector) {
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
    
    EXPECT_EQ(SFS_OK, sfs_init(&sfs, &cfg));
}
