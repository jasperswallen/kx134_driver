#include "KX134.h"

// register map
#define KX134_MAN_ID 0x00
#define KX134_PART_ID 0x01
#define KX134_XADP_L 0x02
#define KX134_XADP_H 0x03
#define KX134_YADP_L 0x04
#define KX134_YADP_H 0x05
#define KX134_ZADP_L 0x06
#define KX134_ZADP_H 0x07
#define KX134_XOUT_L 0x08
#define KX134_XOUT_H 0x09
#define KX134_YOUT_L 0x0A
#define KX134_YOUT_H 0x0B
#define KX134_ZOUT_L 0x0C
#define KX134_ZOUT_H 0x0D
#define KX134_COTR 0x12
#define KX134_WHO_AM_I 0x13
#define KX134_TSCP 0x14
#define KX134_TSPP 0x15
#define KX134_INS1 0x16
#define KX134_INS2 0x17
#define KX134_INS3 0x18
#define KX134_STATUS_REG 0x19
#define KX134_INT_REL 0x1A
#define KX134_CNTL1 0x1B
#define KX134_CNTL2 0x1C
#define KX134_CNTL3 0x1D
#define KX134_CNTL4 0x1E
#define KX134_CNTL5 0x1F
#define KX134_CNTL6 0x20
#define KX134_ODCNTL 0x21
#define KX134_INC1 0x22
#define KX134_INC2 0x23
#define KX134_INC3 0x24
#define KX134_INC4 0x25
#define KX134_INC5 0x26
#define KX134_INC6 0x27
#define KX134_TILT_TIMER 0x29
#define KX134_TDTRC 0x2A
#define KX134_TDTC 0x2B
#define KX134_TTH 0x2C
#define KX134_TTL 0x2D
#define KX134_FTD 0x2E
#define KX134_STD 0x2F
#define KX134_TLT 0x30
#define KX134_TWS 0x31
#define KX134_FFTH 0x32
#define KX134_FFC 0x33
#define KX134_FFCNTL 0x34
#define KX134_TILT_ANGLE_LL 0x37
#define KX134_TILT_ANGLE_HL 0x38
#define KX134_HYST_SET 0x39
#define KX134_LP_CNTL1 0x3A
#define KX134_LP_CNTL2 0x3B
#define KX134_WUFTH 0x49
#define KX134_BTSWUFTH 0x4A
#define KX134_BTSTH 0x4B
#define KX134_BTSC 0x4C
#define KX134_WUFC 0x4D
#define KX134_SELF_TEST 0x5D
#define KX134_BUF_CNTL1 0x5E
#define KX134_BUF_CNTL2 0x5F
#define KX134_BUF_STATUS_1 0x60
#define KX134_BUF_STATUS_2 0x61
#define KX134_BUF_CLEAR 0x62
#define KX134_BUF_READ 0x63
#define KX134_ADP_CNTL1 0x64
#define KX134_ADP_CNTL2 0x65
#define KX134_ADP_CNTL3 0x66
#define KX134_ADP_CNTL4 0x67
#define KX134_ADP_CNTL5 0x68
#define KX134_ADP_CNTL6 0x69
#define KX134_ADP_CNTL7 0x6A
#define KX134_ADP_CNTL8 0x6B
#define KX134_ADP_CNTL9 0x6C
#define KX134_ADP_CNTL10 0x6D
#define KX134_ADP_CNTL11 0x6E
#define KX134_ADP_CNTL12 0x6F
#define KX134_ADP_CNTL13 0x70
#define KX134_ADP_CNTL14 0x71
#define KX134_ADP_CNTL15 0x72
#define KX134_ADP_CNTL16 0x73
#define KX134_ADP_CNTL17 0x74
#define KX134_ADP_CNTL18 0x75
#define KX134_ADP_CNTL19 0x76

#define SPI_FREQ 1000000

