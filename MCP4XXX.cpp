#include "MCP4XXX.h"

#include <cmath>
#include <stdexcept>

MCP4XXX::
MCP4XXX (MCP4XXX::PART_NUMBER part, 
         MCP4XXX::RESISTANCE_VERSION resistance)
{
  resolution        (part);
  total_resistance  (resistance);
}

MCP4XXX::
~MCP4XXX ()
{

}

void MCP4XXX::
resolution (MCP4XXX::PART_NUMBER part)
{
  switch (part)
  {
    case MCP4XXX::PART_NUMBER::MCP4141:
    case MCP4XXX::PART_NUMBER::MCP4142:
    case MCP4XXX::PART_NUMBER::MCP4241:
    case MCP4XXX::PART_NUMBER::MCP4242:
    {
      m_resolution_bits = 7;
      m_resolution_int  = 128;

      break;
    }

    case MCP4XXX::PART_NUMBER::MCP4161:
    case MCP4XXX::PART_NUMBER::MCP4162:
    case MCP4XXX::PART_NUMBER::MCP4261:
    case MCP4XXX::PART_NUMBER::MCP4262:
    {
      m_resolution_bits = 8;
      m_resolution_int  = 256;

      break;
    }
  }
}

void MCP4XXX::
total_resistance (MCP4XXX::RESISTANCE_VERSION resistance)
{
  m_max_resistance = static_cast<double>(resistance) + WIPER_RESISTANCE;
}

double MCP4XXX::
theoretical_resistance (unsigned wiper_value)
{
  double val = ((m_max_resistance * (wiper_value)) / m_resolution_int) + 
    WIPER_RESISTANCE;
  
  return (val);
}

void MCP4XXX::
write_data (MCP4XXX::MEM_ADDR addr, 
            MCP4XXX::COMMAND cmd)
{
  char txdata = (static_cast<uint8_t>(addr) & 0xf) << 4 | 
                (static_cast<uint8_t>(cmd) & 0x3) << 2;
                        
  spi_write (&txdata, 1);
}

void MCP4XXX:: 
write_data (MCP4XXX::MEM_ADDR addr,
            MCP4XXX::COMMAND  cmd, 
            uint16_t          data)
{
  uint8_t temp      = (static_cast<uint8_t>(addr) & 0xf) << 12 | 
                      (static_cast<uint8_t>(cmd)  & 0x3) << 10 | 
                      (data & 0x3ff);
                        
  char    txdata[2] = {temp & 0xff, (temp & 0xff00) >> 8};

  spi_write (txdata, 2);
}

void MCP4XXX::
increment  (unsigned steps)
{

}

void MCP4XXX::
decrement  (unsigned steps)
{

}

double MCP4XXX::
resistance (bool   channel,
            double value)
{
  if (is_valid_resistance (value))
  {
    double n = std::round ((m_resolution_int * 
      (value - WIPER_RESISTANCE)) / m_max_resistance);

    m_wiper_value = static_cast<uint16_t>(n);

    write_data (
      channel ? MEM_ADDR::V_WIPER_1 : MEM_ADDR::V_WIPER_0,
      COMMAND::WRITE,
      m_wiper_value
    );

    m_theoretical_resistance = theoretical_resistance (m_wiper_value);
  }
  else 
  {
    throw (std::out_of_range ("Resistance value invalid."));
  }

  return (m_theoretical_resistance);
}

bool MCP4XXX::
is_valid_resistance (double value)
{
  if (value <= m_max_resistance && value >= m_min_resistance)
  {
    return (true);
  }
  else 
  {
    return (false);
  }
}

