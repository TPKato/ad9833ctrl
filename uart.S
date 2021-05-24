;;; Software UART based on Application Note AVR305
;;; https://www.microchip.com/wwwAppNotes/AppNotes.aspx?appnote=en591261

#include "config.h"

#if !defined(UBRR0H) && !defined(UBRRH)

#include <avr/io.h>

#ifndef UART_DELAY_VALUE
/* Note: "+3" is added for rounding (instead of rounding down) */
#define UART_DELAY_VALUE (((F_CPU / BAUD) - 23 + 3) / 6)
#endif

#define RBITCNT		r18
#define RDELAYTEMP	r19

.global uart_putchar
.global uart_getchar

.section .text

;;; send a char
uart_putchar:	ldi	RBITCNT, 10	; 1 + 8 + 1 stop bit (8N1)
		com	r24
		sec
putchar0:	brcc	putchar1
		cbi	_SFR_IO_ADDR(UART_PORT), UART_TXD
		rjmp	putchar2
putchar1:	sbi	_SFR_IO_ADDR(UART_PORT), UART_TXD
		nop
putchar2:	rcall	uart_delay
		rcall	uart_delay
		lsr	r24
		dec	RBITCNT
		brne	putchar0
		ret

;;; receive a char
uart_getchar:	ldi	RBITCNT, 9	; 8 + 1 stop bit
getchar1:	sbic	_SFR_IO_ADDR(UART_PIN), UART_RXD
		rjmp	getchar1
		rcall	uart_delay
getchar2:	rcall	uart_delay
		rcall	uart_delay
		clc
		sbic	_SFR_IO_ADDR(UART_PIN), UART_RXD
		sec
		dec	RBITCNT
		breq	getchar3
		ror	r24
		rjmp	getchar2
getchar3:	ret

;;; delay
uart_delay:	ldi	RDELAYTEMP, UART_DELAY_VALUE
uart_delay1:	dec	RDELAYTEMP
		brne	uart_delay1
		ret

#endif /* !UBRR0H && !UBRRH */