KX134::KX134(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName int1,
             PinName int2, PinName rst)
    : _spi(mosi, miso, sclk), _int1(int1), _int2(int2), _cs(cs), _rst(rst)
{
    printf("Creating KX134-1211\r\n");
}

bool KX134::init()
{
    printf("Initing KX134-1211\r\n");
    deselect();

    _spi.frequency(SPI_FREQ);
    _spi.format(16, 0); //! not sure about 2nd arg

    init_asynch_reading();

    return reset();
}

// debug function
void KX134::attemptToRead()
{
    // uint8_t whoami[6];
    // whoami[5] = '\0';
    // readRegister(KX134_MAN_ID, whoami, 5);
    // printf("WAI: %s\r\n", whoami);
    // printf("0x%X, 0x%X, 0x%X, 0x%X, 0x%X\r\n", whoami[0], whoami[1],
    // whoami[2],
    //        whoami[3], whoami[4]);

    select();
    int w = _spi.write(KX134_WHO_AM_I), r1 = _spi.write(0x00),
        r2 = _spi.write(0x00), r3 = _spi.write(0x00), r4 = _spi.write(0x00),
        r5 = _spi.write(0x00), r6 = _spi.write(0x00), r7 = _spi.write(0x00),
        r8 = _spi.write(0x00);
    printf("w: 0x%X r: 0x%X r: 0x%X r: 0x%X r: 0x%X "
           "r: 0x%X r: 0x%X r: 0x%X r: "
           "0x%X\r\n",
           w, r1, r2, r3, r4, r5, r6, r7, r8);

    deselect();

    // select();

    // char buf[8];
    // char tx_buff[1] = {KX134_WHO_AM_I};
    // int rsp = _spi.write(tx_buff, 1, buf, 8);
    // printf("Read: %s: 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X,
    // 0x%X\r\n",
    //        buf, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6],
    //        buf[7], rsp);
}

bool KX134::reset()
{
    // write registers to start reset
    uint8_t buf[1];
    writeRegisterOneByte(0x7F, 0x00, buf);
    writeRegisterOneByte(KX134_CNTL2, 0x00, buf);
    writeRegisterOneByte(KX134_CNTL2, 0x80, buf);

    // software reset time
    wait_us(2000);

    // verify WHO_I_AM
    uint8_t whoami[5];
    readRegister(KX134_WHO_AM_I, whoami, 5);
    printf("WAI: %s\r\n", whoami);
    printf("0x%X, 0x%X, 0x%X, 0x%X, 0x%X\r\n", whoami[0], whoami[1], whoami[2],
           whoami[3], whoami[4]);

    if(!(whoami[0] == 0x3D && whoami[1] == 0x4C && whoami[2] == 0x4C &&
         whoami[3] == 0x46 && whoami[4] == 0x4D))
    {
        return false; // WHO_AM_I is incorrect
    }

    // verify COTR
    readRegister(KX134_COTR, buf);
    printf("COTR: 0x%X", buf[0]);
    if(buf[0] != 0x55)
    {
        return false; // COTR is incorrect
    }

    return true;
}

/* Example Initializations
 * ====================================================
 */

/* This example configures and enables the accelerometer to start outputting
 * sensor data that can be asynchronously read from the output registers.
 *
 * Acceleration data can now be read from the XOUT_L, XOUT_H, YOUT_L, YOUT_H,
 * ZOUT_L, and ZOUT_H registers in 2’s complement format asynchronously. To
 * reduce the duplicate sensor data, wait at least 1/ODR period before reading
 * the next sample.
 */
void KX134::init_asynch_reading()
{
    uint8_t buf[1]; // garbage bit to write to
    writeRegisterOneByte(KX134_CNTL1, 0x00, buf);
    writeRegisterOneByte(KX134_ODCNTL, 0x06, buf);
    writeRegisterOneByte(KX134_CNTL1, 0xC0, buf);
}

