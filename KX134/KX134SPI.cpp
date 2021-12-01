#include "KX134SPI.h"
#include <cinttypes>

#define SPI_FREQ 1000000

volatile bool KX134SPI::event_complete;

KX134SPI::KX134SPI(PinName mosi, PinName miso, PinName sclk, PinName cs)
    : KX134Base()
    , _spi(mosi, miso, sclk)
    , _cs(cs)
{
    deselect();
}

bool KX134SPI::init()
{
    deselect();

    _spi.frequency(SPI_FREQ);
    _spi.format(8, 0);

    return reset();
}

void KX134SPI::readRegister(Register addr, char* rx_buf, int size)
{
    select();

    /* Select the register to read */
        Timer t;
        t.start();
        uint8_t buf[1] = {static_cast<uint8_t>(addr) | 0x80};
        uint8_t fakerxbuf[1];
        event_complete = 0;
        int transferresult = _spi.transfer(buf, 1, fakerxbuf, 1, transaction_complete);
        printf("The value of transfer is %d\n", transferresult);
        while(!event_complete){}
        t.stop();
        printf("the time taken was %d seconds\n", t.read());

    for (int i = 0; i < size; ++i)
    {
        rx_buf[i] = _spi.write(0x00);
#if KX134_DEBUG
        printf(
            "Read 0x%X from register 0x%" PRIX8 "\r\n", rx_buf[i], static_cast<uint8_t>(addr));
#endif
    }

    deselect();
}

void KX134SPI::writeRegister(Register addr, char* tx_buf, char* rx_buf, int size)
{
    select();

    _spi.write(static_cast<uint8_t>(addr)); // select register
    for (int i = 0; i < size; ++i)
    {
        if (rx_buf != nullptr)
        {
            rx_buf[i] = _spi.write(tx_buf[i]);
#if KX134_DEBUG
            printf("Wrote 0x%X to register 0x%" PRIX8 " and received 0x%X\r\n",
                tx_buf[i],
                static_cast<uint8_t>(addr),
                rx_buf[i]);
#endif
        }
        else
        {
#if KX134_DEBUG
            printf("Wrote 0x%X to register 0x%" PRIX8 " and received 0x%X\r\n",
                tx_buf[i],
                static_cast<uint8_t>(addr),
#endif
                _spi.write(tx_buf[i])
#if KX134_DEBUG
            )
#endif
                ;
        }
    }

    deselect();
}

void KX134SPI::deselect()
{
    _cs.write(1);

    /* Delay one max-speed clock cycle (100ns) to ensure chip is ready by next write */
    wait_us(1);
}

void KX134SPI::select() { _cs.write(0); }

void KX134SPI::transaction_complete(int event){
    printf("%s\n", "Am I in the callback?");
    event_complete = true;
}
