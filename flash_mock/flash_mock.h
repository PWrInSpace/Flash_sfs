// Copyrights PWrInSpace Kuba
#ifndef __FLASH_MOCK_H_
#define __FLASH_MOCK_H_

#include <stdint.h>
#include <stdbool.h>

#define MB_TO_BYTES(x) (x * 1024 * 1024)
#define KB_TO_BYTES(x) (x * 1024)

#define ERASED_BYTE 0xFF

typedef enum {
    SIZE_8MB = 8,
    SIZE_16MB = 16
} flash_mock_size_t;

typedef struct {
    uint32_t memory_size_bytes;
    uint32_t sector_size_bytes;
    uint8_t* memory;
} flash_mock_t;

bool flash_mock_init(flash_mock_t *dev, flash_mock_size_t size, uint32_t sector_size_kb);

#endif