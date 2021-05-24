#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <avr/io.h>

/* ------------------------------------------------------------
 * enable/disable functionalities
 *
 * USE_HELP
 *   enables help command.
 *
 * USE_EEPROM
 *   enables functionalities to save/restore the setting of AD9833
 *   in/from the EEPROM.
 *
 * USE_BOTH_REGISTERS
 *   Define this option, if you want to use both FREG0 and FREG1. If
 *   you define USE_PHASE, you can use PHASE0 and PHASE1, too.
 *
 * USE_PHASE
 *   enables PHASE functionalities. NOT FULLY TESTED.
 *
 * DEBUG
 *   Some further functionalities will be also compiled. (In most
 *   cases you don't need to define this.)
 */

#define USE_HELP
#define USE_EEPROM
#undef USE_BOTH_REGISTERS
#undef USE_PHASE
#undef DEBUG

/* ------------------------------------------------------------
 * UART
 */

#define F_CPU	1000000	/* CPU clock in Hz */
#define BAUD	4800

/* settings for software UART:
   Define the following macros if your AVR has no hardware USART
   module (e.g. ATtiny85).
   These lines have no effect for AVRs which have USART module(s).
 */

#define UART_DDR  DDRB
#define UART_PORT PORTB
#define UART_PIN  PINB
#define UART_RXD  3		/* PB3 (Pin 2 of ATtiny85) */
#define UART_TXD  4		/* PB4 (Pin 3 of ATtiny85) */

/* ------------------------------------------------------------
 * AD9833 control
 *
 * Define here the pins of AVR connected to AD9833.
 * Note that this software does *not* use a hardware SPI module.
 */

#define AD9833_DDR	DDRB
#define AD9833_PORT	PORTB
#define AD9833_FSYNC	0
#define AD9833_SCLK	1
#define AD9833_SDATA 	2

#endif /* __CONFIG_H__ */
