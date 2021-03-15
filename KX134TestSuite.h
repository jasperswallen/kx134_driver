//
// Created by Jasper Swallen on 2-15-21
//

#ifndef KX134TESTSUITE_H
#define KX134TESTSUITE_H

#include "KX134.h"
#include "mbed.h"

#define PIN_SPI_MOSI PB_5
#define PIN_SPI_MISO PB_4
#define PIN_SPI_CS PA_4
#define PIN_SPI_SCK PB_3
#define PIN_INT1 PA_0 // placeholder
#define PIN_INT2 PA_0 // placeholder
#define PIN_RST PA_0  // placeholder

class KX134TestSuite
{
  public:
    void test_existance();
    void set_hz();
    void set_range();
    void test_stddev();
};

#endif
