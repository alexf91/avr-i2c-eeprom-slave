#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#include "i2c.h"


static i2c_state_t state = ADDRESS_HIGH;
static uint16_t address = 0;


void i2c_init(void)
{
    TWAR = (SLAVE_ADDRESS << 1);
    TWCR = (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
}


ISR(TWI_vect)
{
    switch(TW_STATUS) {
        /* Slave receive mode */
        case TW_SR_SLA_ACK:
            TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
            state = ADDRESS_HIGH;
            break;

        case TW_SR_DATA_ACK:
            if(state == ADDRESS_HIGH) {
                address = (TWDR << 8);
                state = ADDRESS_LOW;
            }
            else if(state == ADDRESS_LOW) {
                address |= TWDR;
                state = WR_DATA;
            }
            else if(state == WR_DATA) {
                uint8_t data = TWDR;
                eeprom_wr_callback(address, data);
                ++address;
            }
            TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
            break;

        case TW_SR_STOP:
            TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
            break;

        /* Not allowed */
        case TW_SR_DATA_NACK:
        case TW_SR_GCALL_ACK:
        case TW_SR_ARB_LOST_GCALL_ACK:
        case TW_SR_ARB_LOST_SLA_ACK:
        case TW_SR_GCALL_DATA_ACK:
        case TW_SR_GCALL_DATA_NACK:
            break;

        /* Slave transmit mode */
        case TW_ST_SLA_ACK:
        case TW_ST_DATA_ACK:
            TWDR = eeprom_rd_callback(address);
            TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
            ++address;
            break;

        case TW_ST_DATA_NACK:
            TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
            break;

        /* Not allowed */
        case TW_ST_ARB_LOST_SLA_ACK:
        case TW_ST_LAST_DATA:
            break;
    }
}
