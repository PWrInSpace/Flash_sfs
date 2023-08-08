#include <iostream>

extern "C" {
    #include "sfs/simple_file_system.h"
    #include "flash_mock/flash_mock.h"
}

int main(void) {
    std::cout << "Hello world" << std::endl;
    return 0;
}