#include <avr/io.h>
#include <string.h>

#include "config.h"
#include "ad9833.h"
#include "uartutil.h"
#include "version.h"

#ifdef USE_HELP
void help(); /* help.S */
#endif

#ifdef USE_EEPROM
#include <avr/eeprom.h>
#endif

#define BUFSIZE 32 /* must be < 256 (unsigned char) */

void
set_freq(char *str)
{
  float freq = 0;
  float base = 1;
  long mul = 1;
  char dec = 0;
  char err = 0;

  while (*str != '\0') {
    if (('0' <= *str) && (*str <= '9')) {
      if (dec == 0) {
	freq *= 10.0;
	freq += (float)(*str - '0');
      } else {
	base /= 10.0;
	freq += (float)(*str - '0') * base;
      }
    } else if (*str == '.') {
      if ((mul > 1) || (dec == 1)) {
	err = 1;
      }
      dec = 1;
    } else if (*str == 'K') {
      if (mul > 1) {
	err = 1;
      }
      mul = 1000;
      dec = 1;
    } else if (*str == 'M') {
      if (mul > 1) {
	err = 1;
      }
      mul = 1000000;
      dec = 1;
    } else {
      err = 1;
    }
    str++;
  }

  if (err == 1) {
    uart_puts("? invalid param");
  } else {
    /* 10.73741824 = 2^28 / 25 MHz (f_MCLK = 25 MHz) */
    AD9833_frequency_set((long)(10.73741824 * freq * (float)mul));
  }
}

#if defined(DEBUG) || defined(USE_PHASE)
long
strtolong(char *str)
{
  long data = 0;

  while (*str != '\0') {
    if (('0' <= *str) && (*str <= '9')) {
      data *= 10;
      data += (*str - '0');
    }
    str++;
  }
  return data;
}
#endif

#ifdef USE_EEPROM
void
save_registers()
{
  int addr = 0;
  int i;
  char version = 0x01;

  /* magic */
  eeprom_write_dword((uint32_t *)addr, 0xad9833 << 8 | version);
  do {} while (! eeprom_is_ready());
  addr += 4;

  /* status */
  eeprom_write_word((uint16_t *)addr, AD9833_get_status());
  do {} while (! eeprom_is_ready());
  addr += 2;

  for (i = 0; i <= 1; i++) {
    /* freq */
    eeprom_write_dword((uint32_t *)addr, AD9833_get_freq(i));
    do {} while (! eeprom_is_ready());
    addr += 4;

    /* phase */
    eeprom_write_word((uint16_t *)addr, AD9833_get_phase(i));
    do {} while (! eeprom_is_ready());
    addr += 2;
  }
  /* uart_puts("saved"); */
}

int
load_registers()
{
  unsigned long uldata;
  int idata;
  int addr = 0;
  int i;

  /* magic */
  uldata = eeprom_read_dword((uint32_t *)addr);
  if ((uldata >> 8) != 0xad9833) {
    return -1;
  }
  addr += 4;

  /* status */
  idata = eeprom_read_word((uint16_t *)addr);
  AD9833_restore_status(idata);
  addr += 2;

  for (i = 0; i <= 1; i++) {
    /* freq */
    uldata = eeprom_read_dword((uint32_t *)addr);
    if ((long)uldata > 0) {
      AD9833_frequency_set_B28_by_reg(uldata, i);
    }
    addr += 4;

    /* phase */
#ifdef USE_PHASE
    idata = eeprom_read_word((uint16_t *)addr);
    if (idata >= 0) {
      AD9833_phase_set_by_reg(idata, i);
    }
#endif
    addr += 2;
  }
  return 0;
}

void
eeprom_erase()
{
  eeprom_write_dword((uint32_t *)0, -1);
  do {} while (! eeprom_is_ready());
  /* uart_puts("erased"); */
}

#ifdef DEBUG
void
puthex1(unsigned char n)
{
  if (n < 10) {
    uart_putchar(n + '0');
  } else {
    uart_putchar(n + 'A' - 10);
  }
}

void
puthex(unsigned char d)
{
  puthex1((d >> 4) & 0x0f);
  puthex1(d & 0x0f);
}

void
eeprom_dump()
{
  int addr = 0;
  int i, j;
  for (j = 0; j < 4; j++) {
    for (i = 0; i < 8; i++) {
      puthex(eeprom_read_byte((uint8_t *)addr));
      addr++;
      uart_putchar(' ');
    }
    uart_puts("");
  }
}
#endif
#endif

