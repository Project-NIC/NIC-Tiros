/* NIC-Tiros — the record: Tiros's profile in MUF (RECORD.md), written through NIC-SDK's writer. */
#ifndef TIROS_RECORD_H
#define TIROS_RECORD_H
#include <stdint.h>
#include "muf.h"

enum rec_stream { REC_FAST, REC_SLOW, REC_GNSS, REC_EVENTS, REC_INFO, REC_CONST, REC_STREAMS };

struct rec_fast { int16_t acc[3], acc_hg[3], gyro[3], mag[3]; int32_t press_64pa; };
struct rec_slow { int16_t ptemp_cc, rh_c, air_t_cc; };
struct rec_gnss { int32_t lat_e7, lon_e7, h_mm; int16_t vel_cms[3]; uint8_t kind, sats, acc_dm, age_10ms; };
struct rec_info { int16_t imu_t_cc; uint16_t cell_mv; int8_t rssi; uint32_t utc, utc_step; int32_t utc_ns; };

int rec_begin(struct muf_writer *w, muf_write_fn write, void *ctx, uint16_t session, uint8_t unit,
	      const char *name, uint32_t fast_hz, uint64_t start_us, const char *table_csv,
	      const int32_t mag_off_nt[3], const int32_t mag_mat_e6[9]);
int rec_fast(struct muf_writer *w, const struct rec_fast *f);
int rec_slow(struct muf_writer *w, const struct rec_slow *s);
int rec_gnss(struct muf_writer *w, const struct rec_gnss *g);
int rec_event(struct muf_writer *w, uint32_t t_step, uint8_t event);
int rec_info(struct muf_writer *w, const struct rec_info *i);

#endif
