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

  private:
    SPI _spi;
    PinName _int1, _int2, _rst;

    // example init sequences
    bool init_asynch_reading();
    bool init_synch_reading(bool init_hw_interrupt);
    bool init_sample_buffer_bfi();
    bool init_sample_buffer_wmi();
    bool init_sample_buffer_trigger();
    bool init_wake_up();
    bool init_wake_up_and_back_to_sleep();
    bool init_tilt_pos_face_detect();
    bool init_face_detect();
    bool init_tap();
    bool init_free_fall();
};

#endif // KX134_H