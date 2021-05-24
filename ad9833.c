/* ad9833.c: driver for AD9833 */

#include "config.h"
#include <avr/io.h>

#define AD9833_B28	13
#define AD9833_HLB	12
#define AD9833_FSELECT	11
#define AD9833_PSELECT	10
#define AD9833_RESET	8
#define AD9833_SLEEP1	7
#define AD9833_SLEEP12	6
#define AD9833_OPBITEN	5
#define AD9833_DIV2	3
#define AD9833_MODE	1

#define AD9833_FREG0 (1 << 14)
#define AD9833_FREG1 (2 << 14)
#define AD9833_PREG0 (3 << 14)
#define AD9833_PREG1 (3 << 14 | 1 << 13)

static unsigned int status = 0;

#ifdef USE_EEPROM
static long REGfreq[2] = {-1, -1};
static int REGphase[2] = {-1, -1};
#endif

static void
setStatus(unsigned int s)
{
  status |= s;
}
static void
unsetStatus(unsigned int s)
{
  status &= ~s;
}
#ifdef USE_BOTH_REGISTERS
static void
toggleStatus(unsigned int s)
{
  status ^= s;
}
#endif

static void
fsync_begin() {
  AD9833_PORT &= ~(1 << AD9833_FSYNC);
}

static void
fsync_end() {
  AD9833_PORT |= (1 << AD9833_FSYNC);
}

static void
AD9833_send(unsigned int d)
{
  int i;

  fsync_begin();
  for (i = 0; i < 16; i++) {
    if (d & (1 << 15)) {
      AD9833_PORT |= (1 << AD9833_SDATA);
    } else {
      AD9833_PORT &= ~(1 << AD9833_SDATA);
    }
    AD9833_PORT &= ~(1 << AD9833_SCLK);
    AD9833_PORT |= (1 << AD9833_SCLK);
    d = d << 1;
  }
  fsync_end();
}

/* ------------------------------------------------------------
   initialize
*/

void
AD9833_spi_init() {
  AD9833_DDR  |= (1 << AD9833_SDATA) | (1 << AD9833_SCLK) | (1 << AD9833_FSYNC);
  AD9833_PORT |= (1 << AD9833_SCLK) | (1 << AD9833_FSYNC);
}

void
AD9833_reset()
{
  AD9833_send(1 << AD9833_RESET);
  AD9833_send(0);
  status = 0;
}

/* ------------------------------------------------------------
   wave form
*/

void
AD9833_wave_sine() {
  unsetStatus(1 << AD9833_MODE | 1 << AD9833_OPBITEN /* | 1 << AD9833_DIV2 : don't care */);
  AD9833_send(status);
}

void
AD9833_wave_triangle() {
  unsetStatus(1 << AD9833_OPBITEN /* | 1 << AD9833_DIV2: don't care */);
  setStatus(1 << AD9833_MODE);
  AD9833_send(status);
}

void
AD9833_wave_square() {
  unsetStatus(1 << AD9833_MODE);
  setStatus(1 << AD9833_OPBITEN | 1 << AD9833_DIV2);
  AD9833_send(status);
}

#ifdef DEBUG
void
AD9833_wave_MSB2()
{
  unsetStatus(1 << AD9833_MODE | 1 << AD9833_DIV2);
  setStatus(1 << AD9833_OPBITEN);
  AD9833_send(status);
}
#endif

/* ------------------------------------------------------------
   frequency
*/

void
AD9833_frequency_set_B28_by_reg(long data, unsigned char reg)
{
  AD9833_send(1 << AD9833_B28 | status);
  AD9833_send(1 << (14 + (reg & 1)) | (data & 0x3fff));
  AD9833_send(1 << (14 + (reg & 1)) | ((data >> 14) & 0x3fff));
#ifdef USE_EEPROM
  REGfreq[reg & 1] = data;
#endif
}

