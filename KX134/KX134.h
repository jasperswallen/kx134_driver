/**
 * @file KX134.h
 * @author Jasper Swallen
 * @brief Software Driver for KX134-1211 accelerometer
 * @date 2021-02-20
 * @copyright Copyright (c) 2021
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
#include "platform/Stream.h"

/**
 * @brief The KX134 Driver
 */
class KX134
{
public:
    /**
     * @brief The possible acceleration ranges
     */
    enum class Range : uint8_t
    {
        RANGE_8G = 0b00,
        RANGE_16G = 0b01,
        RANGE_32G = 0b10,
        RANGE_64G = 0b11
    };

#ifdef KX_SPI
public:
    /**
     * @brief Construct a new KX134 driver
     *
     * @param[in] debug The debug port to output debug messages to
     * @param[in] mosi The SPI MOSI pin
     * @param[in] miso The SPI MISO pin
     * @param[in] sclk The SPI SCLK pin
     * @param[in] cs The chip select pin
     */
    KX134(Stream* debug, PinName mosi, PinName miso, PinName sclk, PinName cs);

    /**
     * @brief Initializes the KX134
     *
     * @return true if the init is successful, false otherwise
     */
    bool init();

    /**
     * @brief Performs a software reset
     *
     * @return true if the reset was successful, false otherwise
     */
    bool reset();

    /**
     * @brief Verifies the KX134 unit is connected and functioning normally
     *
     * @return true if functioning normally, false otherwise. If false, call reset() and check
     * again.
     */
    bool checkExistence();

    /**
     * @brief Reads the accelerations in LSB immediately
     *
     * @param[out] output The array to read accelerations into. output[0] is X acceleration,
     * output[1] is Y accel, output[2] is Z accel.
     */
    void getAccelerations(int16_t* output);

    /**
     * @brief Returns if the unit is ready to read acceleration data
     *
     * @return true if ready, false otherwise
     */
    bool dataReady();

    /**
     * @brief Converts an LSB value to gravs
     *
     * Note:
     * +-64g: 1LSB = 0.00195g
     * +-32g: 1LSB = 0.00098g
     * +-16g: 1LSB = 0.00049g
     * +-8g: 1LSB = 0.00024g

     * @param[in] lsbValue The value in LSB to convert to gravs
     * @return The value in gravs
     */
    float convertRawToGravs(int16_t lsbValue) const;

    /**
     * @brief Set offsets that will be added to each acceleration reading before it is returned.
     *
     * @param[in] offsets array of 3 integers that will be added to the results
     */
    void setAccelOffsets(int16_t* offsets);

    /**
     * @brief Set acceleration range (8, 16, 32, or 64 gs)
     *
     * @param[in] range The Range to set the acceleration range to
     */
    void setAccelRange(Range range);

    /**
     * @brief Set Output Data Rate from Hz
     *
     * @param[in] hz An integer representation of the ODR in Hz
     */
    void setOutputDataRateHz(uint32_t hz);

    /**
     * @brief Set Output Data Rate Bitwise
     *
     * @param[in] byteHz A bit-wise representation of the ODR
     */
    void setOutputDataRateBytes(uint8_t byteHz);

private:
    /**
     * @brief The list of registers
     */
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

private:
    /**
     * @brief Reads a given register a given number of bytes
     *
     * @param[in] addr The register to read from
     * @param[out] rx_buf The buffer to read into
     * @param[in] size The number of bytes to read
     */
    void readRegister(Register addr, uint8_t* rx_buf, int size = 1);

    /**
     * @brief Writes data to a given register
     *
     * @param[in] addr The register to write to
     * @param[in] data The data to write
     * @param[out] rx_buf The response to receive
     * @param[in] size The number of bytes to write.
     */
    void writeRegister(Register addr, uint8_t* data, uint8_t* rx_buf, int size = 1);

    /**
     * @brief Writes a given register 1 byte
     * Convenience function, calls writeRegister()
     *
     * @param[in] addr The register to write to
     * @param[in] data The data to write
     * @param[out] buf The response data to receive
     */
    void writeRegisterOneByte(Register addr, uint8_t data, uint8_t* buf = nullptr);

    /**
     * @brief Reads a value from a low and high address and combines them to create a signed (2s
     * complement) 16-bit integer
     *
     * @param[in] lowAddr The low address to read from
     * @param[in] highAddr The high address to read from
     * @return The signed representation of their value
     */
    int16_t read16BitValue(Register lowAddr, Register highAddr);

    /**
     * @brief Converts 2 8-bit unsigned integers to a single signed 16-bit (2s complement) integer
     *
     * @param[in] low The 8 lower bits
     * @param[in] high The 8 upper bits
     * @return The signed representation of their value
     */
    int16_t convertTo16BitValue(uint8_t low, uint8_t high);

    /**
     * @brief Enables writing new settings to the ODCNTL and CNTL1 registers
     *
     * After writing settings, register writing is automatically disabled, and this function must be
     * called again to enable it.
     */
    void enableRegisterWriting();

    /**
     * @brief Saves current settings and disables modification to ODCNTL and CNTL1 registers
     */
    void disableRegisterWriting();

    /**
     * @brief Deselect (push high) chip select pin to let other devices perform transactions
     */
    void deselect();

    /**
     * @brief Select (push low) chip select pin to allow transactions
     */
    void select();

private:
    /** @brief The debug port */
    Stream* _debug;

    /** @brief The SPI interface */
    SPI _spi;

    /** @brief The chip select pin */
    DigitalOut _cs;

