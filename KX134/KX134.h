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
    enum class Register : uint8_t
    {
        MAN_ID = 0x00,
        PART_ID = 0x01,
        XADP_L = 0x02,
        XADP_H = 0x03,
        YADP_L = 0x04,
        YADP_H = 0x05,
        ZADP_L = 0x06,
        ZADP_H = 0x07,
        XOUT_L = 0x08,
        XOUT_H = 0x09,
        YOUT_L = 0x0A,
        YOUT_H = 0x0B,
        ZOUT_L = 0x0C,
        ZOUT_H = 0x0D,
        COTR = 0x12,
        WHO_AM_I = 0x13,
        TSCP = 0x14,
        TSPP = 0x15,
        INS1 = 0x16,
        INS2 = 0x17,
        INS3 = 0x18,
        STATUS_REG = 0x19,
        INT_REL = 0x1A,
        CNTL1 = 0x1B,
        CNTL2 = 0x1C,
        CNTL3 = 0x1D,
        CNTL4 = 0x1E,
        CNTL5 = 0x1F,
        CNTL6 = 0x20,
        ODCNTL = 0x21,
        INC1 = 0x22,
        INC2 = 0x23,
        INC3 = 0x24,
        INC4 = 0x25,
        INC5 = 0x26,
        INC6 = 0x27,
        TILT_TIMER = 0x29,
        TDTRC = 0x2A,
        TDTC = 0x2B,
        TTH = 0x2C,
        TTL = 0x2D,
        FTD = 0x2E,
        STD = 0x2F,
        TLT = 0x30,
        TWS = 0x31,
        FFTH = 0x32,
        FFC = 0x33,
        FFCNTL = 0x34,
        TILT_ANGLE_LL = 0x37,
        TILT_ANGLE_HL = 0x38,
        HYST_SET = 0x39,
        LP_CNTL1 = 0x3A,
        LP_CNTL2 = 0x3B,
        WUFTH = 0x49,
        BTSWUFTH = 0x4A,
        BTSTH = 0x4B,
        BTSC = 0x4C,
        WUFC = 0x4D,
        SELF_TEST = 0x5D,
        BUF_CNTL1 = 0x5E,
        BUF_CNTL2 = 0x5F,
        BUF_STATUS_1 = 0x60,
        BUF_STATUS_2 = 0x61,
        BUF_CLEAR = 0x62,
        BUF_READ = 0x63,
        ADP_CNTL1 = 0x64,
        ADP_CNTL2 = 0x65,
        ADP_CNTL3 = 0x66,
        ADP_CNTL4 = 0x67,
        ADP_CNTL5 = 0x68,
        ADP_CNTL6 = 0x69,
        ADP_CNTL7 = 0x6A,
        ADP_CNTL8 = 0x6B,
        ADP_CNTL9 = 0x6C,
        ADP_CNTL10 = 0x6D,
        ADP_CNTL11 = 0x6E,
        ADP_CNTL12 = 0x6F,
        ADP_CNTL13 = 0x70,
        ADP_CNTL14 = 0x71,
        ADP_CNTL15 = 0x72,
        ADP_CNTL16 = 0x73,
        ADP_CNTL17 = 0x74,
        ADP_CNTL18 = 0x75,
        ADP_CNTL19 = 0x76,
        INTERNAL_0X7F = 0x7F
    };

    enum class Range : uint8_t
    {
        RANGE_8G = 0b00,
        RANGE_16G = 0b01,
        RANGE_32G = 0b10,
        RANGE_64G = 0b11
    };

    KX134(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName int1,
          PinName int2, PinName rst);

    ~KX134();

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

    /* Saves settings as currently set and disables register writing.
     * Useful for state changes
     */
    void disableRegisterWriting();

    // Set acceleration range (8, 16, 32, or 64 gs)
    void setAccelRange(Range range);

    // Set Output Data Rate Bitwise
    void setOutputDataRateBytes(uint8_t byteHz);

    // Set Output Data Rate from Hz
    void setOutputDataRateHz(uint32_t hz);

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
    void readRegister(Register addr, uint8_t *rx_buf, int size = 2);

    /* Writes a given register a given number of bytes
     *
     * Note: the first byte read should return 0x0, so the data begins at
     * rx_buf[1]
     */
    void writeRegister(Register addr, uint8_t *data, uint8_t *rx_buf,
                       int size = 1);

    /* Writes a given register 1 byte (convenience function, calls
     * writeRegister())
     *
     * Note: the first byte read should return 0x0, so the data begins at
     * rx_buf[1]
     */
    void writeRegisterOneByte(Register addr, uint8_t data, uint8_t *buf);

    /* Reads a value from a low and high address and combines them to create a
     * signed (2s complement) 16-bit integer
     */
    int16_t read16BitValue(Register lowAddr, Register highAddr);

    /* Converts 2 8-bit unsigned integers to a single signed 16-bit (2s
     * complement) integer
     */
    int16_t convertTo16BitValue(uint8_t low, uint8_t high);

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