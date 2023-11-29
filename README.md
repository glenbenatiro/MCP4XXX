# MCP4XXX

MCP4XXX is a C++ base library for the Microchip Technology MCP4XXX family of 7/8-bit single/dual SPI digital potentiometers.

Use this base class with your SPI library/implementation of your choice.

### How to Use
Create a new class deriving from this base class and implement the virtual function _spi_xfer_. SPI setup and cleanup should be placed in your class' constructor and destructor.
