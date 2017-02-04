#ifndef I2C_H
#define I2C_H

typedef enum {
    ADDRESS_HIGH = 0,
    ADDRESS_LOW,
    WR_DATA,
} i2c_state_t;

extern void eeprom_wr_callback(uint16_t address, uint8_t data);
extern uint8_t eeprom_rd_callback(uint16_t address);

void i2c_init(void);

#endif /* I2C_H */
