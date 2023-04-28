#ifndef MCP4XXX_H
#define MCP4XXX_H

#include <cstdint>

class MCP4XXX
{
  // https://ww1.microchip.com/downloads/en/DeviceDoc/22059b.pdf
  public:
    enum class MEM_ADDR
    {
      V_WIPER_0   = 0x00,
      V_WIPER_1   = 0x01,
      NV_WIPER_0  = 0x02,
      NV_WIPER_1  = 0X03,
      TCON_REG    = 0x04,
      STAT_REG    = 0x05,
      DATA_0      = 0x06,
      DATA_1      = 0x07,
      DATA_2      = 0x08,
      DATA_3      = 0x09,
      DATA_4      = 0x0a,
      DATA_5      = 0x0b,
      DATA_6      = 0x0c,
      DATA_7      = 0x0d,
      DATA_8      = 0x0e,
      DATA_9      = 0x0f
    };

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

    enum class COMMAND
    {
      WRITE     = 0,
      INCREMENT = 1,
      DECREMENT = 2,
      READ      = 3
    };

    static constexpr double WIPER_RESISTANCE  = 75; // ideal resistance

  private: 
    void*     m_controller;
    uint16_t  m_wiper_value;
    unsigned  m_resolution_int;
    unsigned  m_resolution_bits;
    double    m_max_resistance;
    double    m_step_resistance;
    double    m_theoretical_resistance;
    double    m_min_resistance = WIPER_RESISTANCE;
    
    void      resolution              (MCP4XXX::PART_NUMBER part);
    void      total_resistance        (MCP4XXX::RESISTANCE_VERSION resistance);
    double    theoretical_resistance  (unsigned wiper_value);

  public:
    MCP4XXX   (MCP4XXX::PART_NUMBER part, MCP4XXX::RESISTANCE_VERSION resistance);
    ~MCP4XXX  ();

    void    write_data          (MCP4XXX::MEM_ADDR addr, MCP4XXX::COMMAND cmd);
    void    write_data          (MCP4XXX::MEM_ADDR addr, MCP4XXX::COMMAND cmd, uint16_t data);
    void    increment           (unsigned steps = 1);
    void    decrement           (unsigned steps = 1);
    double  resistance          (bool channel, double value);
    bool    is_valid_resistance (double value);

    virtual void init           (void* controller)            = 0;
    virtual void spi_write      (char* data, unsigned length) = 0;
};

#endif

// eof