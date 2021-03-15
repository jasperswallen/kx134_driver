//
// Created by Jasper Swallen on 2-15-21
//

#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <limits>

#include "KX134TestSuite.h"
#include "mbed.h"
#include "SerialStream.h"

#define BAUDRATE 115200

BufferedSerial serial(USBTX, USBRX, BAUDRATE);
SerialStream<BufferedSerial> pc(serial);

#ifdef KX134_SPI

KX134 new_accel(&pc, PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_SCK, PIN_SPI_CS);

void KX134TestSuite::test_existance()
{
    if (new_accel.checkExistence())
    {
        pc.printf("[SUCCESS]\r\n");
    }
    else
    {
        pc.printf("[FAILURE]\r\n");
        pc.printf("Is the device plugged in?\r\n");
    }
}

// This test pairs well with the standard deviation test
void KX134TestSuite::set_hz()
{
    int hz = -1;
    pc.printf("Enter output data rate (hz): ");
    pc.scanf("%d", &hz);

    new_accel.setOutputDataRateHz(hz);
}

// This test pairs well with the standard deviation test
void KX134TestSuite::set_range()
{
    int range = -1;
    pc.printf("Select range:\r\n");
    pc.printf("1.  +-8G\r\n");
    pc.printf("2.  +-16G\r\n");
    pc.printf("3.  +-32G\r\n");
    pc.printf("4.  +-64G\r\n");
    pc.scanf("%d", &range);

    pc.printf("Setting range: ");
    switch (range)
    {
        case 1:
            pc.printf("+-8G\r\n");
            new_accel.setAccelRange(KX134::Range::RANGE_8G);
            break;
        case 2:
            pc.printf("+-16G\r\n");
            new_accel.setAccelRange(KX134::Range::RANGE_16G);
            break;
        case 3:
            pc.printf("+-32G\r\n");
            new_accel.setAccelRange(KX134::Range::RANGE_32G);
            break;
        case 4:
            pc.printf("+-64G\r\n");
            new_accel.setAccelRange(KX134::Range::RANGE_64G);
            break;
        default:
            pc.printf("Invalid Selection\r\n");
            break;
    }
}

void KX134TestSuite::test_stddev()
{
    const int numTrials = 200;
    float gravs[numTrials][3];

    for (size_t trialIndex = 0; trialIndex < numTrials; ++trialIndex)
    {
        while (!new_accel.dataReady())
            ;

        int16_t output[3];
        new_accel.getAccelerations(output);
        float ax = new_accel.convertRawToGravs(output[0]);
        float ay = new_accel.convertRawToGravs(output[1]);
        float az = new_accel.convertRawToGravs(output[2]);

        printf("KX134 Accel: X: %" PRIi16 " LSB, Y: %" PRIi16 " LSB, Z: %" PRIi16 " LSB \r\n",
            output[0],
            output[1],
            output[2]);
        printf("KX134 Accel in Gravs: X: %f g, Y: %f g, Z: %f g \r\n", ax, ay, az);

        gravs[trialIndex][0] = ax;
        gravs[trialIndex][1] = ay;
        gravs[trialIndex][2] = az;
    }

    float average[3] = { 0 };
    float maxGravs[3] = { 0 };
    float minGravs[3] = { std::numeric_limits<float>::max() };

    for (size_t trialIndex = 0; trialIndex < numTrials; ++trialIndex)
    {
        for (int i = 0; i < 3; ++i)
        {
            average[i] += gravs[trialIndex][i];
            maxGravs[i] = std::max(gravs[trialIndex][i], maxGravs[i]);
            minGravs[i] = std::min(gravs[trialIndex][i], minGravs[i]);
        }
    }

    for (int i = 0; i < 3; ++i)
    {
        average[i] /= numTrials;
    }

    float variance[3] = { 0 };
    for (size_t trialIndex = 0; trialIndex < numTrials; ++trialIndex)
    {
        for (int i = 0; i < 3; ++i)
        {
            variance[i] += std::pow(gravs[trialIndex][i] - average[i], 2);
        }
    }

    float stdDeviation[3];
    for (int i = 0; i < 3; ++i)
    {
        variance[i] /= numTrials - 1;
        stdDeviation[i] = std::sqrt(variance[i]);
    }

    printf("Average Gravs: %f x, %f y, %f z\n", average[0], average[1], average[2]);
    printf("Standard Deviation: %f x, %f y, %f z\n",
        stdDeviation[0],
        stdDeviation[1],
        stdDeviation[2]);
}

#if HAMSTER_SIMULATOR != 1
int main()
#else
int kx134_test_main()
#endif
{
    if (!new_accel.init())
    {
        printf("Failed to initialize KX134\r\n");
        return 1;
    }
    printf("Successfully initialized KX134\r\n");

    new_accel.setAccelRange(KX134::Range::RANGE_64G);

    // test suite harness
    KX134TestSuite harness;

    while (1)
    {
        int test = -1;
        pc.printf("\r\n\nHamster Acceleromter Test Suite:\r\n");

        pc.printf("Select a test: \n\r");
        pc.printf("0.  Exit Test Suite\r\n");
        pc.printf("1.  Device alive?\r\n");
        pc.printf("2.  Set Output Data Rate\r\n");
        pc.printf("3.  Set Range\r\n");
        pc.printf("4.  Read Data & Standard Deviation\r\n");

        pc.scanf("%d", &test);
        pc.printf("Running test %d:\r\n\n", test);

        switch (test)
        {
            case 0:
                return 0;
            case 1:
                harness.test_existance();
                break;
            case 2:
                harness.set_hz();
                break;
            case 3:
                harness.set_range();
                break;
            case 4:
                harness.test_stddev();
                break;
            default:
                pc.printf("Invalid test number\r\n");
                break;
        }
    }
}

#else
KX134 new_accel(NC, NC);

int main()
{
    pc.printf("Initialized %u", new_accel.init());
}

#endif
