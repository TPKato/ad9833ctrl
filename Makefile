#
# Makefile for ad9833ctrl
#

#DEVICE = atmega328p
#DEVICE = atmega8
DEVICE = attiny85

# settings of writer
AVRDUDE = avrdude
AVRWRITER = sparkfun
AVRDUDEBAUDRATE = 2400


# ------------------------------------------------------------
# In most cases you don't need to change any of the lines after this,
# if you are using GNU Toolchain and AVRDUDE.
# ------------------------------------------------------------

CC = avr-gcc
CFLAGS = -Wall -Os -mmcu=$(DEVICE)

AS = $(CC)
ASFLAGS = $(CFLAGS)

LDFLAGS = -mmcu=$(DEVICE)

OBJCOPY = avr-objcopy

SIZE = avr-size

# ------------------------------------------------------------
TARGET = ad9833ctrl

all: $(TARGET).hex

$(TARGET): $(TARGET).o ad9833.o uart.o uartutil.o help.o

$(TARGET).hex: $(TARGET)

flash: $(TARGET).hex
	$(AVRDUDE) -c $(AVRWRITER) -p $(DEVICE) -b $(AVRDUDEBAUDRATE) -e -U flash:w:$<

clean:
	rm -f $(TARGET) *.o

distclean: clean
	rm -f $(TARGET).hex

$(TARGET).o: $(TARGET).c config.h Makefile
ad9833.o: ad9833.c ad9833.h config.h Makefile
uart.o: uart.S config.h Makefile
uartutil.o: uartutil.c uartutil.h config.h Makefile
help.o: help.c config.h Makefile

%.hex: %
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	$(SIZE) $@
