#include "config.h"

#ifdef USE_HELP
#include <avr/pgmspace.h>
#include "uartutil.h"

const char helpmsg[] PROGMEM = 
  "Usage (case insensitive):\n"
#ifdef DEBUG
  "  (Compile options:"
#ifdef USE_HELP
  " USE_HELP"
#endif
#ifdef USE_EEPROM
  " USE_EEPROM"
#endif
#ifdef USE_BOTH_REGISTERS
  " USE_BOTH_REGISTERS"
#endif
#ifdef USE_PHASE
  " USE_PHASE"
#endif
  " DEBUG)\n"
#endif
  "  [freq]: set frequency\n"
  "    e.g.: 100 10k 1.5k 1k5 (= 1.5k) 2M\n"
  "  S: sine wave\n"
  "  T: triangle wave\n"
  "  Q: square wave\n"
#ifdef DEBUG
  "  M: MSB2\n"
#endif
  "\n"

#ifdef USE_PHASE
  "  A[number]: set phase\n"
  "\n"
#endif

#ifdef USE_BOTH_REGISTERS
  "  F[0|1], F: select/toggle Freg\n"
#ifdef USE_PHASE
  "  P[0|1], P: select/toggle Preg\n"
#endif
  "\n"
#endif

#ifdef DEBUG
  "  U[num]: set FREG-MSB (14 bits)\n"
  "  L[num]: set FREG-LSB (14 bits)\n"
  "  W[0|1|2|3]: set SLEEP1, SLEEP12\n"
  "\n"
#endif

  "  RESET: send reset to AD9833\n"
  "\n"

#ifdef USE_EEPROM
  "  SAVE: save current registers \n"
  "  LOAD: restore saved registers\n"
  "  ERASE: erase saved registers\n"
#ifdef DEBUG
  "  ROM: dump EEPROM\n"
#endif
  "\n"
#endif

  "  H, ?: help\n";

void
help()
{
  PGM_P str = helpmsg;
  char c;

  while ((c = pgm_read_byte_near(str++)) != '\0') {
    if (c == '\n') {
      uart_putchar(0x0d);
      uart_putchar(0x0a);
    } else {
      uart_putchar(c);
    }
  }
}  

#endif /* USE_HELP */
