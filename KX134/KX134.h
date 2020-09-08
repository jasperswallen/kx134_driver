/**
 * @author Jasper Swallen
 * @filename KX134.h
 *
 * @section DESCRIPTION
 *
 * Software Driver for KX134-1211 accelerometer
 *
 * Datasheets:
 * http://kionixfs.kionix.com/en/document/AN101-Getting-Started.pdf
 * https://d10bqar0tuhard.cloudfront.net/en/document/KX134-1211-Technical-Reference-Manual-Rev-1.0.pdf
 * https://d10bqar0tuhard.cloudfront.net/en/document/TN027-Power-On-Procedure.pdf
 * https://d10bqar0tuhard.cloudfront.net/en/datasheet/KX134-1211-Specifications-Rev-1.0.pdf
 */

#ifndef KX134_H
#define KX134_H

#include "mbed.h"

class KX134
{
  public:
    KX134(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName int1,
          PinName int2, PinName rst);

    bool init();

    /* Converts a LSB value to gravs
     * To convert to m/s^2, multiply by 1G (~9.8m/s^2)
     *
     * Note:
     * +-64g: 1LSB = 0.00195g
     * +-32g: 1LSB = 0.00098g
     * +-16g: 1LSB = 0.00049g
     * +-8g: 1LSB = 0.00024g
     */
    float convertRawToGravs(int16_t lsbValue);

    /* Changes the value of output[3] as follows:
     * output[0] is X acceleration
     * output[1] is Y accel
     * output[2] is Z accel
     *
     * Results are in LSB format, to convert call convertRawToGravs() on each
     * output
     */
    void getAccelerations(int16_t *output);

    /* Verifies the KX134-1211 unit is connected and functioning normally.
     * If it returns false, call reset() and check again.
     */
    bool checkExistence();

    /* To enable writing to settings registers, this function must be called.
     * After writing settings, register writing is automatically disabled, and
     * this function must be called again to enable it.
     */
    void enableRegisterWriting();

    // Set acceleration range (8, 16, 32, or 64 gs)
    void setAccelRange(int range);

    // Set Output Data Rate
    void setOutputDataRate(float hz);

  private:
    // Mbed pin identities
    SPI _spi;
    PinName _int1, _int2;
    DigitalOut _cs;
    DigitalOut _rst;

    /* Reset function
     * Should be called on initial start (init()) and every software reset
     */
    bool reset();

    /* Deselect (push high) _cs
     */
    void deselect();

    /* Select (push low) _cs
     */
    void select();

    /* Read a given register a given number of bytes
     *
     * Note: the first byte read should return 0x0, so the data begins at
     * rx_buf[1]
     */
    void readRegister(char addr, char *rx_buf, int size = 2);

    /* Writes a given register a given number of bytes
     *
     * Note: the first byte read should return 0x0, so the data begins at
     * rx_buf[1]
     */
    void writeRegister(uint8_t addr, uint8_t *data, char *rx_buf, int size = 1);

    /* Writes a given register 1 byte (convenience function, calls
     * writeRegister())
     *
     * Note: the first byte read should return 0x0, so the data begins at
     * rx_buf[1]
     */
    void writeRegisterOneByte(uint8_t addr, uint8_t data, char *buf);

    /* Reads a value from a low and high address and combines them to create a
     * signed (2s complement) 16-bit integer
     */
    int16_t read16BitValue(char lowAddr, char highAddr);

    // Settings variables

    // CNTL1 vars
    bool resStatus;
    bool drdyeStatus;
    bool gsel1Status;
    bool gsel0Status;
    bool tdteStatus;
    bool tpeStatus;

    // ODCNTL vars
    bool iirBypass;
    bool lpro;
    bool fstup;
    bool osa3;
    bool osa2;
    bool osa1;
    bool osa0;

    bool registerWritingEnabled;
};

#endif // KX134_H