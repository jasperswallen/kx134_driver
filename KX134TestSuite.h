//
// Created by Jasper Swallen on 2-15-21
//

#ifndef KX134TESTSUITE_H
#define KX134TESTSUITE_H

#include "mbed.h"

#ifdef USING_I2C
#include "KX134I2C.h"

#define PIN_I2C_SDA NC
#define PIN_I2C_SCL NC

KX134I2C new_accel(PIN_I2C_SDA, PIN_I2C_SCL, 0x1F);
#else
#include "KX134SPI.h"

#define PIN_SPI_MOSI PB_5
#define PIN_SPI_MISO PB_4
#define PIN_SPI_SCK PB_3
#define PIN_SPI_CS PA_4

KX134SPI new_accel(PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_SCK, PIN_SPI_CS);
#endif
class KX134TestSuite
{
public:
    void test_existence();
    void set_hz();
    void set_range();
    void test_stddev();
};

#endif