    /** @brief Calibration offsets in LSB */
    int16_t _offsets[3];

    /**
     * @name CNTL1
     *
     * Control register 1. Read/write control register that controls the main feature set.
     *
     * Note that to properly change the value of these registers, the PC1 bit in CNTL1 register must
     * first be set to “0”.
     * @{
     */

    /**
     * @brief The RES bit determines the performance mode
     *
     * RES = 0 – Low Power mode (higher noise, lower current, 16-bit output data)
     * RES = 1 – High-Performance mode (lower noise, higher current, 16-bit output data)
     */
    bool res;

    /**
     * @brief Data Ready Engine enable bit
     *
     * DRDYE = 0 – Data Ready Engine is disabled
     * DRDYE = 1 – Data Ready Engine is enabled
     */
    bool drdye_enable;

    /**
     * @brief G-range Select (GSEL) bits select the acceleration range of the accelerometer outputs
     * per Table 7. This range is also called a full-scale range of the accelerometer.
     *
     * GSEL1 | GSEL0 | Range
     * ----- | ----- | -----
     * 0     | 0     | +-8g
     * 0     | 1     | +-16g
     * 1     | 0     | +-32g
     * 1     | 1     | +-64g
     */
    bool gsel[2];

    /**
     * @brief Tap/Double-Tap Engine (TDTE) enable bit.
     *
     * TDTE = 0 – Tap/Double-Tap Engine is disabled
     * TDTE = 1 – Tap/Double-Tap Engine is enabled
     */
    bool tdte_enable;

    /**
     * @brief Tilt Position Engine (TPE) enable bit.
     *
     * TPE = 0 – Tilt Position Engine is disabled
     * TPE = 1 – Tilt Position Engine is enabled
     */
    bool tpe_enable;

    /**
     * @}
     */

    /**
     * @name ODCNTL
     *
     * Output data control register that configures the acceleration outputs.
     *
     * Note that to properly change the value of these registers, the PC1 bit in CNTL1 register must
     * first be set to “0”.
     * @{
     */

    /**
     * @brief IIR Filter Bypass mode enable bit
     *
     * IIR_BYPASS = 0 – IIR filter is not bypassed, i.e. filtering is applied (default)
     * IIR_BYPASS = 1 – IIR filter is bypassed.
     *
     * Notes for IIR_BYPASS = 1 setting:
     * 1. Not recommended at OSA<3:0> = 1111 (ODR = 25600Hz)
     * 2. Not recommended in Low Power Mode with AVC<2:0> = 000 setting (no averaging)
     * 3. This setting may reduce the resolution of the output data.
     */
    bool iir_bypass;

    /**
     * @brief Low-Pass filter Roll-Off control
     *
     * LPRO = 0 – IIR filter corner frequency set to ODR/9 (default)
     * LPRO = 1 – IIR filter corner frequency set to ODR/2
     */
    bool lpro;

    /**
     * @brief Fast Start Up Enable bit.
     *
     * The setting of this bit controls the start up time only when accelerometer operates in
     * High-Performance mode with ODR ≤ 200Hz. If fast start up is disabled (FSTUP=0), the start up
     * time in High-Performance mode would vary with ODR. If fast start up is enabled (FSTUP=1), the
     * start up time in High Performance mode would be fixed. See KX134-1211 Product specifications
     * for details.
     *
     * FSTUP = 0 – Fast Start is disabled
     * FSTUP = 1 – Fast Start is enabled
     */
    bool fstup;

    /**
     * @brief Output Data Rate (ODR) settings for accelerometer sensor.
     *
     * The default ODR is 50Hz.
     *
     * OSA3|OSA2|OSA1|OSA0|Output Data Rate (Hz)
     * :--:|:--:|:--:|:--:|:-------------------:
     * 0   |0   |0   |0   |0.781*
     * 0   |0   |0   |1   |1.563*
     * 0   |0   |1   |0   |3.125*
     * 0   |0   |1   |1   |6.25*
     * 0   |1   |0   |0   |12.5*
     * 0   |1   |0   |1   |25*
     * 0   |1   |1   |0   |50*
     * 0   |1   |1   |1   |100*
     * 1   |0   |0   |0   |200*
     * 1   |0   |0   |1   |400*
     * 1   |0   |1   |0   |800**
     * 1   |0   |1   |1   |1600**
     * 1   |1   |0   |0   |3200**
     * 1   |1   |0   |1   |6400**
     * 1   |1   |1   |0   |12800**
     * 1   |1   |1   |1   |25600**
     *
     * <p>* Available in Low Power and High-Performance modes</p>
     * <p>** Available in High-Performance mode only. Accelerometer will default to High-Performance
     * mode regardless of the RES bit setting in CNTL1 register.</p>
     */
    bool osa[4];

    /**
     * @}
     */

#else
public:
    KX134(PinName sda, PinName scl);

    bool init();

    bool reset();

    bool checkExistance();

private:
    /**
     * @brief The list of registers
     */
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

private:
    int writeRegister(Register addr, uint8_t* tx_buf, size_t size = 1);
    int writeRegisterOneByte(Register addr, uint8_t tx_data); // helper

    int readRegister(Register addr, uint8_t *rx_buf, size_t size = 1);
    int readRegisterOneByte(Register addr, uint8_t &rx_buf);

    void select();
    void deselect();

private:
    I2C i2c_;

    const uint8_t i2c_addr = 0x1E; // if connected to GND, 0x1F if connected to IO_VDD

#endif
};

#endif // KX134_H
