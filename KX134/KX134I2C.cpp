#include "KX134I2C.h"
#include "inttypes.h"

#define KX_I2C_FREQ 100000

KX134I2C::KX134I2C(Stream* debug, PinName sda, PinName scl, uint8_t i2c_addr_)
    : KX134Base(debug)
    , i2c_(sda, scl)
    , i2c_addr(i2c_addr_)
{
}

bool KX134I2C::init()
{
    i2c_.frequency(KX_I2C_FREQ);
    return reset();
}

void KX134I2C::writeRegister(Register addr, char* tx_buf, char* rx_buf, int size)
{
    (void)rx_buf;

    uint8_t buf[size + 1];

    buf[0] = static_cast<char>(addr);
    memcpy(buf + 1, tx_buf, size);

#if KX134_DEBUG
    _debug->printf("Write Addr: 0x%" PRIx8 " Reg Addr: 0x%" PRIx8 "\r\n",
        static_cast<uint8_t>(i2c_addr << 1 | 0),
        buf[0]);

    for (int i = 0; i < size; i++)
    {
        _debug->printf("0x%" PRIx8 " ", tx_buf[i]);
    }
#endif

    int ret = i2c_.write(i2c_addr << 1 | 0, reinterpret_cast<char*>(buf), size + 1, false);

    (void)ret;

#if KX134_DEBUG
    if (ret != 0)
    {
        _debug->printf("WriteRegister: write failed!\r\n");
    }
#endif
}

void KX134I2C::readRegister(Register addr, char* rx_buf, int size)
{
    char reg = static_cast<char>(addr);

    int ret;
    (void)ret;

    ret = i2c_.write(i2c_addr << 1 | 0, &reg, 1, true);

#if KX134_DEBUG
    if (ret != 0)
    {
        _debug->printf("ReadRegister: write failed!\r\n");
    }

    _debug->printf("Write Addr: 0x%" PRIx8 " Reg Addr: 0x%" PRIx8 "\r\n", i2c_addr << 1 | 0, reg);
#endif

    ret = i2c_.read(i2c_addr << 1 | 1, rx_buf, size);

#if KX134_DEBUG
    if (ret != 0)
    {
        _debug->printf("ReadRegister: read failed!\r\n");
    }

    _debug->printf("Read Addr: 0x%" PRIx8 "\r\n", i2c_addr << 1 | 1);

    for (int i = 0; i < size; i++)
    {
        _debug->printf("0x%" PRIx8 " ", rx_buf[i]);
    }
    _debug->printf("\r\n");
#endif
}
