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

#define SPI_FREQ 100000

char defaultBuffer[2] = {0}; // allows calling writeRegisterOneByte
                             // without buf argument

/* Writes one byte to a register
 */
void KX134::writeRegisterOneByte(uint8_t addr, uint8_t data,
                                 char *buf = defaultBuffer)
{
    uint8_t _data[1] = {data};
    writeRegister(addr, _data, buf);
}

KX134::KX134(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName int1,
             PinName int2, PinName rst)
    : _spi(mosi, miso, sclk), _int1(int1), _int2(int2), _cs(cs), _rst(rst)
{
    printf("Creating KX134-1211\r\n");

    // set default values for settings variables
    resStatus = 1;   // high performance mode
    drdyeStatus = 0; // Data Ready Engine disabled
    gsel1Status = 0; // +-8g bit 1
    gsel0Status = 0; // +-8g bit 0
    tdteStatus = 0;  // Tap/Double-Tap engine disabled
    tpeStatus = 0;   // Tilt Position Engine disabled

    iirBypass = 0; // IIR filter is not bypassed, i.e. filtering is applied
    lpro = 0;      // IIR filter corner frequency set to ODR/9
    fstup = 0;     // Fast Start is disabled
    osa3 = 0;      // Output Data Rate bits
    osa2 = 1;      // default is 50hz
    osa1 = 1;
    osa0 = 0;

    registerWritingEnabled = 0;

    deselect();
}

bool KX134::init()
{
    printf("Initing KX134-1211\r\n");
    deselect();

    _spi.frequency(SPI_FREQ);
    _spi.format(8, 1); //! not sure about

    return reset();
}

bool KX134::reset()
{
    // write registers to start reset
    writeRegisterOneByte(0x7F, 0x00);
    writeRegisterOneByte(KX134_CNTL2, 0x00);
    writeRegisterOneByte(KX134_CNTL2, 0x80);

    // software reset time
    wait_us(2000);

    // verify WHO_I_AM
    char whoami[5];
    readRegister(KX134_WHO_AM_I, whoami);
    printf("WHO_AM_I: 0x%X, 0x%X\r\n", whoami[0], whoami[1]);

    if(whoami[1] != 0x46)
    {
        return false; // WHO_AM_I is incorrect
    }

    // verify COTR
    char cotr[2];
    readRegister(KX134_COTR, cotr);
    printf("COTR: 0x%X, 0x%X\r\n", cotr[0], cotr[1]);
    if(cotr[1] != 0x55)
    {
        return false; // COTR is incorrect
    }

    return true;
}

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

void KX134::readRegister(char addr, char *rx_buf, int size)
{
    select();

    char tx_buf[1] = {addr | (1 << 7)};

    _spi.write(tx_buf, 1, rx_buf, size);

    deselect();
}

void KX134::writeRegister(uint8_t addr, uint8_t *data, char *rx_buf, int size)
{
    select();

    _spi.write(addr); // select register
    for(int i = 0; i < size; ++i)
    {
        rx_buf[i] = _spi.write(data[i]);
    }

    deselect();
}

/* Returns a 16 bit signed integer representation of a 2 address read
 * Assumes 2s Complement
 */
int16_t KX134::read16BitValue(char lowAddr, char highAddr)
{
    // get contents of register
    char lowWord[2], highWord[2];
    readRegister(lowAddr, lowWord);
    readRegister(highAddr, highWord);

    // combine low & high words
    uint16_t val2sComplement =
        (static_cast<uint16_t>(highWord[1] << 8)) | lowWord[1];
    int16_t value = static_cast<int16_t>(val2sComplement);

    return value;
}

float KX134::convertRawToGravs(int16_t lsbValue)
{
    if(gsel1Status && gsel0Status)
    {
        // +-64g
        return (float)lsbValue * 0.00195f;
    }
    else if(gsel1Status && !gsel0Status)
    {
        // +-32g
        return (float)lsbValue * 0.00098f;
    }
    else if(!gsel1Status && gsel0Status)
    {
        // +-16g
        return (float)lsbValue * 0.00049f;
    }
    else if(!gsel1Status && !gsel0Status)
    {
        // +-8g
        return (float)lsbValue * 0.00024f;
    }
    else
    {
        return 0;
    }
}

void KX134::getAccelerations(int16_t *output)
{
    // read X, Y, and Z
    output[0] = read16BitValue(KX134_XOUT_L, KX134_XOUT_H);
    output[1] = read16BitValue(KX134_YOUT_L, KX134_YOUT_H);
    output[2] = read16BitValue(KX134_ZOUT_L, KX134_ZOUT_H);
}

