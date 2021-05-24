#ifndef __AD9833_H__
#define __AD9833_H__

/* initialize */
void AD9833_spi_init();
void AD9833_reset();

/* wave form */
void AD9833_wave_sine();
void AD9833_wave_triangle();
void AD9833_wave_square();
#ifdef DEBUG
void AD9833_wave_MSB2();
#endif

/* frequency */
void AD9833_frequency_set_B28_by_reg(long data, unsigned char reg);
void AD9833_frequency_set(long data);

#ifdef USE_BOTH_REGISTERS
void AD9833_freg_select(unsigned char reg);
void AD9833_freg_toggle();
#endif

#ifdef DEBUG
void AD9833_frequency_set_MSB_by_reg(long data, unsigned char reg);
void AD9833_frequency_set_LSB_by_reg(long data, unsigned char reg);
void AD9833_frequency_set_u(long data);
void AD9833_frequency_set_l(long data);
#endif

/* phase */
#ifdef USE_PHASE
void AD9833_phase_set_by_reg(long data, unsigned char reg);
void AD9833_phase_set(long data);

#ifdef USE_BOTH_REGISTERS
void AD9833_preg_select(unsigned char reg);
void AD9833_preg_toggle();
#endif
#endif

/* power */
#ifdef DEBUG
void AD9833_power(unsigned char sleep1, unsigned char sleep12);
#endif

/* registers */
#ifdef USE_EEPROM
void AD9833_restore_status(unsigned int s);
long AD9833_get_freq(int reg);
int AD9833_get_phase(int reg);
#endif
#if defined(DEBUG) || defined(USE_EEPROM)
unsigned int AD9833_get_status();
#endif

#endif /* __AD9833_H_ */
