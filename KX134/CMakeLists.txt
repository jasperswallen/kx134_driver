add_library(KX134 KX134.cpp)
target_include_directories(KX134 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(KX134 mbed-os)