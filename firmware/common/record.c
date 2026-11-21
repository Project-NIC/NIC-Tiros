/* NIC-Tiros — the record, as RECORD.md sets out Tiros's profile in MUF. */
#include <string.h>
#include "le.h"
#include "record.h"

#define G(st, fi, n, t, e, m, nm, un) { st, fi, n, t, 0, e, m, 0, nm, un, 0, 0 }

static const struct muf_group groups[] = {
	G(0x10, 0, 3, 'h', -6, 488, "ACC", "g"),
	G(0x10, 3, 3, 'h', -6, 10417, "ACC_HG", "g"),
	G(0x10, 6, 3, 'h', -3, 140, "GYRO", "dps"),    /* the LSM6DSV320X at ±4000 dps: 140 mdps */
	G(0x10, 9, 3, 'h', -12, 24414, "MAG", "T"), /* the MMC5983MA in 16 bits: 1/4096 G */
	G(0x10, 12, 1, 'i', -6, 15625, "PRESS", "Pa"),
	G(0x11, 0, 1, 'h', -2, 1, "PTEMP", "degC"),
	G(0x11, 1, 1, 'h', -2, 1, "RH", "%"),
	G(0x11, 2, 1, 'h', -2, 1, "AIR_T", "degC"),
	G(0x12, 0, 2, 'i', -7, 1, "GNSS_LL", "deg"),
	G(0x12, 2, 1, 'i', -3, 1, "GNSS_H", "m"),
	G(0x12, 3, 3, 'h', -2, 1, "GNSS_VEL", "m/s"),
	G(0x12, 6, 4, 'B', 0, 1, "GNSS_FIX", ""),
	G(0x13, 0, 1, 'I', 0, 1, "T_STEP", ""),
	G(0x13, 1, 1, 'B', 0, 1, "EVENT", ""),
	G(0x05, 0, 1, 'h', -2, 1, "IMU_T", "degC"),
	G(0x05, 1, 1, 'H', -3, 1, "CELL", "V"),
	G(0x05, 2, 1, 'b', 0, 1, "RSSI", "dBm"),
	G(0x05, 3, 1, 'I', 0, 1, "TIME_UTC", "s"),
	G(0x05, 4, 1, 'I', 0, 1, "TIME_STP", ""),
	G(0x05, 5, 1, 'i', -9, 1, "TIME_NS", "s"),
	G(0x06, 0, 3, 'i', -9, 1, "MAG_OFF", "T"),
	G(0x06, 3, 9, 'i', -6, 1, "MAG_MAT", ""),
};

int rec_begin(struct muf_writer *w, muf_write_fn write, void *ctx, uint16_t session, uint8_t unit,
	      const char *name, uint32_t fast_hz, uint64_t start_us, const char *table_csv,
	      const int32_t mag_off_nt[3], const int32_t mag_mat_e6[9])
{
	const struct muf_stream streams[REC_STREAMS] = {
		{ 0x10, fast_hz * 1000u, 17, 28, 0 }, { 0x11, 10000, 80, 6, 0 }, { 0x12, 10000, 21, 22, 0 },
		{ 0x13, 0, 96, 5, 1 }, { 0x05, 1000, 28, 17, 0 }, { 0x06, 0, 1, 48, 0 },
	};
	struct muf_head h = { 'T', session, unit, 0, { 0, 7, 0 }, 1, start_us, 0, "" };
	uint8_t c[48];

	strncpy(h.name, name, sizeof(h.name) - 1);
	for (int k = 0; k < 3; k++) {
		put_i32(c + 4 * k, mag_off_nt[k]);
	}
	for (int k = 0; k < 9; k++) {
		put_i32(c + 12 + 4 * k, mag_mat_e6[k]);
	}
	return muf_begin(w, write, ctx, &h, streams, REC_STREAMS, groups,
			 sizeof(groups) / sizeof(groups[0]), table_csv, c);
}

int rec_fast(struct muf_writer *w, const struct rec_fast *f)
{
	uint8_t s[28];

	for (int k = 0; k < 3; k++) {
		put_i16(s + 2 * k, f->acc[k]);
		put_i16(s + 6 + 2 * k, f->acc_hg[k]);
		put_i16(s + 12 + 2 * k, f->gyro[k]);
		put_i16(s + 18 + 2 * k, f->mag[k]);
	}
	put_i32(s + 24, f->press_64pa);
	return muf_step(w, REC_FAST, s);
}

int rec_slow(struct muf_writer *w, const struct rec_slow *v)
{
	uint8_t s[6];

	put_i16(s, v->ptemp_cc);
	put_i16(s + 2, v->rh_c);
	put_i16(s + 4, v->air_t_cc);
	return muf_step(w, REC_SLOW, s);
}

int rec_gnss(struct muf_writer *w, const struct rec_gnss *g)
{
	uint8_t s[22];

	put_i32(s, g->lat_e7);
	put_i32(s + 4, g->lon_e7);
	put_i32(s + 8, g->h_mm);
	for (int k = 0; k < 3; k++) {
		put_i16(s + 12 + 2 * k, g->vel_cms[k]);
	}
	s[18] = g->kind;
	s[19] = g->sats;
	s[20] = g->acc_dm;
	s[21] = g->age_10ms;
	return muf_step(w, REC_GNSS, s);
}

int rec_event(struct muf_writer *w, uint32_t t_step, uint8_t event)
{
	uint8_t s[5];

	put_u32(s, t_step);
	s[4] = event;
	return muf_step(w, REC_EVENTS, s);
}

int rec_info(struct muf_writer *w, const struct rec_info *i)
{
	uint8_t s[17];

	put_i16(s, i->imu_t_cc);
	put_u16(s + 2, i->cell_mv);
	s[4] = (uint8_t)i->rssi;
	put_u32(s + 5, i->utc);
	put_u32(s + 9, i->utc_step);
	put_i32(s + 13, i->utc_ns);
	return muf_step(w, REC_INFO, s);
}
