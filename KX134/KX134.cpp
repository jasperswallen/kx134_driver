#include "KX134.h"

#include <inttypes.h>
#include <math.h>

#ifdef KX_SPI

#define SPI_FREQ 1000000

/** Set to 1 to enable debug printouts */
#define KX134_DEBUG 1

KX134::KX134(Stream* debug, PinName mosi, PinName miso, PinName sclk, PinName cs)
    : _debug(debug)
    , _spi(mosi, miso, sclk)
    , _cs(cs)
    , res(1)
    , drdye_enable(1)
    , gsel { 0, 0 }
    , tdte_enable(0)
    , tpe_enable(0)
    , iir_bypass(0)
    , lpro(0)
    , fstup(0)
    , osa { 0, 1, 1, 0 }
{
    deselect();
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

bool KX134::checkExistence()
{
    // verify WHO_I_AM
    uint8_t whoami;
    readRegister(Register::WHO_AM_I, &whoami);

#if KX134_DEBUG
    _debug->printf("Checking existence: WHO_AM_I returned 0x%X", whoami);
#endif
    if (whoami != 0x46)
    {
#if KX134_DEBUG
        _debug->printf(" but expected 0x46\r\n");
#endif
        return false; // WHO_AM_I is incorrect
    }

    // verify COTR
    uint8_t cotr;
    readRegister(Register::COTR, &cotr);

#if KX134_DEBUG
    _debug->printf(" and COTR returned 0x%X", cotr);
#endif
    if (cotr != 0x55)
    {
#if KX134_DEBUG
        _debug->printf(" but expected 0x55\r\n");
#endif
        return false; // COTR is incorrect
    }

#if KX134_DEBUG
    _debug->printf("\r\nSuccessfully checked existence\r\n");
#endif

    return true;
}

void KX134::getAccelerations(int16_t* output)
{
    uint8_t words[6];

    // this was the recommended method by Kionix
    // for some reason, this has *significantly* less noise than reading
    // one-by-one
    readRegister(Register::XOUT_L, words, 6);

    output[0] = convertTo16BitValue(words[0], words[1]) + _offsets[0];
    output[1] = convertTo16BitValue(words[2], words[3]) + _offsets[1];
    output[2] = convertTo16BitValue(words[4], words[5]) + _offsets[2];

#if KX134_DEBUG
    _debug->printf("Got accelerations: x=%d, y=%d, z=%d\r\n", output[0], output[1], output[2]);
#endif
}

bool KX134::dataReady()
{
    uint8_t buf;
    readRegister(Register::INS2, &buf);

#if KX134_DEBUG
    _debug->printf("Checking if data is ready: expected 0x10, received 0x%X\r\n", buf);
#endif

    return (buf == 0x10);
}

float KX134::convertRawToGravs(int16_t lsbValue) const
{
    if (gsel[1] && gsel[0])
    {
        // +-64g
        return (float)lsbValue * 0.00195f;
    }
    else if (gsel[1] && !gsel[0])
    {
        // +-32g
        return (float)lsbValue * 0.00098f;
    }
    else if (!gsel[1] && gsel[0])
    {
        // +-16g
        return (float)lsbValue * 0.00049f;
    }
    else if (!gsel[1] && !gsel[0])
    {
        // +-8g
        return (float)lsbValue * 0.00024f;
    }
    else
    {
        return 0;
    }
}

void KX134::setAccelOffsets(int16_t* offsets) { memcpy(_offsets, offsets, sizeof(_offsets)); }

void KX134::setAccelRange(Range range)
{
#if KX134_DEBUG
    _debug->printf("Setting range to 0x%" PRIx8 "\r\n", static_cast<uint8_t>(range));
#endif

    enableRegisterWriting();

    gsel[0] = static_cast<uint8_t>(range) & 0b01;
    gsel[1] = static_cast<uint8_t>(range) & 0b10;

    uint8_t writeByte = (1 << 7) | (res << 6) | (drdye_enable << 5) | (gsel[1] << 4)
        | (gsel[0] << 3) | (tdte_enable << 2) | (tpe_enable);
    // reserved bit 1, PC1 bit must be enabled

    writeRegisterOneByte(Register::CNTL1, writeByte);
}

void KX134::setOutputDataRateHz(uint32_t hz)
{
#if KX134_DEBUG
    _debug->printf("Setting ODR to %" PRIu32 " hz\r\n", hz);
#endif

    // calculate byte representation from new polling rate
    // bytes = log2(32*rate/25)

    double new_rate = (double)hz;

    double bytes_double = log2((32.f / 25.f) * new_rate);
    uint8_t bytes_int = (uint8_t)ceil(bytes_double);

    setOutputDataRateBytes(bytes_int);
}

void KX134::setOutputDataRateBytes(uint8_t byteHz)
{
#if KX134_DEBUG
    _debug->printf("Setting ODR to 0x%x byte-wise\r\n", byteHz);
    _debug->printf("That should be %f hz\r\n", pow(2, byteHz) * 25.0 / 32.0);
#endif

    enableRegisterWriting();

    osa[0] = byteHz & 0b0001;
    osa[1] = byteHz & 0b0010;
    osa[2] = byteHz & 0b0100;
    osa[3] = byteHz & 0b1000;

    uint8_t writeByte = (iir_bypass << 7) | (lpro << 6) | (fstup << 5) | (osa[3] << 3)
        | (osa[2] << 2) | (osa[1] << 1) | osa[0];
    // reserved bit 4

    writeRegisterOneByte(Register::ODCNTL, writeByte);

    disableRegisterWriting();
}

void KX134::readRegister(Register addr, uint8_t* rx_buf, int size)
{
    select();

    /* Select the register to read */
#if KX134_DEBUG
    _debug->printf("Selected register 0x%" PRIX8 " and received 0x%X\r\n",
        static_cast<uint8_t>(addr),
#endif
        _spi.write(static_cast<uint8_t>(addr) | 0x80)
#if KX134_DEBUG
    )
#endif
        ;

    for (int i = 0; i < size; ++i)
    {
        rx_buf[i] = _spi.write(0x00);
#if KX134_DEBUG
        _debug->printf(
            "Read 0x%X from register 0x%" PRIX8 "\r\n", rx_buf[i], static_cast<uint8_t>(addr));
#endif
    }

    deselect();
}

void KX134::writeRegister(Register addr, uint8_t* data, uint8_t* rx_buf, int size)
{
    select();

    _spi.write(static_cast<uint8_t>(addr)); // select register
    for (int i = 0; i < size; ++i)
    {
        rx_buf[i] = _spi.write(data[i]);
#if KX134_DEBUG
        _debug->printf("Wrote 0x%X to register 0x%" PRIX8 " and received 0x%X\r\n",
            data[i],
            static_cast<uint8_t>(addr),
            rx_buf[i]);
#endif
    }

    deselect();
}

void KX134::writeRegisterOneByte(Register addr, uint8_t data, uint8_t* buf)
{
    static uint8_t defaultBuffer[2] = { 0 };
    if (buf == nullptr)
    {
        buf = defaultBuffer;
    }

    uint8_t _data[1] = { data };
    writeRegister(addr, _data, buf);
}

int16_t KX134::read16BitValue(Register lowAddr, Register highAddr)
{
    // get contents of register
    uint8_t lowWord, highWord;
    readRegister(lowAddr, &lowWord);
    readRegister(highAddr, &highWord);

    return convertTo16BitValue(lowWord, highWord);
}

int16_t KX134::convertTo16BitValue(uint8_t low, uint8_t high)
{
    // combine low & high words
    uint16_t val2sComplement = (static_cast<uint16_t>(high << 8)) | low;
    int16_t value = static_cast<int16_t>(val2sComplement);

#if KX134_DEBUG
    _debug->printf(
        "Converting low (%d) and high (%d) to get 16 bit value (%d)\r\n", low, high, value);
#endif

    return value;
}

void KX134::enableRegisterWriting()
{
#if KX134_DEBUG
    _debug->printf("Enabling register writing\r\n");
#endif
    uint8_t writeByte = (0 << 7) | (res << 6) | (drdye_enable << 5) | (gsel[1] << 4)
        | (gsel[0] << 3) | (tdte_enable << 2) | (tpe_enable);

    writeRegisterOneByte(Register::CNTL1, writeByte);
}

void KX134::disableRegisterWriting()
{
#if KX134_DEBUG
    _debug->printf("Disabling register writing\r\n");
#endif

    uint8_t writeByte = (1 << 7) | (res << 6) | (drdye_enable << 5) | (gsel[1] << 4)
        | (gsel[0] << 3) | (tdte_enable << 2) | (tpe_enable);
    // reserved bit 1, PC1 bit must be enabled

    writeRegisterOneByte(Register::CNTL1, writeByte);
}

void KX134::deselect()
{
    _cs.write(1);

    /* Delay one max-speed clock cycle (100ns) to ensure chip is ready by next write */
    wait_us(1);
}

void KX134::select()
{
    _cs.write(0);
}

#else

#define KX_I2C_FREQ 100000

KX134::KX134(PinName sda, PinName scl) : i2c_(sda, scl)
{
}

bool KX134::init()
{
    i2c_.frequency(KX_I2C_FREQ);
    return reset();
}

bool KX134::reset()
{
    int ack = writeRegisterOneByte(Register::INTERNAL_0X7F, 0x00);
    if (!ack) return false;

    ack = writeRegisterOneByte(Register::CNTL2, 0x00);
    if (!ack) return false;

    ack = writeRegisterOneByte(Register::CNTL2, 0x80);
    if (!ack) return false;

    ThisThread::sleep_for(2ms);

    return checkExistance();
}

bool KX134::checkExistance()
{
    uint8_t who_ami_i;
    readRegisterOneByte(Register::WHO_AM_I, who_ami_i);

    if(who_ami_i != 0x46)
    {
        return false;
    }

    uint8_t cotr;
    readRegisterOneByte(Register::COTR, cotr);

    if(cotr != 0x55)
    {
        return false;
    }

    return true;
}

int KX134::writeRegisterOneByte(Register addr, uint8_t tx_data)
{
    return writeRegister(addr, &tx_data);
}

int KX134::writeRegister(Register addr, uint8_t *tx_data, size_t size)
{
    select(); // S

    int ack = i2c_.write(i2c_addr << 1 | 0); // write mode

    if (!ack) return ack;

    ack = i2c_.write(static_cast<uint8_t>(addr));

    if (!ack) return ack;

    for (size_t i = 0; i < size; ++i)
    {
        ack = i2c_.write(tx_data[i]);

        if (!ack) return ack;
    }

    deselect(); // P

    return ack;
}

int KX134::readRegisterOneByte(Register addr, uint8_t &rx_buf)
{
    return readRegister(addr, &rx_buf);
}

int KX134::readRegister(Register addr, uint8_t *rx_buf, size_t size)
{
    select(); // S

    int ack = i2c_.write(i2c_addr << 1 | 0); // write mode
    if (!ack) return ack;

    ack = i2c_.write(static_cast<uint8_t>(addr));
    if (!ack) return ack;

    select(); // Sr

    ack = i2c_.write(i2c_addr << 1 | 1); // read mode

    for (size_t i = 0; i < size; ++i)
    {
        if (i == size - 1)
        {
            rx_buf[i] = i2c_.read(0); // NACK
        }
        else
        {
            rx_buf[i] = i2c_.read(1); // ACK
        }
    }

    deselect(); // P

    return ack;
}

void KX134::select()
{
    i2c_.lock();
}

void KX134::deselect()
{
    i2c_.unlock();
}

#endif
