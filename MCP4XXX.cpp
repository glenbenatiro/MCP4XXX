#include "MCP4XXX.h"

#include <cmath>
#include <stdexcept>
#include <iostream>

MCP4XXX::
MCP4XXX (MCP4XXX::PART_NUMBER part, 
         MCP4XXX::RESISTANCE_VERSION resistance)
{
  total_resistance  (resistance);
  resolution        (part);
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
      break;
    }

    case MCP4XXX::PART_NUMBER::MCP4161:
    case MCP4XXX::PART_NUMBER::MCP4162:
    case MCP4XXX::PART_NUMBER::MCP4261:
    case MCP4XXX::PART_NUMBER::MCP4262:
    {
      m_resolution_bits = 8;
      break;
    }
  }

  m_resolution_int  = std::pow (2, m_resolution_bits);
  m_step_resistance = m_max_resistance / m_resolution_int;
}

void MCP4XXX::
total_resistance (MCP4XXX::RESISTANCE_VERSION resistance)
{
  m_max_resistance = static_cast<double>(resistance) + WIPER_RESISTANCE;
}

void MCP4XXX:: 
theoretical_resistance (unsigned wiper_value)
{
  double val = ((m_max_resistance * (wiper_value)) / m_resolution_int) + 
    WIPER_RESISTANCE;

  m_theoretical_resistance = val;
}

void MCP4XXX::
write_data (uint8_t          addr, 
            MCP4XXX::COMMAND cmd)
{
  char txdata = ((addr & 0xF) << 4) | ((static_cast<uint8_t>(cmd) & 0x3) << 2);
  char rxdata = 0;

  spi_xfer (&rxdata, &txdata, 1);
}

void MCP4XXX:: 
write_data (uint8_t           addr,
            MCP4XXX::COMMAND  cmd, 
            uint16_t          data)
{
  uint16_t temp = (addr & 0xF) << 12                      | 
                  (static_cast<uint8_t>(cmd) & 0x3) << 10 |
                  (data & 0x3FF);
                        
  char txdata[2] = {static_cast<char>((temp & 0xFF00) >> 8), 
                    static_cast<char>(temp & 0xFF)}; // MS byte sent first
  char rxdata[2] = {0, 0};

  spi_xfer (rxdata, txdata, 2);
}

bool MCP4XXX::
is_valid_resistance (double value)
{
  return (value <= m_max_resistance && value >= MIN_RESISTANCE);
}

uint16_t MCP4XXX:: 
hw_wiper_value ()
{
  return 0;
}

void MCP4XXX::
increment (bool     channel,
           unsigned steps)
{
  for (int i = 0; i < steps; i++)
  {
    if (m_wiper_value >= m_resolution_int)
    {
      break;
    }
    else 
    {
      write_data (
        static_cast<uint8_t>(channel ? WIPER_ADDR::VOLATILE_WIPER_1 : 
          WIPER_ADDR::VOLATILE_WIPER_0),
        COMMAND::INCREMENT
      );

      theoretical_resistance (++m_wiper_value);
    }
  }
}

void MCP4XXX::
decrement  (bool     channel,
            unsigned steps)
{
  for (int i = 0; i < steps; i++)
  {
    if (m_wiper_value <= 0)
    {
      break;
    }
    else 
    {
      write_data (
        static_cast<uint8_t>(channel ? WIPER_ADDR::VOLATILE_WIPER_1 : 
          WIPER_ADDR::VOLATILE_WIPER_0),
        COMMAND::DECREMENT
      );

      theoretical_resistance (--m_wiper_value);
    }
  }
}

double MCP4XXX::
resistance (bool   channel,
            double value)
{
  if (is_valid_resistance (value))
  {
    double n = std::round ((m_resolution_int * 
      (value - WIPER_RESISTANCE)) / m_max_resistance);

    std::cout << "n is: " << n << std::endl; 

    m_wiper_value = static_cast<uint16_t>(n);

    write_data (
      static_cast<uint8_t>(channel ? WIPER_ADDR::VOLATILE_WIPER_1 : 
        WIPER_ADDR::VOLATILE_WIPER_0),
      COMMAND::WRITE,
      m_wiper_value
    );

    theoretical_resistance (m_wiper_value);
  }
  else 
  {
    throw (std::out_of_range ("Resistance value invalid."));
  }

  return (m_theoretical_resistance);
}

double MCP4XXX::
resistance_per (bool   channel,
                double value)
{
  if (value <= 100.0 && value >= 0.0)
  { 
    double actual_resistance = ((value / 100.0) * (m_max_resistance - 
     MIN_RESISTANCE)) + MIN_RESISTANCE;

     return (resistance (channel, actual_resistance));
  }
  else 
  {
    throw (std::out_of_range ("resistance_per () value invalid."));
  }
}

double MCP4XXX::
max_resistance ()
{
  return (m_max_resistance);
}

double MCP4XXX::
min_resistance ()
{
  return (MIN_RESISTANCE);
}

// eof