void
run_command(char *str)
{
  if (('0' <= *str) && (*str <= '9')) {
    set_freq(str);

  /* wave form */
  } else if ((*str == 'S') && (*(str + 1) == '\0')) {
    AD9833_wave_sine();
  } else if ((*str == 'T') && (*(str + 1) == '\0')) {
    AD9833_wave_triangle();
  } else if ((*str == 'Q') && (*(str + 1) == '\0')) {
    AD9833_wave_square();
#ifdef DEBUG
  } else if ((*str == 'M') && (*(str + 1) == '\0')) {
    AD9833_wave_MSB2();
#endif

  } else if (! strncmp(str, "RESET", 6)) {
    AD9833_reset();

#ifdef USE_BOTH_REGISTERS
  } else if (*str == 'F') {
    if (*(str + 1) == '\0') {
      AD9833_freg_toggle();
    } else if ((*(str + 1) == '0') && (*(str + 2) == '\0')) {
      AD9833_freg_select(0);
    } else if ((*(str + 1) == '1') && (*(str + 2) == '\0')) {
      AD9833_freg_select(1);
    } else {
      uart_puts("? invalid register");
    }
#endif

#ifdef USE_PHASE
  } else if (*str == 'A') {
    long data = strtolong(str + 1);
    AD9833_phase_set(data);

#ifdef USE_BOTH_REGISTERS
  } else if (*str == 'P') {
    if (*(str + 1) == '\0') {
      AD9833_preg_toggle();
    } else if ((*(str + 1) == '0') && (*(str + 2) == '\0')) {
      AD9833_preg_select(0);
    } else if ((*(str + 1) == '1') && (*(str + 2) == '\0')) {
      AD9833_preg_select(1);
    } else {
      uart_puts("? invalid register");
    }
#endif /* USE_BOTH_REGISTERS */
#endif /* USE_PHASE */

#ifdef USE_EEPROM
  } else if (! strncmp(str, "SAVE", 5)) {
    save_registers();
  } else if (! strncmp(str, "LOAD", 5)) {
    if (load_registers() < 0) {
      uart_puts("? no saved data");
    }
  } else if (! strncmp(str, "ERASE", 6)) {
    eeprom_erase();
#ifdef DEBUG
  } else if (! strncmp(str, "ROM", 4)) {
    eeprom_dump();
#endif
#endif

#ifdef DEBUG
  } else if (*str == 'U') {
    long data = strtolong(str + 1);
    AD9833_frequency_set_u(data);

  } else if (*str == 'L') {
    long data = strtolong(str + 1);
    AD9833_frequency_set_l(data);

  } else if (*str == 'W') {
    if (('0' <= *(str + 1)) && (*(str + 1) <= '3') && (*(str + 2) == '\0')) {
      AD9833_power(((*(str + 1) - '0') & 2) >> 1, (*(str + 1) - '0') & 1);
    } else {
      uart_puts("? invalid param");
    }
#endif

#ifdef USE_HELP
  } else if (((*str == 'H') || (*str == '?')) && (*(str + 1) == '\0')) {
    help();
#endif

#ifdef DEBUG
  } else if (! strncmp(str, "STAT", 5)) {
    /* status (undocumented) */
    unsigned char i;
    unsigned int s = AD9833_get_status();
    for (i = 1; i <= 16; i++) {
      uart_putchar(s & 0x8000 ? '1' : '0');
      s = s << 1;
      if ((i % 4) == 0) {
	uart_putchar(' ');
      }
    }
    uart_puts("");
#endif
  } else {
#ifdef DEBUG
    uart_putchar('>');
    uart_puts(str);
#endif
    uart_puts("? invalid command");
  }
}

int
main(void)
{
  char c;
  char buf[BUFSIZE];
  unsigned char nbuf = 0;
  char toolong = 0;

  uart_init();
#ifdef USE_HELP
  uart_puts("AD9833 CONTROLLER VERSION " AD9833CTRL_VERSION "\n");
#endif

  AD9833_spi_init();
  AD9833_reset();

#ifdef USE_EEPROM
  load_registers();
#endif

  while(1) {
    c = uart_getchar();

    if ((c == 0x0d) || (c == 0x0a)) {
      uart_puts("");
      if (toolong == 1) {
	uart_puts("? too long");
	toolong = 0;
      } else if (nbuf != 0) {
	buf[nbuf] = '\0';
	run_command(buf);
      }
      nbuf = 0;
    } else {
      uart_putchar(c); /* echo */

      if (nbuf >= BUFSIZE) {
	toolong = 1;
      } else {
	if (c == 0x7f) {
	  /* DEL */
	  if (nbuf > 0) {
	    nbuf--;
	    uart_putchar(0x08);
	  }
	} else if (c == 0x15) {
	  /* Ctrl-U */
	  nbuf = 0;
	  uart_putchar(0x0d);
	} else if (c == ' ') {
	  /* just ignore */
	} else {
	  if (('a' <= c) && (c <= 'z')) {
	    buf[nbuf++] = c + 'A' - 'a';
	  } else {
	    buf[nbuf++] = c;
	  }
	}
      }
    }
  }
  return 0;
}
