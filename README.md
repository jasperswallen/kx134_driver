# KX134-1211 Driver

## About

This is a C++ driver for the KX134-1211 tri-axis accelerometer. It was developed
on a NUCLEO-H743ZI2 development board with Mbed OS 6

## Running

0. Specify correct pins and serial protocol. The pins are defined in
`KX134TestSuite.h`, and by default SPI mode is used. To use I2C, `#define USING_I2C`.
1. Configure `mbed-cmake` for your board. If you are using the NUCLEO-H743ZI2
dev board, this can be done automatically by running
`python3 configure_mbed_cmake.py`. Otherwise, manually configure mbed-cmake for
your board with the `configure_for_target.py` script. This would look like
`python3 mbed-cmake/configure_for_target.py -a mbed_app.json -x . -i .mbedignore <target name>`
2. Create a build directory and configure CMake. For non-Windows platforms, this
can typically be done with `cmake .. -DUPLOAD_METHOD=<upload method>`. For
Windows, you may need to specify your build tool (`cmake .. -G"MinGW Makefiles"`,
for example).
3. Build and flash. Run `make flash-kx134_example` to flash to your connected target.
