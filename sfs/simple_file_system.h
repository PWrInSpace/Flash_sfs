#ifndef __SIMPLE_FILE_SYSTEM_H_
#define __SIMPLE_FILE_SYSTEM_H_

#include <stdint.h>
#include <stdbool.h>

#define NO_MORE_DATA 0xFFFF
#define SFS_EMPTY_VALUE 0x00
#define FLASH_NO_DATA 0xFF

#define MAX_FILE_NAME_SIZE 8
#define FILE_PREFIX_SIZE 5
#define DATA_LEN_SIZE 2
#define FILE_INFO_SIZE (MAX_FILE_NAME_SIZE + FILE_PREFIX_SIZE)

#define MB_TO_BITS(x) (x * 1024 * 1024)
#define KB_TO_BITS(x) (x * 1024)

#define SFS_DEBUG_ON
#define SFS_DEBUG(format, ...) 
#ifdef SFS_DEBUG_ON
    // printf(format, __VA_ARGS__)
#endif

typedef bool(*sfs_flash_erase)(uint32_t sector);
typedef int(*sfs_flash_read)(uint32_t address, uint8_t *buffer, uint32_t size);
typedef int(*sfs_flash_write)(uint32_t address, uint8_t* buffer, uint32_t size);

typedef enum {
    SFS_OK = 0,
    SFS_NULL_POINTER,
    SFS_FLASH_READ,
    SFS_FLASH_WRITE,
    SFS_INVALID_PREFIX,
    SFS_INVALID_FILE_NAME,
    SFS_END_OF_DATA,
    SFS_FLASH_FULL,
    SFS_UNKNOWN,
    SFS_INVALID_SIZE,
    SFS_INVALID_VALUE,
} sfs_err_t;

typedef struct {
    uint8_t name[MAX_FILE_NAME_SIZE]; // File name
    uint32_t end_address;   // End of data
    uint32_t start_address; // Begining of data
    uint32_t address_pointer; // User address pointer

    uint8_t file_descriptor;
} sfs_file_t;

typedef struct {
    sfs_flash_erase erase_fnc;
    sfs_flash_read read_fnc;
    sfs_flash_write write_fnc;

    uint32_t flash_size_bits;
    uint32_t flash_sector_bits;
} sfs_t;

typedef struct {
    uint32_t flash_size_mb;
    uint32_t flash_sector_kb;

    sfs_flash_erase erase_fnc;
    sfs_flash_read read_fnc;
    sfs_flash_write write_fnc;
} sfs_config_t;

sfs_err_t sfs_init(sfs_t *sfs, sfs_config_t *config);
sfs_err_t sfs_open(sfs_t *sfs, sfs_file_t *file, char *file_name);
// bool sfs_write(sfs_t *sfs, uint8_t *data, uint32_t size);
// bool sfs_read_line(sfs_t *sfs, uint8_t *buffer, uint16_t buffer_size);


#define SFS_FILE_INIT_DEFAULT() \
    {                           \
        .name = {0},            \
        .start_address = 0,     \
        .end_address = 0,       \
        .address_pointer = 0,   \
        .file_descirptor = 0,   \
    }                           \

#endif