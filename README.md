# AVR I2C-EEPROM slave

Emulates a AT24C512 EEPROM in an AVR microcontroller.

The interface to the application using this is quite easy. The application
needs to provide two functions:

* `void eeprom_wr_callback(uint16_t addr, uint8_t data)`
* `uint8_t eeprom_rd_callback(uint16_t addr)`

I've only tested it with `eeprog` on a Raspberry Pi. `eeprog` only reads and writes
one byte at a time, so the rest is pretty much untested.

If you encounter transmission errors, try reducing the speed of the interface on both
the AVR and the master. On the AVR, this can be done by editing the prescaler and
bitrate register in the init function in [i2c.c](i2c.c).
