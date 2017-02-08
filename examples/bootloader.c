#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "i2c.h"

volatile uint8_t i2c_accessed = 0;

volatile int8_t page_active = 0;
volatile uint16_t current_page = 0;
volatile uint8_t pagebuffer[SPM_PAGESIZE];

void finish_page_write()
{
    if(page_active) {
        boot_page_erase_safe(current_page * SPM_PAGESIZE);
        boot_spm_busy_wait();
        for(uint16_t i=0; i<SPM_PAGESIZE; i+=2) {

            uint16_t w = pagebuffer[i];
            w |= (pagebuffer[i+1] << 8);

            boot_page_fill(current_page * SPM_PAGESIZE + i, w);
        }
        boot_page_write_safe(current_page * SPM_PAGESIZE);
        boot_spm_busy_wait();
        eeprom_busy_wait();
        boot_rww_enable();
    }
    page_active = 0;
}

void eeprom_wr_callback(uint16_t addr, uint8_t data)
{
    i2c_accessed = 1;

    uint16_t page = addr / SPM_PAGESIZE;
    uint8_t  offs = addr % SPM_PAGESIZE;

    if(page != current_page || page_active == 0) {
        /* Writing to a new page, so we need to write the old page and
         * read the content of the new page into the page buffer */
        finish_page_write();
        page_active = 1;
        current_page = page;

        for(uint16_t i=0; i<SPM_PAGESIZE; i++)
            pagebuffer[i] = pgm_read_byte(current_page * SPM_PAGESIZE + i);

    }
    pagebuffer[offs] = data;
}


uint8_t eeprom_rd_callback(uint16_t addr)
{
    i2c_accessed = 1;

    /* Write the old page before reading */
    finish_page_write();
    return pgm_read_byte(addr);
}


int main(void)
{
    uint8_t tmp;

    /* Move interrupt addresses to boot section */
    cli();
    tmp = MCUCR;
    MCUCR = tmp | (1<<IVCE);
    MCUCR = tmp | (1<<IVSEL);

    /* Initialize HW and wait for commands */
    i2c_init();
    sei();

    /* Wait a few seconds for I2C data */
    _delay_ms(BOOT_DELAY_MS);

    /* Boot if we don't receive anything for 100 ms */
    do {
        _delay_ms(BOOT_IDLE_MS);
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            tmp = i2c_accessed;
            i2c_accessed = 0;
        }
    } while(tmp);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        finish_page_write();
    }

    /* Move interrupt addresses to application section */
    cli();
    tmp = MCUCR;
    MCUCR = tmp | (1<<IVCE);
    MCUCR = tmp & ~(1<<IVSEL);

    i2c_deinit();

    void (*start)(void) = 0x0000;
    start();

    return 0;
}


__attribute__ ((section (".dummy")))
void loopy(void)
{
    while(1);
}
