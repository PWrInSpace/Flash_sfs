#include <gtest/gtest.h>
#include <iostream>
#include "sfs_wrapper.h"

TEST_F(FlashTest, Open_invalid_file_name) {
    char file_name[MAX_FILE_NAME_SIZE + 2];
    memset(file_name, 0x54, sizeof(file_name));
    file_name[sizeof(file_name) - 1] = '\0';
    
    sfs_file_t file;
    EXPECT_EQ(SFS_INVALID_FILE_NAME,
              sfs_open(this->file_system, &file, file_name));
}

TEST_F(FlashTest, Open_null_file_name) {
    sfs_file_t file;
    EXPECT_EQ(SFS_NULL_POINTER,
              sfs_open(this->file_system, &file, NULL));
}

TEST_F(FlashTest, Open_valid_file_name) {
    char file_name[] = "test";
    sfs_file_t file;
    EXPECT_EQ(SFS_OK,
              sfs_open(this->file_system, &file, file_name));

    // Check that file was created 
    EXPECT_EQ(true, this->checkFileStartAddress(&file, 0));
    EXPECT_EQ(true, this->checkFileEndAddress(&file, 0, FILE_INFO_SIZE));
    EXPECT_EQ(true, this->checkSectorFileName(0, file_name));
    EXPECT_EQ(true, this->checkSFSNextFreeSector(1));
}

TEST_F(FlashTest, Open_two_file) {
    char file_name[] = "file1";
    char file_name2[] = "file2";
    sfs_file_t file;
    sfs_file_t file2;
    EXPECT_EQ(SFS_OK,
              sfs_open(this->file_system, &file, file_name));
    EXPECT_EQ(SFS_OK,
              sfs_open(this->file_system, &file2, file_name2));

    // Check that file was created 
    EXPECT_EQ(true, this->checkFileStartAddress(&file, 0));
    EXPECT_EQ(true, this->checkSectorFileName(0, file_name));
    EXPECT_EQ(true, this->checkFileStartAddress(&file2, 1));
    EXPECT_EQ(true, this->checkSectorFileName(1, file_name2));
    EXPECT_EQ(true, this->checkSFSNextFreeSector(2));
}

TEST_F(FlashTest, Open_previous_created_file) {
    char file_name[] = "file1";
    char file_name2[] = "file2";
    sfs_file_t file;
    sfs_file_t file2;
    
    // create two files
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file2, file_name2));
    
    // close first file
    EXPECT_EQ(SFS_OK, sfs_close(this->file_system, &file));
    
    // reopen first file
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));
    EXPECT_EQ(true, this->checkFileStartAddress(&file, 0));
    EXPECT_EQ(true, this->checkSectorFileName(0, file_name));
    EXPECT_EQ(true, this->checkSFSNextFreeSector(2));
}

TEST_F(FlashTest, WearLevel_new_file) {
    char file_name[] = "file2";
    sfs_file_t file;

    EXPECT_EQ(true, this->setMemory(2, 0, 12, 10));
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));

    EXPECT_EQ(true, this->checkFileStartAddress(&file, 3));
    EXPECT_EQ(true, this->checkFileEndAddress(&file, 3, FILE_INFO_SIZE));
    EXPECT_EQ(true, this->checkSectorFileName(3, file_name));
    EXPECT_EQ(true, this->checkSFSNextFreeSector(4));
}

TEST_F(FlashTest, WearLevel_new_last_sector) {
    char file_name[] = "file2";
    sfs_file_t file;
    uint32_t nb_of_sectors = this->file_system->flash_size_bits / this->file_system->flash_sector_bits;

    EXPECT_EQ(true, this->setMemory(nb_of_sectors - 1, 0, 12, 10));
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));

    EXPECT_EQ(true, this->checkFileStartAddress(&file, 0));
    EXPECT_EQ(true, this->checkFileEndAddress(&file, 0, FILE_INFO_SIZE));
    EXPECT_EQ(true, this->checkSectorFileName(0, file_name));
    EXPECT_EQ(true, this->checkSFSNextFreeSector(1));
}

TEST_F(FlashTest, WearLevel_new_last_sector_with_data_in_the_middle_and_end) {
    char file_name[] = "file2";
    sfs_file_t file;
    uint32_t nb_of_sectors = this->file_system->flash_size_bits / this->file_system->flash_sector_bits;

    EXPECT_EQ(true, this->setMemory(nb_of_sectors - 1, 0, 12, 10));
    EXPECT_EQ(true, this->setMemory(3, 0, 12, 10));
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));

    EXPECT_EQ(true, this->checkFileStartAddress(&file, 0));
    EXPECT_EQ(true, this->checkFileEndAddress(&file, 0, FILE_INFO_SIZE));
    EXPECT_EQ(true, this->checkSectorFileName(0, file_name));
    EXPECT_EQ(true, this->checkSFSNextFreeSector(1));
}

TEST_F(FlashTest, Write_to_file) {
    char file_name[] = "file";
    sfs_file_t file;
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));

    uint8_t data[10] = {0x32};
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, sizeof(data)));
    EXPECT_EQ(true, this->checkFileEndAddress(&file, 0, FILE_INFO_SIZE + 2 + sizeof(data)));
}

TEST_F(FlashTest, Reopen_file_with_data) {
    char file_name[] = "file";
    sfs_file_t file;
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));

    uint8_t data[10] = {0x32};
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, sizeof(data)));

    EXPECT_EQ(SFS_OK, sfs_close(this->file_system, &file));
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));

    EXPECT_EQ(true, this->checkFileEndAddress(&file, 0, FILE_INFO_SIZE + 2 + sizeof(data)));
}

