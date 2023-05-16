#ifndef MCP4XXX_H
#define MCP4XXX_H

#include <cstdint>

class MCP4XXX
{
  // https://ww1.microchip.com/downloads/en/DeviceDoc/22059b.pdf

  private:
    enum class WIPER_ADDR
    {
      VOLATILE_WIPER_0      = 0x00,
      VOLATILE_WIPER_1      = 0x01,
      NON_VOLATILE_WIPER_0  = 0x02,
      NON_VOLATILE_WIPER_1  = 0x03
    };

    enum class REG_ADDR
    {
      TCON    = 0x04,
      STATUS  = 0x05
    };

    enum class DATA_ADDR  
    {
      DATA_0 = 0x06,
      DATA_1 = 0x07,
      DATA_2 = 0x08,
      DATA_3 = 0x09,
      DATA_4 = 0x0a,
      DATA_5 = 0x0b,
      DATA_6 = 0x0c,
      DATA_7 = 0x0d,
      DATA_8 = 0x0e,
      DATA_9 = 0x0f
    };

    enum class COMMAND
    {
      WRITE     = 0,
      INCREMENT = 1,
      DECREMENT = 2,
      READ      = 3
    };

    static constexpr double WIPER_RESISTANCE  = 75; 

  protected:
    void*     m_controller;
    unsigned  m_SPI_channel;

  public:
    enum class PART_NUMBER
    {
      MCP4141,
      MCP4142,
      MCP4161,
      MCP4162,
      MCP4241,
      MCP4242,
      MCP4261,
      MCP4262
    };

    enum class RESISTANCE_VERSION
    {
      _502  = 5'000,
      _103  = 10'000,
      _503  = 50'000,
      _104  = 100'000
    };

  private: 
    uint16_t  m_wiper_value             = 0x80;
    unsigned  m_resolution_int          = 8,
              m_resolution_bits         = 256;
    double    m_max_resistance          = 50'000.0,
              m_step_resistance         = 195.3125,
              m_theoretical_resistance  = 25'000.0 + WIPER_RESISTANCE,
              m_min_resistance          = WIPER_RESISTANCE;
    
    void      resolution                  (MCP4XXX::PART_NUMBER part);
    void      total_resistance            (MCP4XXX::RESISTANCE_VERSION resistance);
    void      theoretical_resistance      (unsigned wiper_value);
    void      write_data                  (uint8_t addr, MCP4XXX::COMMAND cmd);
    void      write_data                  (uint8_t addr, MCP4XXX::COMMAND cmd, uint16_t data);
    bool      is_valid_resistance         (double value);
    uint16_t  hw_wiper_value              ();
  
  protected:
    virtual void spi_xfer (char* rxd, char* txd, unsigned length) = 0;

  public:
    MCP4XXX   (MCP4XXX::PART_NUMBER part, MCP4XXX::RESISTANCE_VERSION resistance);
    
    void    increment   (bool channel, unsigned steps = 1);
    void    decrement   (bool channel, unsigned steps = 1);
    double  resistance  (bool channel, double value);
};

#endif

// eof