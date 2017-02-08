PROJECT = bootloader

MCU = atmega328p
BOOT_DELAY_MS = 1000
BOOT_IDLE_MS  = 100

ifeq ($(MCU), atmega168)
F_CPU = 8000000
BOOTLOADER_SECTION = 0x3800
SLAVE_ADDRESS = 0x1C
else
F_CPU = 16000000
BOOTLOADER_SECTION = 0x7000
SLAVE_ADDRESS = 0x1B
endif

CC = avr-gcc
AR = avr-ar
OBJCOPY = avr-objcopy
SIZE = avr-size

DEFINES  = -DF_CPU=$(F_CPU) -DSLAVE_ADDRESS=$(SLAVE_ADDRESS)
DEFINES += -DBOOT_DELAY_MS=$(BOOT_DELAY_MS) -DBOOT_IDLE_MS=$(BOOT_IDLE_MS)

CFLAGS = $(DEFINES) -Os -std=gnu11 -mmcu=$(MCU) -Wall -I/usr/avr/include -I../..
OBJECTS = bootloader.o i2c.o

LDFLAGS  = -mmcu=$(MCU) -Ttext=$(BOOTLOADER_SECTION)
LDFLAGS += -Wl,--section-start=.dummy=0x0000

PROG = avrdude
PROGFLAGS = -p $(MCU) -P usb -c avrispmkii -e

all: libraries $(PROJECT).hex

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: ../../%.c
	$(CC) $(CFLAGS) -c $<

$(PROJECT).elf: $(OBJECTS) $(LIBS)
	$(CC) -o $(PROJECT).elf $(OBJECTS) $(LDFLAGS)

$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -R .eeprom -O ihex $< $@

install: $(PROJECT).hex
	$(PROG) $(PROGFLAGS) -U flash:w:$<

size: $(PROJECT).elf
	$(SIZE) --format=avr --mcu=$(MCU) $<

clean:
	rm -f $(OBJECTS) $(PROJECT).elf $(PROJECT).hex


.PHONY: libraries size install
