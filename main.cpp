#include "KX134.h"
#include "mbed.h"

#define PIN_SPI_MOSI PB_5
#define PIN_SPI_MISO PB_4
#define PIN_SPI_CS PA_4
#define PIN_SPI_SCK PB_3
#define PIN_INT1 PA_0 // placeholder
#define PIN_INT2 PA_0 // placeholder
#define PIN_RST PA_0  // placeholder

int main(void)
{
    KX134 kx134Obj(PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_SCK, PIN_SPI_CS,
                   PIN_INT1, PIN_INT2, PIN_RST);
    kx134Obj.init();
}
