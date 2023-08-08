# Flash_sfs
Simple, wear-leveled file system for SPI flash

## Compile
```
cmake -S . -B build
cmake --build build -j 12
```

## Run example
```
./build/executables/[name_of_exec]
```

## Run tests
```
cd build && ctest && cd..
```
