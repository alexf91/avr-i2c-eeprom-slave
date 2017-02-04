#include <avr/io.h>
#include <avr/interrupt.h>

#include "i2c.h"

void eeprom_wr_callback(uint16_t addr, uint8_t data)
{

}

uint8_t eeprom_rd_callback(uint16_t addr)
{
    return addr & 0xFF;
}

int main(void)
{
    i2c_init();
    sei();
    while(1) {
        ;
    }
}
