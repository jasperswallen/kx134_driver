#include "KX134Base.h"

#include <inttypes.h>
#include <math.h>

/** Set to 1 to enable debug printouts */
#define KX134_DEBUG 0

KX134Base::KX134Base()
    : res(1)
    , drdye_enable(1)
    , gsel { 0, 0 }
    , tdte_enable(0)
    , tpe_enable(0)
    , iir_bypass(0)
    , lpro(0)
    , fstup(1)
    , osa { 0, 1, 1, 0 }
{
}

bool KX134Base::reset()
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

bool KX134Base::checkExistence()
{
    // verify WHO_I_AM
    char whoami;
    readRegisterOneByte(Register::WHO_AM_I, whoami);

#if KX134_DEBUG
    printf("Checking existence: WHO_AM_I returned 0x%X", whoami);
#endif
    if (whoami != 0x46)
    {
#if KX134_DEBUG
        printf(" but expected 0x46\r\n");
#endif
        return false; // WHO_AM_I is incorrect
    }

    // verify COTR
    char cotr;
    readRegisterOneByte(Register::COTR, cotr);

#if KX134_DEBUG
    printf(" and COTR returned 0x%X", cotr);
#endif
    if (cotr != 0x55)
    {
#if KX134_DEBUG
        printf(" but expected 0x55\r\n");
#endif
        return false; // COTR is incorrect
    }

#if KX134_DEBUG
    printf("\r\nSuccessfully checked existence\r\n");
#endif

    return true;
}

void KX134Base::getAccelerations(int16_t* output)
{
    char words[6];

    // this was the recommended method by Kionix
    // for some reason, this has *significantly* less noise than reading
    // one-by-one
    readRegister(Register::XOUT_L, words, 6);

    output[0] = convertTo16BitValue(words[0], words[1]) + _offsets[0];
    output[1] = convertTo16BitValue(words[2], words[3]) + _offsets[1];
    output[2] = convertTo16BitValue(words[4], words[5]) + _offsets[2];

#if KX134_DEBUG
    printf("Got accelerations: x=%d, y=%d, z=%d\r\n", output[0], output[1], output[2]);
#endif
}

bool KX134Base::dataReady()
{
    char buf;
    readRegisterOneByte(Register::INS2, buf);

#if KX134_DEBUG
    printf("Checking if data is ready: expected 0x10, received 0x%X\r\n", buf);
#endif

    return (buf & (1 << 4)); // bit4 should be set
}

float KX134Base::convertRawToGravs(int16_t lsbValue) const
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

void KX134Base::setAccelOffsets(int16_t* offsets) { memcpy(_offsets, offsets, sizeof(_offsets)); }

void KX134Base::setAccelRange(Range range)
{
#if KX134_DEBUG
    printf("Setting range to 0x%" PRIx8 "\r\n", static_cast<uint8_t>(range));
#endif

    enableRegisterWriting();

    gsel[0] = static_cast<uint8_t>(range) & 0b01;
    gsel[1] = static_cast<uint8_t>(range) & 0b10;

    uint8_t writeByte = (1 << 7) | (res << 6) | (drdye_enable << 5) | (gsel[1] << 4)
        | (gsel[0] << 3) | (tdte_enable << 2) | (tpe_enable);
    // reserved bit 1, PC1 bit must be enabled

    writeRegisterOneByte(Register::CNTL1, writeByte);
}

void KX134Base::setOutputDataRateHz(uint32_t hz)
{
#if KX134_DEBUG
    printf("Setting ODR to %" PRIu32 " hz\r\n", hz);
#endif

    // calculate byte representation from new polling rate
    // bytes = log2(32*rate/25)

    double new_rate = (double)hz;

    double bytes_double = log2((32.f / 25.f) * new_rate);
    uint8_t bytes_int = (uint8_t)ceil(bytes_double);

    setOutputDataRateBytes(bytes_int);
}

void KX134Base::setOutputDataRateBytes(uint8_t byteHz)
{
#if KX134_DEBUG
    printf("Setting ODR to 0x%x byte-wise\r\n", byteHz);
    printf("That should be %f hz\r\n", pow(2, byteHz) * 25.0 / 32.0);
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

void KX134Base::readRegisterOneByte(Register addr, char &rx_buf)
{
    readRegister(addr, &rx_buf);
}

void KX134Base::writeRegisterOneByte(Register addr, char data, char* buf)
{
    writeRegister(addr, &data, buf);
}


int16_t KX134Base::read16BitValue(Register lowAddr, Register highAddr)
{
    // get contents of register
    char lowWord, highWord;
    readRegisterOneByte(lowAddr, lowWord);
    readRegisterOneByte(highAddr, highWord);

    return convertTo16BitValue(lowWord, highWord);
}

int16_t KX134Base::convertTo16BitValue(uint8_t low, uint8_t high)
{
    // combine low & high words
    uint16_t val2sComplement = (static_cast<uint16_t>(high << 8)) | low;
    int16_t value = static_cast<int16_t>(val2sComplement);

#if KX134_DEBUG
    printf(
        "Converting low (%d) and high (%d) to get 16 bit value (%d)\r\n", low, high, value);
#endif

    return value;
}

void KX134Base::enableRegisterWriting()
{
#if KX134_DEBUG
    printf("Enabling register writing\r\n");
#endif
    uint8_t writeByte = (0 << 7) | (res << 6) | (drdye_enable << 5) | (gsel[1] << 4)
        | (gsel[0] << 3) | (tdte_enable << 2) | (tpe_enable);

    writeRegisterOneByte(Register::CNTL1, writeByte);
}

void KX134Base::disableRegisterWriting()
{
#if KX134_DEBUG
    printf("Disabling register writing\r\n");
#endif

    uint8_t writeByte = (1 << 7) | (res << 6) | (drdye_enable << 5) | (gsel[1] << 4)
        | (gsel[0] << 3) | (tdte_enable << 2) | (tpe_enable);
    // reserved bit 1, PC1 bit must be enabled

    writeRegisterOneByte(Register::CNTL1, writeByte);
}
