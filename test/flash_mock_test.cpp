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
}

TEST(FlashMock, Init16MB_invalid_secotrs) {
    flash_mock_t dev;
    EXPECT_EQ(false, flash_mock_init(&dev, SIZE_16MB, 15));
}

TEST(FlashMock, Init8MB_valid_secotrs) {
    flash_mock_t dev;
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_8MB, 8));
}

TEST(FlashMock, Init8MB_invalid_secotrs) {
    flash_mock_t dev;
    EXPECT_EQ(false, flash_mock_init(&dev, SIZE_8MB, 13));
}

TEST(FlashMock, check_size_16MB) {
    flash_mock_t dev;
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    EXPECT_EQ(16 * 1024 * 1024, dev.memory_size_bytes);
}

TEST(FlashMock, check_sector_size_16MB) {
    flash_mock_t dev;
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    EXPECT_EQ(16 * 1024, dev.sector_size_bytes);
}

TEST(FlashMock, check_memory_init) {
    flash_mock_t dev;
    EXPECT_EQ(true, flash_mock_init(&dev, SIZE_16MB, 16));
    for (uint32_t i = 0; i < dev.memory_size_bytes; ++i) {
        EXPECT_EQ(ERASED_BYTE, dev.memory[i]);
    }
}

// TEST(FlashMock, write_invalid_sector) {
//     EXPECT_EQ(0, 0);
// }

// TEST(FlashMock, write_invalid_address) {
//     EXPECT_EQ(0, 0);
// }

// TEST(FlashMock, write_over_memory) {
//     EXPECT_EQ(0, 0);
// }

// TEST(FlashMock, read_invalid_sector) {
//     EXPECT_EQ(0, 0);
// }

// TEST(FlashMock, read_invalid_address) {
//     EXPECT_EQ(0, 0);
// }

// TEST(FlashMock, write_read) {
//     EXPECT_EQ(0, 0);
// }