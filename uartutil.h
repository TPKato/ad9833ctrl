#ifndef __UARTUTIL_H__
#define __UARTUTIL_H__

void uart_init();
char uart_getchar();
void uart_putchar(char c);
void uart_puts(char *str);

#endif
