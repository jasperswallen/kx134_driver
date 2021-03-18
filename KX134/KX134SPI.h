#ifndef KX134SPI_H
#define KX134SPI_H

#include "KX134Base.h"

/**
 * @brief SPI implementation of KX134 driver
 */
class KX134SPI : public KX134Base
{
public:
    /**
     * @brief Construct a new KX134 driver using the SPI communication protocol
     *
     * @param[in] debug The debug port to output debug messages to
     * @param[in] mosi The SPI MOSI pin
     * @param[in] miso The SPI MISO pin
     * @param[in] sclk The SPI SCLK pin
     * @param[in] cs The chip select pin
     */
    KX134SPI(Stream* debug, PinName mosi, PinName miso, PinName sclk, PinName cs);

    /**
     * @brief Initializes the KX134
     *
     * @return true if the init is successful, false otherwise
     */
    virtual bool init() override;

protected:
    /**
     * @brief Reads a given register a given number of bytes
     *
     * @param[in] addr The register to read from
     * @param[out] rx_buf The buffer to read into
     * @param[in] size The number of bytes to read
     */
    virtual void readRegister(Register addr, char* rx_buf, int size = 1) override;

    /**
     * @brief Writes data to a given register
     *
     * @param[in] addr The register to write to
     * @param[in] data The data to write
     * @param[out] rx_buf The response to receive
     * @param[in] size The number of bytes to write.
     */
    virtual void writeRegister(Register addr, char* data, char* rx_buf = nullptr, int size = 1) override;

    /**
     * @brief Deselect (push high) chip select pin to let other devices perform transactions
     */
    void deselect();

    /**
     * @brief Select (push low) chip select pin to allow transactions
     */
    void select();

private:
    /** @brief The SPI interface */
    SPI _spi;

    /** @brief The chip select pin */
    DigitalOut _cs;
};

#endif
