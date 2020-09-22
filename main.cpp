#include "KX134.h"
#include "math.h"
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

    int hz = 6400;

    // initialize asynch reading
    kx134Obj.enableRegisterWriting();
    kx134Obj.setOutputDataRateHz(hz);
    kx134Obj.setAccelRange(KX134::Range::RANGE_64G);
    kx134Obj.disableRegisterWriting();

    int numTrials = 200;
    float gravs[numTrials][3];

    for(size_t trialIndex = 0; trialIndex < numTrials; ++trialIndex)
    {
        float time = 1.0 / (float)hz * 1000.0;
        ThisThread::sleep_for(time);

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

        gravs[trialIndex][0] = ax;
        gravs[trialIndex][1] = ay;
        gravs[trialIndex][2] = az;
    }

    float average[3] = {0};
    float maxGravs[3] = {0};
    float minGravs[3] = {std::numeric_limits<float>::max()};

    for(size_t trialIndex = 0; trialIndex < numTrials; ++trialIndex)
    {
        for(int i = 0; i < 3; ++i)
        {
            average[i] += gravs[trialIndex][i];
            maxGravs[i] = std::max(gravs[trialIndex][i], maxGravs[i]);
            minGravs[i] = std::min(gravs[trialIndex][i], minGravs[i]);
        }
    }

    for(int i = 0; i < 3; ++i)
    {
        average[i] /= numTrials;
    }

    float variance[3] = {0};
    for(size_t trialIndex = 0; trialIndex < numTrials; ++trialIndex)
    {
        for(int i = 0; i < 3; ++i)
        {
            variance[i] += pow(gravs[trialIndex][i] - average[i], 2);
        }
    }

    float stdDeviation[3];
    for(int i = 0; i < 3; ++i)
    {
        variance[i] /= numTrials - 1;
        stdDeviation[i] = sqrt(variance[i]);
    }

    printf("Average Gravs at %d Hz: %f x, %f y, %f z\n", hz, average[0],
           average[1], average[2]);
    printf("Standard Deviation at %d Hz: %f x, %f y, %f z\n", hz,
           stdDeviation[0], stdDeviation[1], stdDeviation[2]);
}
