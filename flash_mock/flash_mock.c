#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "flash_mock.h"

bool flash_mock_init(flash_mock_t *dev, flash_mock_size_t size, uint32_t sector_size_kb) {
    if (dev == NULL) {
        return false;
    }

    if (sector_size_kb > size) {
        return false;
    }
    
    dev->memory_size_bytes = MB_TO_BYTES(size);
    dev->sector_size_bytes = KB_TO_BYTES(sector_size_kb);

    if (dev->memory_size_bytes % dev->sector_size_bytes != 0) {
        return false;
    }

    dev->memory = (uint8_t*)malloc(dev->memory_size_bytes);
    if (dev->memory == NULL) {
        return false;
    }

    (void) memset(dev->memory, ERASED_BYTE, dev->memory_size_bytes);

    return true;
}


int flash_mock_write(flash_mock_t *dev, uint32_t sector, uint32_t addr, uint8_t *data, uint32_t size) {
    if (dev == NULL || dev->memory == NULL) {
        return -1;
    }

    // check if sector is valid
    if (sector >= (dev->memory_size_bytes / dev->sector_size_bytes)) {
        return -1;
    }

    // check if address is valid
    if (addr >= dev->sector_size_bytes) {
        return -1;
    }

    // Check if data move over memory size
    uint32_t data_start_address = sector * dev->sector_size_bytes + addr;
    if ((data_start_address + size) > dev->memory_size_bytes) {
        size = dev->memory_size_bytes - data_start_address;
    }

    int ret = 0;
    for (uint32_t i = 0; i < size; ++i) {
        if (i < dev->memory_size_bytes) {
            // TO DO: better function to mock multiply writing to the same address
            dev->memory[data_start_address + i] = dev->memory[data_start_address + i] & data[i];
            ret += 1;
        }
    }
    return ret;
}

int flash_mock_read(flash_mock_t *dev, uint32_t addr, uint8_t *data, uint32_t size) {
    if (dev == NULL || dev->memory == NULL) {
        return false;
    }
    
    if (addr >= dev->memory_size_bytes) {
        return -1;
    }


    if (addr + size >= dev->memory_size_bytes) {
        size = dev->memory_size_bytes - addr - 1; // Minus one to start from 0
    }

    memcpy(data, dev->memory + addr, size);

    return size;
}

bool flash_mock_erase_sector(flash_mock_t * dev, uint32_t sector) {
    if (dev == NULL || dev->memory == NULL) {
        return false;
    }

    if (sector >= dev->memory_size_bytes / dev->sector_size_bytes) {
        return false;
    }

    (void) memset(dev->memory + dev->sector_size_bytes * sector,
                    ERASED_BYTE, dev->sector_size_bytes);
    
    return true;
}

bool flash_mock_deinit(flash_mock_t *dev) {
    if (dev == NULL || dev->memory == NULL) {
        return false;
    }

    (void) free(dev->memory);
    return true;
}
