#include <iostream>
#include "sfs/tools.h"

extern "C" {
    #include "flash_mock/flash_mock.h"
}

int main(void) {
    std::cout << "Hello world" << std::endl;
    return 0;
}