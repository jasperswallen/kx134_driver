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
    _rst.write(0);

    _rst.write(1);
    _spi.frequency(SPI_FREQ);

    init_asynch_reading();

    return reset();
}

bool KX134::reset()
{
    // write registers to start reset
    uint8_t buf[1];
    uint8_t data[1] = {0x00};
    writeRegister(0x7F, data, buf);
    writeRegister(KX134_ADP_CNTL2, data, buf);
    data[0] = 0x80;
    writeRegister(KX134_ADP_CNTL2, data, buf);

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
    uint8_t data[1] = {0x00};
    uint8_t buf[1]; // garbage bit to write to
    writeRegister(KX134_ADP_CNTL1, data, buf);
    data[0] = 0x06;
    writeRegister(KX134_ODCNTL, data, buf);
    data[0] = 0xC0;
    writeRegister(KX134_ADP_CNTL1, data, buf);
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
    uint8_t data[1] = {0x00};
    uint8_t buf[1]; // garbage bit to write to
    writeRegister(KX134_ADP_CNTL1, data, buf);
    if(init_hw_int)
    {
        data[0] = 0x30;
        writeRegister(KX134_INC1, data, buf);
        data[0] = 0x10;
        writeRegister(KX134_INC4, data, buf);
    }
    data[0] = 0x06;
    writeRegister(KX134_ODCNTL, data, buf);
    data[0] = 0xE0;
    writeRegister(KX134_ADP_CNTL1, data, buf);
}

void KX134::deselect()
{
    _cs.write(1);
}

void KX134::select()
{
    _spi.lock();
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
