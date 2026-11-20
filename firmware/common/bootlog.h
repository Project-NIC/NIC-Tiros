/* NIC-Tiros — the boot log's record, 64 B, as BOOTLOG.md sets it out; 64 of them in a ring in RRAM. */
#ifndef TIROS_BOOTLOG_H
#define TIROS_BOOTLOG_H
#include <stdint.h>

#define BOOT_RECORD_LEN 64u
#define BOOT_STEPS 12u
#define BOOT_RING 64u

enum boot_step { STEP_CLOCKS, STEP_POWER, STEP_SETTINGS, STEP_CARD, STEP_FS, STEP_MOTION, STEP_BARO,
		 STEP_MAG, STEP_HUMIDITY, STEP_GNSS, STEP_RADIO, STEP_USB };
enum step_result { STEP_GOOD = 0, STEP_ABSENT = 1, STEP_WRONG = 2, STEP_TIMEOUT = 3, STEP_ERROR = 4,
		   STEP_NOT_REACHED = 7 };
enum run_end { END_UNKNOWN = 0, END_FAULT = 1, END_WATCHDOG = 2, END_ASSERT = 3, END_ORDERLY = 4 };

struct boot_record {
	uint32_t number, reset_cause, firmware, utc;
	uint16_t cell_mv;
	int8_t temp_c;
	uint8_t steps_done;
	uint16_t step[BOOT_STEPS];   /* top 3 bits the result, low 13 the time in ms */
	uint16_t total_10ms;
	uint8_t prev_end, prev_fatal;
	uint32_t prev_pc, prev_uptime_s;
	uint8_t prev_phase;
};

uint16_t boot_step_word(enum step_result r, uint32_t ms);
void boot_record_put(uint8_t out[BOOT_RECORD_LEN], const struct boot_record *r);
/* 0 good, −1 the CRC fails. */
int boot_record_get(const uint8_t in[BOOT_RECORD_LEN], struct boot_record *r);
/* The ring: the slot a start number goes to, and the newest of 64 slots (the highest number). */
static inline unsigned boot_slot(uint32_t number) { return number % BOOT_RING; }

#endif
