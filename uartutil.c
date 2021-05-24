#include "config.h"
#include <avr/io.h>

#if defined(UBRR0H) || defined(UBRRH)
#define USE_HARDWARE_UART

#ifdef UBRR0H
#define UCSRA	UCSR0A
#define UCSRB	UCSR0B
#define UCSRC	UCSR0C
#define RXEN	RXEN0
#define TXEN	TXEN0
#define UBRRH	UBRR0H
#define UBRRL	UBRR0L
#define UCSZ0	UCSZ00
#define U2X	U2X0
#define RXC	RXC0
#define UDR	UDR0
#define UDRE	UDRE0
#endif
#endif /* UBRR0H || UBRRH */

#ifdef USE_HARDWARE_UART
#define USE_2X 1
#include <util/setbaud.h>
#else
extern char uart_getchar();
extern void uart_putchar(char c);
#endif

#ifdef USE_HARDWARE_UART
void
uart_init() {
  /* 8 bit, no parity, 1 stop bit */
  UCSRB = (1 << RXEN) | (1 << TXEN);
  UCSRC = (3 << UCSZ0);

  UBRRH = UBRRH_VALUE;
  UBRRL = UBRRL_VALUE;

#if USE_2X
  UCSRA |= (1 << U2X);
#else
  UCSRA &= ~(1 << U2X);
#endif
}

char
uart_getchar() {
  while (!(UCSRA & (1 << RXC)));
  return UDR;
}

void
uart_putchar(char c) {
  /* wait until data register empty */
  while (! (UCSRA & (1 << UDRE)));
  UDR = c;
}

#else

void
uart_init() {
  UART_DDR  |= (1 << UART_TXD);
  UART_PORT |= (1 << UART_TXD);
}

#endif

void
uart_puts(char *str)
{
  while (*str != '\0') {
    if (*str == '\n') {
      uart_putchar(0x0d);
      uart_putchar(0x0a);
    } else {
      uart_putchar(*str);
    }
    str++;
  }
  uart_putchar(0x0d);
  uart_putchar(0x0a);
}
