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