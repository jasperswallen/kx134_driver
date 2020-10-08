#include "KX134.h"
#include "math.h"

#define SPI_FREQ 100000

static uint8_t defaultBuffer[2] = {0}; // allows calling writeRegisterOneByte
                                       // without buf argument

/* Writes one byte to a register
 */
void KX134::writeRegisterOneByte(Register addr, uint8_t data,
                                 uint8_t *buf = defaultBuffer)
{
    uint8_t _data[1] = {data};
    writeRegister(addr, _data, buf);
}

KX134::KX134(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName int1,
             PinName int2, PinName rst)
    : _spi(mosi, miso, sclk), _int1(int1), _int2(int2), _cs(cs), _rst(rst)
{
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

KX134::~KX134()
{
    printf("%d %d %d %d\r\n", osa3, osa2, osa1, osa0);
}

bool KX134::init()
{
    deselect();

    _spi.frequency(SPI_FREQ);
    _spi.format(8, 1);

    return reset();
}

bool KX134::reset()
{
    // write registers to start reset
    writeRegisterOneByte(Register::INTERNAL_0X7F, 0x00);
    writeRegisterOneByte(Register::CNTL2, 0x00);
    writeRegisterOneByte(Register::CNTL2, 0x80);

    // software reset time
    wait_us(2000);

    // check existence
    return checkExistence();
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

void KX134::readRegister(Register addr, uint8_t *rx_buf, int size)
{
    select();

    rx_buf[0] = _spi.write((uint8_t)addr | (1 << 7));

    for(int i = 1; i < size; ++i)
    {
        rx_buf[i] = _spi.write(0x00);
    }

    deselect();
}

void KX134::writeRegister(Register addr, uint8_t *data, uint8_t *rx_buf,
                          int size)
{
    select();

    _spi.write((uint8_t)addr); // select register
    for(int i = 0; i < size; ++i)
    {
        rx_buf[i] = _spi.write(data[i]);
    }

    deselect();
}

int16_t KX134::read16BitValue(Register lowAddr, Register highAddr)
{
    // get contents of register
    uint8_t lowWord[2], highWord[2];
    readRegister(lowAddr, lowWord);
    readRegister(highAddr, highWord);

    return convertTo16BitValue(lowWord[1], highWord[1]);
}

int16_t KX134::convertTo16BitValue(uint8_t low, uint8_t high)
{
    // combine low & high words
    uint16_t val2sComplement = (static_cast<uint16_t>(high << 8)) | low;
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
    uint8_t words[7];

    // this was the recommended method by Kionix
    // for some reason, this has *significantly* less noise than reading
    // one-by-one
    readRegister(Register::XOUT_L, words, 7);

    output[0] = convertTo16BitValue(words[1], words[2]);
    output[1] = convertTo16BitValue(words[3], words[4]);
    output[2] = convertTo16BitValue(words[5], words[6]);
}

bool KX134::checkExistence()
{
    // verify WHO_I_AM
    uint8_t whoami[2];
    readRegister(Register::WHO_AM_I, whoami);

    if(whoami[1] != 0x46)
    {
        return false; // WHO_AM_I is incorrect
    }

    // verify COTR
    uint8_t cotr[2];
    readRegister(Register::COTR, cotr);
    if(cotr[1] != 0x55)
    {
        return false; // COTR is incorrect
    }

    return true;
}

void KX134::setAccelRange(Range range)
{
    gsel0Status = (uint8_t)range & 0b01;
    gsel1Status = (uint8_t)range & 0b10;

    uint8_t writeByte = (1 << 7) | (resStatus << 6) | (drdyeStatus << 5) |
                        (gsel1Status << 4) | (gsel0Status << 3) |
                        (tdteStatus << 2) | (tpeStatus);
    // reserved bit 1, PC1 bit must be enabled

    writeRegisterOneByte(Register::CNTL1, writeByte);

    registerWritingEnabled = 0;
}

void KX134::setOutputDataRateBytes(uint8_t byteHz)
{
    if(!registerWritingEnabled)
    {
        return;
    }

    osa0 = byteHz & 0b0001;
    osa1 = byteHz & 0b0010;
    osa2 = byteHz & 0b0100;
    osa3 = byteHz & 0b1000;

    uint8_t writeByte = (iirBypass << 7) | (lpro << 6) | (fstup << 5) |
                        (osa3 << 3) | (osa2 << 2) | (osa1 << 1) | osa0;
    // reserved bit 4

    writeRegisterOneByte(Register::ODCNTL, writeByte);
}

void KX134::setOutputDataRateHz(uint32_t hz)
{
    // calculate byte representation from new polling rate
    // bytes = log2(32*rate/25)

    double new_rate = (double)hz;

    double bytes_double = log2((32.f / 25.f) * new_rate);
    uint8_t bytes_int = (uint8_t)ceil(bytes_double);

    setOutputDataRateBytes(bytes_int);
}

void KX134::enableRegisterWriting()
{
    writeRegisterOneByte(Register::CNTL1, 0x00);
    registerWritingEnabled = 1;
}

void KX134::disableRegisterWriting()
{
    if(!registerWritingEnabled)
    {
        return;
    }

    uint8_t writeByte = (0 << 7) | (resStatus << 6) | (drdyeStatus << 5) |
                        (gsel1Status << 4) | (gsel0Status << 3) |
                        (tdteStatus << 2) | (tpeStatus);
    // reserved bit 1, PC1 bit must be enabled

    writeRegisterOneByte(Register::CNTL1, writeByte);

    registerWritingEnabled = 0;
}
