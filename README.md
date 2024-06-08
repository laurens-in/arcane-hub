# ARCANE Hub

This repository contains a prototype application for an arcane hub.

## Dependencies

To build this project you need a working `gcc-arm-none-eabi` toolchain.

## Build

### Debug

To create a debug build run the following commands:

```sh
mkdir debug
cd debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

If you have a BlackMagic debugging probe you can debug the software using:

```sh
cmake --build . --target debug-bmp
```

Make sure you have [configured udev rules](https://github.com/blackmagic-debug/blackmagic/tree/main/driver)

### Release

To create a release build run the following commands:

```sh
mkdir release
cd releases
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

If you have a BlackMagic debugging probe you can flash the software using:

```sh
cmake --build . --target flash-bmp
```

Make sure you have [configured udev rules](https://github.com/blackmagic-debug/blackmagic/tree/main/driver)

### Documentation

To build the documentation you need `doxygen` installed.

```sh
doxygen Doxyfile
```