/* This example configures and enables the accelerometer to start outputting
 * sensor data with a synchronous signal (DRDY) and data can read from the
 * output registers.
 *
 * If no HW interrupt:
 * Acceleration data can now be read from the XOUT_L, XOUT_H, YOUT_L, YOUT_H,
 * ZOUT_L, and ZOUT_H registers in 2’s complement format synchronously when the
 * DRDY bit is set (0x10) in the Interrupt Status 2 Register (INS2).
 * if (INS2 & 0x10)
 * {
 *     // read output registers
 * }
 *
 * If HW interrupt:
 * Acceleration data can now be read from the XOUT_L, XOUT_H, YOUT_L, YOUT_H,
 * ZOUT_L, and ZOUT_H registers in 2’s complement format synchronously following
 * the rising edge of INT1.
 */
void KX134::init_synch_reading(bool init_hw_int)
{
    uint8_t buf[1]; // garbage bit to write to
    writeRegisterOneByte(KX134_CNTL1, 0x00, buf);
    if(init_hw_int)
    {
        writeRegisterOneByte(KX134_INC1, 0x30, buf);
        writeRegisterOneByte(KX134_INC4, 0x10, buf);
    }
    writeRegisterOneByte(KX134_ODCNTL, 0x06, buf);
    writeRegisterOneByte(KX134_CNTL1, 0xE0, buf);
}

/* This example configures enables the accelerometer to start outputting sensor
 * data to the internal buffer until full. When the buffer is full, a hardware
 * interrupt is generated and data can then be read from the buffer. The mode of
 * operation is first in, first out (FIFO) below.
 *
 * Once a Buffer-Full Interrupt is issued on INT1 pin, acceleration data can
 * then be read from the Buffer Read (BUF_READ) register at address 0x63 in 2’s
 * complement format. Since the resolution of the samples data was set to
 * 16-bit, the data is recorded in the following order: X_L, X_H, Y_L, Y_H, Z_L
 * and Z_H with the oldest data point read first as the buffer is in FIFO mode.
 * The full buffer contains 516 bytes of data, which corresponds to 86 unique
 * acceleration data samples. (Note: With BRES=0 (8-bit resolution), in
 * BUF_CNTL2, it is possible to collect 171 samples or 513 bytes of data).
 */
void KX134::init_sample_buffer_bfi()
{
    uint8_t buf[1]; // garbage bit to write to
    writeRegisterOneByte(KX134_CNTL1, 0x00, buf);
    writeRegisterOneByte(KX134_ODCNTL, 0x06, buf);
    writeRegisterOneByte(KX134_INC1, 0x30, buf);
    writeRegisterOneByte(KX134_INC4, 0x40, buf);
    writeRegisterOneByte(KX134_BUF_CNTL2, 0xE0, buf);
    writeRegisterOneByte(KX134_CNTL1, 0xE0, buf);
}

/* This example configures enables the accelerometer to start outputting sensor
 * data to the internal buffer until a watermark is reached. When the watermark
 * is reached, a hardware interrupt is generated and data can then be read from
 * the buffer. The mode of operation is first in, first out (FIFO) below.
 *
 * Once a Buffer-Full Interrupt is issued on INT1 pin, acceleration data can
 * then be read from the Buffer Read (BUF_READ) register at address 0x63 in 2’s
 * complement format. The data is recorded in the following order: X_L, X_H,
 * Y_L, Y_H, Z_L and Z_H (16-bit mode) with the oldest data point read first as
 * the buffer is in FIFO mode. The full buffer contains 258 bytes of data, which
 * corresponds to 43 unique acceleration data samples.
 */
void KX134::init_sample_buffer_wmi()
{
    uint8_t buf[1]; // garbage bit to write to
    writeRegisterOneByte(KX134_CNTL1, 0x00, buf);
    writeRegisterOneByte(KX134_ODCNTL, 0x06, buf);
    writeRegisterOneByte(KX134_INC1, 0x30, buf);
    writeRegisterOneByte(KX134_INC4, 0x20, buf);
    writeRegisterOneByte(KX134_BUF_CNTL1, 0x2B, buf);
    writeRegisterOneByte(KX134_BUF_CNTL2, 0xE0, buf);
    writeRegisterOneByte(KX134_CNTL1, 0xE0, buf);
}

