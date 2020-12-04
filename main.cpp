// #include "KX134.h"
// #include "math.h"
// #include "mbed.h"

// #include <inttypes.h>

// #define PIN_SPI_MOSI PB_5
// #define PIN_SPI_MISO PB_4
// #define PIN_SPI_CS PA_4
// #define PIN_SPI_SCK PB_3
// #define PIN_INT1 PA_0 // placeholder
// #define PIN_INT2 PA_0 // placeholder
// #define PIN_RST PA_0  // placeholder

// // int main(void)
// // {
// //     KX134 kx134Obj(PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_SCK, PIN_SPI_CS,
// //                    PIN_INT1, PIN_INT2, PIN_RST);

// //     if(!kx134Obj.init())
// //     {
// //         printf("Failed to initialize KX134\r\n");
// //         return 1;
// //     }

// //     printf("Successfully initialized KX134\r\n");

// //     int hz = 25600;

// //     // initialize asynch reading
// //     kx134Obj.enableRegisterWriting();
// //     kx134Obj.setOutputDataRateHz(hz);
// //     kx134Obj.setAccelRange(KX134::Range::RANGE_64G);
// //     kx134Obj.disableRegisterWriting();

// //     int numTrials = 200;
// //     float gravs[numTrials][3];

// //     for(size_t trialIndex = 0; trialIndex < numTrials; ++trialIndex)
// //     {
// //         while(!kx134Obj.dataReady())
// //             ;

// //         int16_t output[3];
// //         kx134Obj.getAccelerations(output);
// //         float ax = kx134Obj.convertRawToGravs(output[0]);
// //         float ay = kx134Obj.convertRawToGravs(output[1]);
// //         float az = kx134Obj.convertRawToGravs(output[2]);

// //         printf("KX134 Accel: X: %" PRIi16 " LSB, Y: %" PRIi16
// //                " LSB, Z: %" PRIi16 " LSB \r\n",
// //                output[0], output[1], output[2]);
// //         printf("KX134 Accel in Gravs: X: %f g, Y: %f g, Z: %f g \r\n", ax,
// //         ay,
// //                az);

// //         gravs[trialIndex][0] = ax;
// //         gravs[trialIndex][1] = ay;
// //         gravs[trialIndex][2] = az;
// //     }

// //     float average[3] = {0};
// //     float maxGravs[3] = {0};
// //     float minGravs[3] = {std::numeric_limits<float>::max()};

// //     for(size_t trialIndex = 0; trialIndex < numTrials; ++trialIndex)
// //     {
// //         for(int i = 0; i < 3; ++i)
// //         {
// //             average[i] += gravs[trialIndex][i];
// //             maxGravs[i] = std::max(gravs[trialIndex][i], maxGravs[i]);
// //             minGravs[i] = std::min(gravs[trialIndex][i], minGravs[i]);
// //         }
// //     }

// //     for(int i = 0; i < 3; ++i)
// //     {
// //         average[i] /= numTrials;
// //     }

// //     float variance[3] = {0};
// //     for(size_t trialIndex = 0; trialIndex < numTrials; ++trialIndex)
// //     {
// //         for(int i = 0; i < 3; ++i)
// //         {
// //             variance[i] += pow(gravs[trialIndex][i] - average[i], 2);
// //         }
// //     }

// //     float stdDeviation[3];
// //     for(int i = 0; i < 3; ++i)
// //     {
// //         variance[i] /= numTrials - 1;
// //         stdDeviation[i] = sqrt(variance[i]);
// //     }

// //     printf("Average Gravs at %d Hz: %f x, %f y, %f z\n", hz, average[0],
// //            average[1], average[2]);
// //     printf("Standard Deviation at %d Hz: %f x, %f y, %f z\n", hz,
// //            stdDeviation[0], stdDeviation[1], stdDeviation[2]);
// // }

#include "SerialStream.h"
#include "mbed.h"

#define BAUDRATE 115200
#define CMD_BUFFER_SIZE 32

UnbufferedSerial serial(USBTX, USBRX, BAUDRATE);
SerialStream<UnbufferedSerial> pc(serial);

char cmdStr[CMD_BUFFER_SIZE];
volatile bool pendingCmd = false;
volatile bool charAvailable = false;

void rxCallback(char c)
{
    static char prev_char = '\0';
    static int currPos = 0;

    bool bufferFull = (currPos == (CMD_BUFFER_SIZE - 1));

    // if we got a newline (handling \n, \r, and \r\n equally)
    if(c == '\n' || c == '\r' || bufferFull)
    {
        // Case 1 (\n recieved)   : n > 1, command is copied to cmdStr
        // Case 2 (\r recieved)   : n > 1, command is copied to cmdStr
        // Case 3 (\r\n recieved) : \r is received first (Case 2),
        //                          then \n is recieved but n == 1, so it is
        //                          ignored.
        if(prev_char == '\r')
        {
            prev_char = c;
            pc.sync();
            return; // skip this \r or \n, since it's prob part of a CRLF (or an
                    // \r\r. Which would be weird.)
        }

        // insert string null terminator
        cmdStr[currPos] = '\0';
        pendingCmd = true;
        if(bufferFull)
        {
            prev_char = c;
            pc.sync();
        }
    }
    else
    {
        pc.putc(c);
        cmdStr[currPos] = c;
    }
    currPos++;
    prev_char = c;
}

// A function that echoes any received data back
void updateCommand(char *cmd)
{
    pc.printf("\r\nReceived CMD ");
    pc.printf(cmd);
    pc.printf("\r\n");

    pendingCmd = false;
    memset(cmdStr, 0, strlen(cmdStr));
}

void charToRead()
{
    charAvailable = true;
}

int main(void)
{
    pc.printf("Starting\r\n");
    pc.attach(&charToRead, UnbufferedSerial::RxIrq);

    while(1)
    {
        if(charAvailable)
        {
            pc.printf("charAvailable\r\n");
            char c;
            pc.read(&c, 1);
            rxCallback(c);
        }

        if(pendingCmd)
        {
            updateCommand(cmdStr);
        }
        ThisThread::sleep_for(1s);
    }
}