TEST_F(FlashTest, Reopen_file_with_data_2) {
    char file_name[] = "file";
    sfs_file_t file;
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));

    uint8_t data[10] = {0x32};
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, sizeof(data)));
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, sizeof(data)));

    EXPECT_EQ(SFS_OK, sfs_close(this->file_system, &file));
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));

    EXPECT_EQ(true, this->checkFileEndAddress(&file, 0, FILE_INFO_SIZE + 2*(2 + sizeof(data))));
}

TEST_F(FlashTest, Write_end_of_sector_edge_case) {
    char file_name[] = "file";
    sfs_file_t file;
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));
    uint32_t sector_free_size = this->file_system->flash_sector_bits - FILE_INFO_SIZE;
    uint32_t data_size = sector_free_size - END_OF_SECTOR_SIZE - DATA_LEN_SIZE;
    // uint8_t data[120] = {0x12};
    uint8_t *data = new uint8_t[data_size];
    (void) memset(data, 0x12, data_size);
    // EXPECT_EQ(true, this->write2Bytes(0, FILE_INFO_SIZE, data_size));
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, data_size));
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, data_size));
    delete[] data;
    
    EXPECT_EQ(true, this->checkSectorFileName(0, file_name));
    EXPECT_EQ(true, this->checkSectorFileName(1, file_name));
    EXPECT_EQ(true, this->checkFileStartAddress(&file, 0));
    EXPECT_EQ(true, this->checkFileEndAddress(&file, 1, FILE_INFO_SIZE + data_size + DATA_LEN_SIZE));

}

TEST_F(FlashTest, Write_end_of_sector) {
    char file_name[] = "file";
    sfs_file_t file;
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));
    uint32_t sector_free_size = this->file_system->flash_sector_bits - FILE_INFO_SIZE;
    uint32_t data_size = sector_free_size - END_OF_SECTOR_SIZE - DATA_LEN_SIZE - 20;
    // uint8_t data[120] = {0x12};
    uint8_t *data = new uint8_t[data_size];
    (void) memset(data, 0x12, data_size);
    // EXPECT_EQ(true, this->write2Bytes(0, FILE_INFO_SIZE, data_size));
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, data_size));
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, data_size));
    delete[] data;
    
    EXPECT_EQ(true, this->checkSectorFileName(0, file_name));
    EXPECT_EQ(true, this->checkSectorFileName(1, file_name));
    EXPECT_EQ(true, this->checkFileStartAddress(&file, 0));
    EXPECT_EQ(true, this->checkFileEndAddress(&file, 1, FILE_INFO_SIZE + data_size - 18 + DATA_LEN_SIZE));
}

TEST_F(FlashTest, Read_from_file) {
    char file_name[] = "file";
    sfs_file_t file;
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));

    uint8_t data[] = "Hello I am under the water\n";
    uint8_t ret_buffer[sizeof(data)];
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, sizeof(data)));
    EXPECT_EQ(SFS_OK, sfs_read_line(this->file_system, &file, ret_buffer, sizeof(ret_buffer))); 
    EXPECT_EQ(true, this->arrayEqual(data, ret_buffer, sizeof(data)));
}

TEST_F(FlashTest, Read_check_eof) {
    char file_name[] = "file";
    sfs_file_t file;
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));

    uint8_t data[] = "Hello I am under the water\n";
    uint8_t ret_buffer[sizeof(data)];
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, sizeof(data)));
    EXPECT_EQ(SFS_OK, sfs_read_line(this->file_system, &file, ret_buffer, sizeof(ret_buffer))); 
    EXPECT_EQ(SFS_EOF, sfs_read_line(this->file_system, &file, ret_buffer, sizeof(ret_buffer))); 
}

TEST_F(FlashTest, Read_twice_from_file) {
    char file_name[] = "file";
    sfs_file_t file;
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));

    uint8_t data[] = "Hello I am under the water\n";
    uint8_t data2[] = "Please Help Me :0\n";
    uint8_t ret_buffer[sizeof(data) + sizeof(data2)];
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, sizeof(data)));
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data2, sizeof(data2)));
    
    EXPECT_EQ(SFS_OK, sfs_read_line(this->file_system, &file, ret_buffer, sizeof(ret_buffer))); 
    EXPECT_EQ(SFS_OK, sfs_read_line(this->file_system, &file, ret_buffer, sizeof(ret_buffer))); 
    EXPECT_EQ(true, this->arrayEqual(data2, ret_buffer, sizeof(data2)));
}

TEST_F(FlashTest, Read_buffer_to_smool) {
    char file_name[] = "file";
    sfs_file_t file;
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));

    uint8_t data[] = "Hello I am under the water\n";
    uint8_t ret_buffer[sizeof(data) - 1];
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, sizeof(data)));
    
    EXPECT_EQ(SFS_BUFFER_SIZE, sfs_read_line(this->file_system, &file,
                                             ret_buffer, sizeof(ret_buffer))); 
}

TEST_F(FlashTest, Write_from_two_sectors) {
    char file_name[] = "file";
    sfs_file_t file;
    EXPECT_EQ(SFS_OK, sfs_open(this->file_system, &file, file_name));
    uint32_t sector_free_size = this->file_system->flash_sector_bits - FILE_INFO_SIZE;
    uint32_t data_size = sector_free_size - END_OF_SECTOR_SIZE - DATA_LEN_SIZE - 20;
    uint8_t *data = new uint8_t[data_size];
    uint8_t *ret = new uint8_t[data_size];
    (void) memset(data, 0x12, data_size);
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, data_size));
    EXPECT_EQ(SFS_OK, sfs_write(this->file_system, &file, data, data_size));

    EXPECT_EQ(SFS_OK, sfs_read_line(this->file_system, &file, ret, data_size));
    EXPECT_EQ(SFS_OK, sfs_read_line(this->file_system, &file, ret, data_size));
   
    delete[] data;
}