/* change frequency of the currently selected register */
void
AD9833_frequency_set(long data)
{
  AD9833_frequency_set_B28_by_reg(data, (status >> AD9833_FSELECT) & 1);
}

#ifdef USE_BOTH_REGISTERS
void
AD9833_freg_select(unsigned char reg)
{
  unsetStatus(1 << AD9833_FSELECT);
  setStatus((reg & 1) << AD9833_FSELECT);
  AD9833_send(status);
}

void
AD9833_freg_toggle()
{
  toggleStatus(1 << AD9833_FSELECT);
  AD9833_send(status);
}
#endif

#ifdef DEBUG
void
AD9833_frequency_set_MSB_by_reg(long data, unsigned char reg)
{
  AD9833_send(1 << AD9833_HLB | status);
  AD9833_send(1 << (14+(reg & 1)) | (data & 0x3fff));
#ifdef USE_EEPROM
  /* This might not be exactly same as AD9833 itself, because the
     initial values of FREQ0/1 are unknown. */
  if (REGfreq < 0) {
    REGfreq[reg & 1] = 0;
  }
  REGfreq[reg & 1] = ((data & 0x3fff) << 14) | ((REGfreq[reg & 1] & 0x3fff));
#endif
}

void
AD9833_frequency_set_LSB_by_reg(long data, unsigned char reg)
{
  AD9833_send(status);
  AD9833_send(1 << (14+(reg & 1)) | (data & 0x3fff));
#ifdef USE_EEPROM
  if (REGfreq < 0) {
    REGfreq[reg & 1] = 0;
  }
  REGfreq[reg & 1] = (REGfreq[reg & 1] & 0x0fffc000) | (data & 0x3fff);
#endif
}

void
AD9833_frequency_set_u(long data)
{
  AD9833_frequency_set_MSB_by_reg(data, (status >> AD9833_FSELECT) & 1);
}

void
AD9833_frequency_set_l(long data)
{
  AD9833_frequency_set_LSB_by_reg(data, (status >> AD9833_FSELECT) & 1);
}
#endif

/* ------------------------------------------------------------
   phase
*/

#ifdef USE_PHASE
void
AD9833_phase_set_by_reg(long data, unsigned char reg)
{
  AD9833_send(AD9833_PREG0 | (reg & 1) << 13 | (data & 0x0fff));
#ifdef USE_EEPROM
  REGphase[reg & 1] = data & 0xfff;
#endif
}

void
AD9833_phase_set(long data)
{
  AD9833_phase_set_by_reg(data, (status >> AD9833_PSELECT) & 1);
}

#ifdef USE_BOTH_REGISTERS
void
AD9833_preg_select(unsigned char reg)
{
  unsetStatus(1 << AD9833_PSELECT);
  setStatus((reg & 1) << AD9833_PSELECT);
  AD9833_send(status);
}

void
AD9833_preg_toggle()
{
  toggleStatus(1 << AD9833_PSELECT);
  AD9833_send(status);
}
#endif
#endif

/* ------------------------------------------------------------
   power
 */

#ifdef DEBUG
void
AD9833_power(unsigned char sleep1, unsigned char sleep12)
{
  unsetStatus(1 << AD9833_SLEEP1 | 1 << AD9833_SLEEP12);
  setStatus((sleep1 & 1) << AD9833_SLEEP1 | (sleep12 & 1) << AD9833_SLEEP12);
  AD9833_send(status);
}
#endif

/* ------------------------------------------------------------
   registers
*/

#ifdef USE_EEPROM
void
AD9833_restore_status(unsigned int s)
{
  status = s;
  AD9833_send(status);
}

long AD9833_get_freq(int reg)
{
  return REGfreq[reg & 1];
}

int AD9833_get_phase(int reg)
{
  return REGphase[reg & 1];
}
#endif

#if defined(DEBUG) || defined(USE_EEPROM)
unsigned int
AD9833_get_status()
{
  return status;
}
#endif
