#include <gtest/gtest.h>

extern "C" {
    #include "flash_mock/flash_mock.h"
}

TEST(FlashMock, Init_pass_null) {
    EXPECT_EQ(false, flash_mock_init(NULL, SIZE_16MB, 16));
}

TEST(FlashMock, Init_sector_greater_than_size) {
    flash_mock_t dev;
    EXPECT_EQ(false, flash_mock_init(&dev, SIZE_16MB, MB_TO_BYTES(20)));
}

TEST(FlashMock, Init16MB_valid_secotrs) {
    flash_mock_t dev;
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    flash_mock_deinit(&dev);
}

TEST(FlashMock, Init16MB_invalid_secotrs) {
    flash_mock_t dev;
    EXPECT_EQ(false, flash_mock_init(&dev, SIZE_16MB, 15));
}

TEST(FlashMock, Init8MB_valid_secotrs) {
    flash_mock_t dev;
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_8MB, 8));
    flash_mock_deinit(&dev);
}

TEST(FlashMock, Init8MB_invalid_secotrs) {
    flash_mock_t dev;
    EXPECT_EQ(false, flash_mock_init(&dev, SIZE_8MB, 13));
}

TEST(FlashMock, check_size_16MB) {
    flash_mock_t dev;
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    EXPECT_EQ(16 * 1024 * 1024, dev.memory_size_bytes);
    flash_mock_deinit(&dev);
}

TEST(FlashMock, check_sector_size_16MB) {
    flash_mock_t dev;
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    EXPECT_EQ(16 * 1024, dev.sector_size_bytes);
    flash_mock_deinit(&dev);
}

TEST(FlashMock, check_memory_init) {
    flash_mock_t dev;
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    for (uint32_t i = 0; i < dev.memory_size_bytes; ++i) {
        EXPECT_EQ(ERASED_BYTE, dev.memory[i]);
    }
    flash_mock_deinit(&dev);
}

TEST(FlashMock, write_invalid_sector) {
    flash_mock_t dev;
    uint8_t data[15] = {0};
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    EXPECT_EQ(-1, flash_mock_write(&dev, 1024, 10, data, sizeof(data)));
    flash_mock_deinit(&dev);
}

TEST(FlashMock, write_invalid_address) {
    flash_mock_t dev;
    uint8_t data[15] = {0};
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    EXPECT_EQ(-1, flash_mock_write(&dev, 3, 16921, data, sizeof(data)));
    flash_mock_deinit(&dev);
}

TEST(FlashMock, write_out_of_memory) {
    flash_mock_t dev;
    uint8_t data[15] = {0};
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    EXPECT_EQ(4, flash_mock_write(&dev, 1023, 16380, data, sizeof(data)));
    flash_mock_deinit(&dev);
}

TEST(FlashMock, read_invalid_address) {
    flash_mock_t dev;
    uint8_t data[15] = {0};
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    EXPECT_EQ(-1, flash_mock_read(&dev, MB_TO_BYTES(16), data, sizeof(data)));
    flash_mock_deinit(&dev);
}

TEST(FlashMock, read_valid_address) {
    flash_mock_t dev;
    uint8_t data[15] = {0};
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    EXPECT_EQ(sizeof(data), flash_mock_read(&dev, 23, data, sizeof(data)));
    for (size_t i = 0; i < sizeof(data); ++i) {
        EXPECT_EQ(ERASED_BYTE, data[i]);
    }
    flash_mock_deinit(&dev);
}

TEST(FlashMock, read_out_of_memory) {
    flash_mock_t dev;
    uint8_t data[15] = {0};
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    EXPECT_EQ(4, flash_mock_read(&dev, MB_TO_BYTES(16) - 5, data, sizeof(data)));
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(ERASED_BYTE, data[i]);
    }

    for (size_t i = 4; i < sizeof(data); ++i) {
        EXPECT_EQ(0, data[i]);
    }

    flash_mock_deinit(&dev);
}

TEST(FlashMock, write_read) {
    flash_mock_t dev;
    uint8_t data[15] = {0x12, 0x43, 0x34, 0x65, 0x11, 0x12, 0x43, 0x34, 0x65, 0x11, 0x12, 0x43, 0x34, 0x65, 0x11};
    uint8_t output[15] = {0};

    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    EXPECT_EQ(sizeof(data), flash_mock_write(&dev, 2, 10, data, sizeof(data)));
    EXPECT_EQ(sizeof(output), flash_mock_read(&dev, KB_TO_BYTES(16) * 2 + 10,
                                                output, sizeof(output)));

    for (size_t i = 0 ; i < sizeof(data); ++i) {
        EXPECT_EQ(data[i], output[i]);
    }

    flash_mock_deinit(&dev);
}

TEST(FlashMock, multi_write) {
    flash_mock_t dev;
    uint8_t data[3] = {0x12, 0x43, 0x34};
    uint8_t data2[3] = {0x54, 0x87, 0x91};
    uint8_t output[3] = {0};

    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    EXPECT_EQ(sizeof(data), flash_mock_write(&dev, 0, 5, data, sizeof(data)));
    EXPECT_EQ(sizeof(data2), flash_mock_write(&dev, 0, 5, data2, sizeof(data2)));
    EXPECT_EQ(sizeof(output), flash_mock_read(&dev, 5, output, sizeof(output)));

    for (size_t i = 0 ; i < sizeof(data); ++i) {
        EXPECT_NE(data, output);
    }

    // Check rest of memory
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(dev.memory[i], ERASED_BYTE);
    }

    // Check rest of memory
    for (size_t i = 8; i < dev.memory_size_bytes; ++i) {
        EXPECT_EQ(dev.memory[i], ERASED_BYTE);
    }

    flash_mock_deinit(&dev);
}

TEST(FlashMock, erase_invalid_sector) {
    flash_mock_t dev;
    uint32_t secotrs = 1024;
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));

    EXPECT_EQ(false, flash_mock_erase_sector(&dev, secotrs));
}

TEST(FlashMock, erase_sector) {
    flash_mock_t dev;
    uint8_t data = 0x12;
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    (void) memset(dev.memory, data, dev.memory_size_bytes);

    EXPECT_EQ(true, flash_mock_erase_sector(&dev, 1));

    for (uint32_t i = 0; i < dev.sector_size_bytes; ++i) {
        EXPECT_EQ(data, dev.memory[i]);
    }

    for (uint32_t i = dev.sector_size_bytes; i < dev.sector_size_bytes * 2; ++i) {
        EXPECT_EQ(ERASED_BYTE, dev.memory[i]);
    }

    for (uint32_t i = dev.sector_size_bytes * 2; i < dev.memory_size_bytes; ++i) {
        EXPECT_EQ(data, dev.memory[i]);
    }
}