# mbed-cmake requires at least CMake 3.12
cmake_minimum_required(VERSION 3.12)

# load project upload methods (must be before mbed-cmake is loaded)
# -------------------------------------------------------------
include(cmake/UploadMethodConfig.cmake)

# load compilers and flags
# -------------------------------------------------------------

include(mbed-cmake/mbed-cmake.cmake)

project(KX134-Driver LANGUAGES C CXX)

# recurse to subdirectories
# -------------------------------------------------------------

# add_subdirectory(KX134)
add_mbed_executable(kx134_example main.cpp stm32h7_hal_msp.c)

# target_link_libraries(kx134_example KX134)

# build report
# -------------------------------------------------------------

mbed_cmake_print_build_report()
