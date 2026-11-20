/* NIC-Tiros — the boot log's record, as BOOTLOG.md sets it out byte for byte. */
#include <string.h>
#include "bootlog.h"
#include "crc32.h"
#include "le.h"

uint16_t boot_step_word(enum step_result r, uint32_t ms)
{
	return (uint16_t)(((unsigned)r & 7u) << 13 | (ms > 8191u ? 8191u : ms));
}

void boot_record_put(uint8_t out[BOOT_RECORD_LEN], const struct boot_record *r)
{
	memset(out, 0, BOOT_RECORD_LEN);
	put_u32(out + 0, r->number);
	put_u32(out + 4, r->reset_cause);
	put_u32(out + 8, r->firmware);
	put_u32(out + 12, r->utc);
	put_u16(out + 16, r->cell_mv);
	out[18] = (uint8_t)r->temp_c;
	out[19] = r->steps_done;
	for (unsigned i = 0; i < BOOT_STEPS; i++) {
		put_u16(out + 20 + 2 * i, r->step[i]);
	}
	put_u16(out + 44, r->total_10ms);
	out[46] = r->prev_end;
	out[47] = r->prev_fatal;
	put_u32(out + 48, r->prev_pc);
	put_u32(out + 52, r->prev_uptime_s);
	out[56] = r->prev_phase;
	/* 57–59 free, zeros */
	put_u32(out + 60, crc32_update(0, out, 60));
}

int boot_record_get(const uint8_t in[BOOT_RECORD_LEN], struct boot_record *r)
{
	if (crc32_update(0, in, 60) != get_u32(in + 60)) {
		return -1;
	}
	r->number = get_u32(in + 0);
	r->reset_cause = get_u32(in + 4);
	r->firmware = get_u32(in + 8);
	r->utc = get_u32(in + 12);
	r->cell_mv = get_u16(in + 16);
	r->temp_c = (int8_t)in[18];
	r->steps_done = in[19];
	for (unsigned i = 0; i < BOOT_STEPS; i++) {
		r->step[i] = get_u16(in + 20 + 2 * i);
	}
	r->total_10ms = get_u16(in + 44);
	r->prev_end = in[46];
	r->prev_fatal = in[47];
	r->prev_pc = get_u32(in + 48);
	r->prev_uptime_s = get_u32(in + 52);
	r->prev_phase = in[56];
	return 0;
}