/* This example configures enables the accelerometer to start filling sensor
 * data to the internal buffer. Prior to a trigger event, once the watermark
 * setting is reached, old data will be discarded and new data will be added.
 * Following a trigger event, data will continue to fill until the buffer is
 * full. A hardware interrupt is generated when the buffer is full and data can
 * then be read from the buffer. The mode of operation is first in, first out
 * (FIFO). The purpose of this example is to show how data can be captured both
 * before and after an event (external trigger, tap, wakeup, freefall).
 *
 * Provide some time for the buffer to fill to the configured threshold.
 * Assuming the default ODR was used, it should take approximately 0.86 seconds.
 * After this time, trigger a wakeup event by shaking the unit above the
 * configured threshold and timing settings. Next, wait for the Buffer-Full
 * Interrupt. Once Buffer-Full Interrupt is issued on INT1 pin, acceleration
 * data can then be read from the Buffer Read (BUF_READ) register at address
 * 0x63 in 2’s complement format. Since the resolution of the samples data was
 * set to 16-bit, both high and low bytes of each sample were stored in the
 * buffer, and recorded in the following order: X_L, X_H, Y_L, Y_H, Z_L, Z_H
 * with the oldest data point read first as it is a FIFO buffer. The full buffer
 * contains 516 bytes of data, which corresponds to 86 unique acceleration data
 * samples. The data set will include all the data prior to the trigger event,
 * plus all the data after the event.
 */
void KX134::init_sample_buffer_trigger()
{
    uint8_t buf[1]; // garbage bit to write to
    writeRegisterOneByte(KX134_CNTL1, 0x00, buf);
    writeRegisterOneByte(KX134_ODCNTL, 0x06, buf);
    writeRegisterOneByte(KX134_INC1, 0x30, buf);
    writeRegisterOneByte(KX134_INC4, 0x40, buf);
    writeRegisterOneByte(KX134_BUF_CNTL1, 0x2B, buf);
    writeRegisterOneByte(KX134_BUF_CNTL2, 0xE2, buf);
    writeRegisterOneByte(KX134_INC2, 0x3F, buf);
    writeRegisterOneByte(KX134_CNTL3, 0xAE, buf);
    writeRegisterOneByte(KX134_CNTL4, 0x60, buf);
    writeRegisterOneByte(KX134_CNTL5, 0x01, buf);
    writeRegisterOneByte(KX134_WUFC, 0x05, buf);
    writeRegisterOneByte(KX134_WUFTH, 0x20,
                         buf); // unclear in datasheet - either 0x20 or 0x80
    writeRegisterOneByte(KX134_BTSWUFTH, 0x00, buf);
    writeRegisterOneByte(KX134_CNTL1, 0xE0, buf);
}

/* This example configures and enables the accelerometer to detect wake-up
 * events using an external interrupt pin with Back-to-Sleep function disabled.
 *
 */
