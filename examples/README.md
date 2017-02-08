# I2C Bootloader

Emulates a AT24C512 EEPROM. After booting, the bootloader waits for
some time and programs the flash if data is received.

Programming can be done with `cat blinky.bin | eeprog /dev/i2c-1 0x1b -f -16 -w 0`
