#include "KX134.h"
#include "mbed.h"

#include <inttypes.h>

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

    if(!kx134Obj.init())
    {
        printf("Failed to initialize KX134\r\n");
        return 1;
    }

    printf("Successfully initialized KX134\r\n");

    // initialize asynch reading
    kx134Obj.enableRegisterWriting();
    kx134Obj.setOutputDataRate(50);
    kx134Obj.setAccelRange(32);

    while(1)
    {
        int16_t output[3];
        kx134Obj.getAccelerations(output);
        float ax = kx134Obj.convertRawToGravs(output[0]);
        float ay = kx134Obj.convertRawToGravs(output[1]);
        float az = kx134Obj.convertRawToGravs(output[2]);

        printf("KX134 Accel: X: %" PRIi16 " LSB, Y: %" PRIi16
               " LSB, Z: %" PRIi16 " LSB \r\n",
               output[0], output[1], output[2]);
        printf("KX134 Accel in Gravs: X: %f g, Y: %f g, Z: %f g \r\n", ax, ay,
               az);
        ThisThread::sleep_for(1000ms);
    }
}
