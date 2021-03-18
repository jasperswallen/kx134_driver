add_library(KX134 KX134Base.cpp KX134SPI.cpp KX134I2C.cpp)
target_include_directories(KX134 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(KX134 mbed-os)
