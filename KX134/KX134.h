/**
 * @author Jasper Swallen
 * @filename KX134.h
 *
 * @section DESCRIPTION
 *
 * Software Driver for KX134-1211 accelerometer
 *
 * Datasheet:
 *
 * http://kionixfs.kionix.com/en/document/AN101-Getting-Started.pdf
 */

#ifndef KX134_H
#define KX134_H

#include "mbed.h"

class KX134
{
  public:
    KX134(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName int1,
          PinName int2, PinName rst);
    bool init();

    void attemptToRead();

  private:
    SPI _spi;
    PinName _int1, _int2;
    DigitalOut _cs;
    DigitalOut _rst;

    // example init sequences
    void init_asynch_reading();
    void init_synch_reading(bool init_hw_int);
    void init_sample_buffer_bfi();
    void init_sample_buffer_wmi();
    void init_sample_buffer_trigger();
    void init_wake_up();
    void init_wake_up_and_back_to_sleep();
    void init_tilt_pos_face_detect();
    void init_face_detect();
    void init_tap();
    void init_free_fall();

    bool reset();

    void deselect();
    void select();

    void readRegister(uint8_t addr, uint8_t *buf, int size = 1);
    void writeRegister(uint8_t addr, uint8_t *data, uint8_t *buf, int size = 1);
    void writeRegisterOneByte(uint8_t addr, uint8_t data, uint8_t *buf);
};

#endif // KX134_H