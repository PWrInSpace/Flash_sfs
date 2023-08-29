#pragma once

#include <gtest/gtest.h>
#include <iostream>

extern "C" {
    #include "flash_mock/flash_mock.h"
    #include "sfs/simple_file_system.h"
}

class FlashTest: public ::testing::Test {
    public:
    sfs_t* file_system;
    flash_mock_t* memory;

    void SetUp() override;
    void TearDown() override;

    bool checkSectorFileName(uint32_t sector, char* file_name);
    void dump256(uint32_t sector, uint32_t address);
    bool checkSFSNextFreeSector(int32_t sector);
    bool checkFileStartAddress(sfs_file_t *file, uint32_t sector);
    bool checkFileEndAddress(sfs_file_t *file, uint32_t sector, uint32_t address);
    bool setMemory(uint32_t sector, uint32_t address, uint8_t val, uint32_t size);
    bool write2Bytes(uint32_t sector, uint32_t address, uint16_t data);

    bool arrayEqual(uint8_t *arr1, uint8_t *arr2, size_t size);
};