void KX134::init_wake_up()
{
    uint8_t buf[1];
    writeRegisterOneByte(KX134_CNTL1, 0x00, buf);
    writeRegisterOneByte(KX134_ODCNTL, 0x06, buf);
    writeRegisterOneByte(KX134_INC1, 0x30, buf);
    writeRegisterOneByte(KX134_INC4, 0x02, buf);
    writeRegisterOneByte(KX134_INC1, 0x3F, buf);
    writeRegisterOneByte(KX134_CNTL3, 0xAE, buf);
    writeRegisterOneByte(KX134_CNTL4, 0x60, buf);
    writeRegisterOneByte(KX134_CNTL5, 0x01, buf);
    writeRegisterOneByte(KX134_WUFC, 0x05, buf);
    writeRegisterOneByte(KX134_WUFTH, 0x20, buf); // see prev func
    writeRegisterOneByte(KX134_BTSWUFTH, 0x00, buf);
    writeRegisterOneByte(KX134_CNTL1, 0xE0, buf);

    /* Monitor the physical interrupt INT1 of the accelerometer, if the
     * acceleration input profile satisfies the criteria previously established
     * for the 0.5g motion detect threshold level in both positive and negative
     * directions of the X, Y, Z axis for more than 0.1 second, then there
     * should be positive latched interrupt present. Also, the WUFS bit in
     * Interrupt Status 3 (INS3) will be set to indicate the wake-up interrupt
     * has fired. INS3 also provides information regarding which axis/axes
     * caused the wakeup interrupt. Finally, WAKE bit in Status (STAT) will also
     * be set to indicate the sensor is in WAKE mode.
     *
     * if (INS3 & 0x80)
     * {
     *     // handle wakeup event
     * }
     */

    readRegister(KX134_INT_REL, buf);
    writeRegisterOneByte(KX134_CNTL5, 0x01, buf);
}
void KX134::init_wake_up_and_back_to_sleep()
{
    uint8_t buf[1];
    writeRegisterOneByte(KX134_CNTL1, 0x00, buf);
    writeRegisterOneByte(KX134_ODCNTL, 0x06, buf);
    writeRegisterOneByte(KX134_INC1, 0x30, buf);
    writeRegisterOneByte(KX134_INC4, 0x0A, buf);
    writeRegisterOneByte(KX134_INC2, 0x3F, buf);
    writeRegisterOneByte(KX134_CNTL3, 0xAE, buf);
    writeRegisterOneByte(KX134_CNTL4, 0x76, buf);
    writeRegisterOneByte(KX134_CNTL5, 0x01, buf);
    writeRegisterOneByte(KX134_BTSC, 0x05, buf);
    writeRegisterOneByte(KX134_WUFC, 0x05, buf);
    writeRegisterOneByte(KX134_WUFTH, 0x20, buf); // see above
    writeRegisterOneByte(KX134_BTSWUFTH, 0x00, buf);
    writeRegisterOneByte(KX134_BTSTH, 0x20, buf);
    writeRegisterOneByte(KX134_CNTL1, 0xE0, buf);

    /* Monitor the physical interrupt INT1 of the accelerometer, if the
     * acceleration input profile satisfies the criteria previously established
     * for the 0.5g motion detect threshold level in both positive and negative
     * directions of the X, Y, Z axis for more than 0.1 second, then there
     * should be positive latched interrupt present. Also, the WUFS bit in
     * Interrupt Status 3 (INS3) will be set to indicate the wake-up interrupt
     * has fired. INS3 also provides information regarding which axis/axes
     * caused the wakeup interrupt. Finally, WAKE bit in Status (STAT) will also
     * be set to indicate the sensor is in WAKE mode.
     *
     * if (INS3 & 0x80)
     * {
     *     // handle wakeup event
     * }
     */

    readRegister(KX134_INT_REL, buf);
}
void KX134::init_tilt_pos_face_detect() {}
void KX134::init_face_detect() {}
void KX134::init_tap() {}
void KX134::init_free_fall() {}

/* Helper Functions
 * ====================================================
 */

void KX134::deselect()
{
    _cs.write(1);
}

void KX134::select()
{
    _cs.write(0);
}

void KX134::readRegister(uint8_t addr, uint8_t *buf, int size)
{
    select();

    _spi.write(addr); // select the register
    for(int i = 0; i < size; ++i)
    {
        buf[i] = _spi.write(0x00);
    }

    deselect();
}

void KX134::writeRegister(uint8_t addr, uint8_t *data, uint8_t *buf, int size)
{
    select();

    _spi.write(addr); // select register
    for(int i = 0; i < size; ++i)
    {
        buf[i] = _spi.write(data[i]);
    }

    deselect();
}

void KX134::writeRegisterOneByte(uint8_t addr, uint8_t data, uint8_t *buf)
{
    uint8_t _data[1] = {data};
    writeRegister(addr, _data, buf);
}