bool KX134::checkExistence()
{
    // verify WHO_I_AM
    char whoami[5];
    readRegister(KX134_WHO_AM_I, whoami);
    printf("0x%X, 0x%X\r\n", whoami[0], whoami[1]);

    if(whoami[1] != 0x46)
    {
        return false; // WHO_AM_I is incorrect
    }

    // verify COTR
    char cotr[2];
    readRegister(KX134_COTR, cotr);
    printf("COTR: 0x%X, 0x%X\r\n", cotr[0], cotr[1]);
    if(cotr[1] != 0x55)
    {
        return false; // COTR is incorrect
    }

    return true;
}

void KX134::setAccelRange(int range)
{
    switch(range)
    {
        case 8:
            gsel1Status = 0;
            gsel0Status = 0;
            break;
        case 16:
            gsel1Status = 0;
            gsel0Status = 1;
            break;
        case 32:
            gsel1Status = 1;
            gsel0Status = 0;
            break;
        case 64:
            gsel1Status = 1;
            gsel0Status = 1;
            break;

        default:
            return;
    }

    uint8_t writeByte = (1 << 7) | (resStatus << 6) | (drdyeStatus << 5) |
                        (gsel1Status << 4) | (gsel0Status << 3) |
                        (tdteStatus << 2) | (tpeStatus);
    // reserved bit 1, PC1 bit must be enabled

    writeRegisterOneByte(KX134_CNTL1, writeByte);

    registerWritingEnabled = 0;
}

void KX134::setOutputDataRate(float hz)
{
    if(!registerWritingEnabled)
    {
        return;
    }

    if(hz == 0.781)
    {
        osa3 = 0;
        osa2 = 0;
        osa1 = 0;
        osa0 = 0;
    }
    else if(hz == 1.563)
    {
        osa3 = 0;
        osa2 = 0;
        osa1 = 0;
        osa0 = 1;
    }
    else if(hz == 3.125)
    {
        osa3 = 0;
        osa2 = 0;
        osa1 = 1;
        osa0 = 0;
    }
    else if(hz == 6.25)
    {
        osa3 = 0;
        osa2 = 0;
        osa1 = 1;
        osa0 = 1;
    }
    else if(hz == 12.5)
    {
        osa3 = 0;
        osa2 = 1;
        osa1 = 0;
        osa0 = 0;
    }
    else if(hz == 25)
    {
        osa3 = 0;
        osa2 = 1;
        osa1 = 0;
        osa0 = 1;
    }
    else if(hz == 50)
    {
        osa3 = 0;
        osa2 = 1;
        osa1 = 1;
        osa0 = 0;
    }
    else if(hz == 100)
    {
        osa3 = 0;
        osa2 = 1;
        osa1 = 1;
        osa0 = 1;
    }
    else if(hz == 200)
    {
        osa3 = 1;
        osa2 = 0;
        osa1 = 0;
        osa0 = 0;
    }
    else if(hz == 400)
    {
        osa3 = 1;
        osa2 = 0;
        osa1 = 0;
        osa0 = 1;
    }
    else if(hz == 800) // available in high-performance mode only
    {
        osa3 = 1;
        osa2 = 0;
        osa1 = 1;
        osa0 = 0;
    }
    else if(hz == 1600) // available in high-performance mode only
    {
        osa3 = 1;
        osa2 = 0;
        osa1 = 1;
        osa0 = 1;
    }
    else if(hz == 3200) // available in high-performance mode only
    {
        osa3 = 1;
        osa2 = 1;
        osa1 = 0;
        osa0 = 0;
    }
    else if(hz == 6400) // available in high-performance mode only
    {
        osa3 = 1;
        osa2 = 1;
        osa1 = 0;
        osa0 = 1;
    }
    else if(hz == 12800) // available in high-performance mode only
    {
        osa3 = 1;
        osa2 = 1;
        osa1 = 1;
        osa0 = 0;
    }
    else if(hz == 25600) // available in high-performance mode only
    {
        osa3 = 1;
        osa2 = 1;
        osa1 = 1;
        osa0 = 1;
    }
    else
    {
        return;
    }

    uint8_t writeByte = (iirBypass << 7) | (lpro << 6) | (fstup << 5) |
                        (osa3 << 3) | (osa2 << 2) | (osa1 << 1) | osa0;
    // reserved bit 4

    writeRegisterOneByte(KX134_ODCNTL, writeByte);
}

void KX134::enableRegisterWriting()
{
    writeRegisterOneByte(KX134_CNTL1, 0x00);
    registerWritingEnabled = 1;
}
