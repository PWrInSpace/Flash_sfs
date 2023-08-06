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


int flash_mock_write(flash_mock_t *dev, uint32_t sector, uint32_t addr, uint8_t *data, uint16_t size) {
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
    uint32_t data_end_address = sector * dev->sector_size_bytes + addr + size;
    if (data_end_address > dev->memory_size_bytes) {
        size = size - (data_end_address - dev->memory_size_bytes);
    }

    (void) memcpy(dev->memory + ((sector * dev->sector_size_bytes) + addr), data, size);
    return size;
}

// int flash_mock_read(flash_mock_t *dev, uint32_t addr, uint8_t *data, uint16_t size) {
//     return size;
// }
