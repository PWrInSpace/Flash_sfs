#ifndef __SIMPLE_FILE_SYSTEM_H_
#define __SIMPLE_FILE_SYSTEM_H_

#include <stdint.h>
#include <stdbool.h>

#define NO_MORE_DATA 0xFFFF
#define SFS_EMPTY_VALUE 0x00

#define MAX_FILE_NAME_SIZE 8
#define FILE_PREFIX_SIZE 5
#define DATA_LEN_SIZE 2
#define FILE_INFO_SIZE (MAX_FILE_NAME_SIZE + FILE_PREFIX_SIZE)

typedef int(*sfs_flash_erase)(uint32_t address, uint32_t size);
typedef int(*sfs_flash_read)(uint32_t address, uint8_t *buffer, uint32_t size);
typedef int(*sfs_flash_write)(uint32_t address, uint8_t* buffer, uint32_t size);

typedef enum {
    SFS_OK = 0,
    SFS_FLASH_READ,
    SFS_INVALID_PREFIX,
    SFS_INVALID_FILE_NAME,
    SFS_END_OF_DATA,
} sfs_err_t;

typedef struct {
    char name[MAX_FILE_NAME_SIZE];
    uint32_t end_address;
    uint32_t start_address;
} sfs_file_t;

typedef struct {
    sfs_file_t file;

    sfs_flash_erase erase_fnc;
    sfs_flash_read read_fnc;
    sfs_flash_write write_fnc;
} sfs_t;

typedef struct {
    sfs_flash_erase erase_fnc;
    sfs_flash_read read_fnc;
    sfs_flash_write write_fnc;
} sfs_config_t;

bool sfs_init(sfs_t *sfs, sfs_config_t *config);
bool sfs_open(sfs_t *sfs, char *file_name, uint8_t file_name_size);
// bool sfs_write(sfs_t *sfs, uint8_t *data, uint32_t size);
// bool sfs_read_line(sfs_t *sfs, uint8_t *buffer, uint16_t buffer_size);


#define SFS_FILE_INIT_DEFAULT() \
    {                           \
        .name = {0},            \
        .start_address = 0,     \
        .end_address = 0,       \
    }                           \

#endif