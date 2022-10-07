//
// Created by Jasper Swallen on 2-15-21
//

#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <limits>

#include "KX134TestSuite.h"
#include "KX134Base.h"
#include "mbed.h"

// void KX134TestSuite::test_existance()
// {
//     if (new_accel.checkExistence())
//     {
//         printf("[SUCCESS]\r\n");
//     }
//     else
//     {
//         printf("[FAILURE]\r\n");
//         printf("Is the device plugged in?\r\n");
//     }
// }

// // This test pairs well with the standard deviation test
// void KX134TestSuite::set_hz()
// {
//     int hz = -1;
//     printf("Enter output data rate (hz): ");
//     scanf("%d", &hz);

//     new_accel.setOutputDataRateHz(hz);
// }

// // This test pairs well with the standard deviation test
// void KX134TestSuite::set_range()
// {
//     int range = -1;
//     printf("Select range:\r\n");
//     printf("1.  +-8G\r\n");
//     printf("2.  +-16G\r\n");
//     printf("3.  +-32G\r\n");
//     printf("4.  +-64G\r\n");
//     scanf("%d", &range);

//     printf("Setting range: ");
//     switch (range)
//     {
//         case 1:
//             printf("+-8G\r\n");
//             new_accel.setAccelRange(KX134Base::Range::RANGE_8G);
//             break;
//         case 2:
//             printf("+-16G\r\n");
//             new_accel.setAccelRange(KX134Base::Range::RANGE_16G);
//             break;
//         case 3:
//             printf("+-32G\r\n");
//             new_accel.setAccelRange(KX134Base::Range::RANGE_32G);
//             break;
//         case 4:
//             printf("+-64G\r\n");
//             new_accel.setAccelRange(KX134Base::Range::RANGE_64G);
//             break;
//         default:
//             printf("Invalid Selection\r\n");
//             break;
//     }
// }

// void KX134TestSuite::test_stddev()
// {
//     const int numTrials = 200;
//     float gravs[numTrials][3];

//     for (size_t trialIndex = 0; trialIndex < numTrials; ++trialIndex)
//     {
//         while (!new_accel.dataReady())
//             ;

//         int16_t output[3];
//         new_accel.getAccelerations(output);
//         float ax = new_accel.convertRawToGravs(output[0]);
//         float ay = new_accel.convertRawToGravs(output[1]);
//         float az = new_accel.convertRawToGravs(output[2]);

//         printf("KX134 Accel: X: %" PRIi16 " LSB, Y: %" PRIi16 " LSB, Z: %" PRIi16 " LSB \r\n",
//             output[0],
//             output[1],
//             output[2]);
//         printf("KX134 Accel in Gravs: X: %f g, Y: %f g, Z: %f g \r\n", ax, ay, az);

//         gravs[trialIndex][0] = ax;
//         gravs[trialIndex][1] = ay;
//         gravs[trialIndex][2] = az;
//     }

//     float average[3] = { 0 };
//     float maxGravs[3] = { 0 };
//     float minGravs[3] = { std::numeric_limits<float>::max() };

//     for (size_t trialIndex = 0; trialIndex < numTrials; ++trialIndex)
//     {
//         for (int i = 0; i < 3; ++i)
//         {
//             average[i] += gravs[trialIndex][i];
//             maxGravs[i] = std::max(gravs[trialIndex][i], maxGravs[i]);
//             minGravs[i] = std::min(gravs[trialIndex][i], minGravs[i]);
//         }
//     }

//     for (int i = 0; i < 3; ++i)
//     {
//         average[i] /= numTrials;
//     }

//     float variance[3] = { 0 };
//     for (size_t trialIndex = 0; trialIndex < numTrials; ++trialIndex)
//     {
//         for (int i = 0; i < 3; ++i)
//         {
//             variance[i] += std::pow(gravs[trialIndex][i] - average[i], 2);
//         }
//     }

//     float stdDeviation[3];
//     for (int i = 0; i < 3; ++i)
//     {
//         variance[i] /= numTrials - 1;
//         stdDeviation[i] = std::sqrt(variance[i]);
//     }

//     printf("Average Gravs: %f x, %f y, %f z\n", average[0], average[1], average[2]);
//     printf("Standard Deviation: %f x, %f y, %f z\n",
//         stdDeviation[0],
//         stdDeviation[1],
//         stdDeviation[2]);
// }

// #if HAMSTER_SIMULATOR != 1
// int main()
// #else
// int kx134_test_main()
// #endif
// {
//     /*while (!new_accel.init())
//     {
//         printf("Failed to initialize KX134\r\n");
//     }*/
//     new_accel.init();
//     printf("Successfully initialized KX134\r\n");

//     new_accel.setAccelRange(KX134Base::Range::RANGE_64G);

//     // test suite harness
//     KX134TestSuite harness;

//     while (1)
//     {
//         int test = -1;
//         printf("\r\n\nHamster Acceleromter Test Suite:\r\n");

//         printf("Select a test: \n\r");
//         printf("0.  Exit Test Suite\r\n");
//         printf("1.  Device alive?\r\n");
//         printf("2.  Set Output Data Rate\r\n");
//         printf("3.  Set Range\r\n");
//         printf("4.  Read Data & Standard Deviation\r\n");

//         scanf("%d", &test);
//         printf("Running test %d:\r\n\n", test);

//         switch (test)
//         {
//             case 0:
//                 return 0;
//             case 1:
//                 harness.test_existance();
//                 break;
//             case 2:
//                 harness.set_hz();
//                 break;
//             case 3:
//                 harness.set_range();
//                 break;
//             case 4:
//                 harness.test_stddev();
//                 break;
//             default:
//                 printf("Invalid test number\r\n");
//                 break;
//         }
//     }
// }

volatile bool event_complete = false;
volatile int event_result = -1;

static int callback_func(int event)
{
    event_complete = true;
    event_result = event;
}

int main()
{
    SPI spi(PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_SCK, PIN_SPI_CS);

    printf("%p\r\n", &spi._peripheral->spi.spi);

    char tx_buf[1] = {0xFF};
    char rx_buf[1];

    event_complete = false;
    int tr = spi.transfer(tx_buf, 1, rx_buf, 1, callback_func);
    printf("Started SPI transfer! %d\r\n", tr);

    while(!event_complete);

    printf("Event completed! %d\r\n", event_result);

    while(1)
    {
        
    }
}
