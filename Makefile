PROJECT = avr-eeprom-slave

MCU = atmega328p
F_CPU = 16000000
SLAVE_ADDRESS = 0x1B

CC = avr-gcc
AR = avr-ar
OBJCOPY = avr-objcopy
SIZE = avr-size

DEFINES = -DF_CPU=$(F_CPU) -DSLAVE_ADDRESS=$(SLAVE_ADDRESS)
CFLAGS = $(DEFINES) -Os -std=gnu11 -mmcu=$(MCU) -Wall -I/usr/avr/include
OBJECTS = main.o i2c.o

LDFLAGS = -mmcu=$(MCU)

PROG = avrdude
PROGFLAGS = -p $(MCU) -P usb -c avrispmkii -e

all: libraries $(PROJECT).hex

%.o: %.c
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
