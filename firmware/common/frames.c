/* NIC-Tiros — the radio's frames, as RADIO.md sets them out byte for byte. */
#include <string.h>
#include "frames.h"
#include "le.h"

static uint8_t cell_step(uint16_t mv)
{
	uint32_t s = (mv + 10u) / 20u; /* 20 mV a step, rounded */

	return s > 255u ? 255u : (uint8_t)s;
}

static uint8_t fix_byte(const struct fix *f)
{
	uint8_t sats = f->satellites > 15 ? 15 : f->satellites;

	return (uint8_t)((f->kind & 0x0F) << 4 | sats);
}

size_t frame_head_put(uint8_t *out, const struct head *h)
{
	put_u8(out + 0, h->type);
	put_u16(out + 1, h->set);
	put_u8(out + 3, h->sender);
	put_u8(out + 4, h->target);
	put_u8(out + 5, h->count);
	return HEAD_LEN;
}

int frame_head_get(const uint8_t *in, size_t len, struct head *h)
{
	if (len < HEAD_LEN) {
		return -1;
	}
	h->type = in[0];
	h->set = get_u16(in + 1);
	h->sender = in[3];
	h->target = in[4];
	h->count = in[5];
	return 0;
}

size_t frame_beacon_put(uint8_t *out, const struct head *h, const struct beacon *b)
{
	frame_head_put(out, h);
	put_i32(out + 6, b->fix.lat_e7);
	put_i32(out + 10, b->fix.lon_e7);
	put_i16(out + 14, b->fix.height_m);
	out[16] = fix_byte(&b->fix);
	out[17] = b->accuracy_m;
	out[18] = b->age_min;
	out[19] = b->phase;
	out[20] = cell_step(b->cell_mv);
	out[21] = (uint8_t)b->rssi_dbm;
	memset(out + 22, 0, TAG_LEN);
	return BEACON_LEN;
}

size_t frame_live_put(uint8_t *out, const struct head *h, const struct live *l)
{
	frame_head_put(out, h);
	put_u32(out + 6, l->step);
	put_i32(out + 10, l->pressure_64pa);
	put_i16(out + 14, l->accel_cg);
	put_i16(out + 16, l->roll_dps);
	out[18] = l->phase;
	out[19] = l->events;
	out[20] = cell_step(l->cell_mv);
	out[21] = l->has_gnss ? 0x01 : 0x00;
	if (!l->has_gnss) {
		return LIVE_LEN;
	}
	put_i32(out + 22, l->fix.lat_e7);
	put_i32(out + 26, l->fix.lon_e7);
	put_i16(out + 30, l->fix.height_m);
	out[32] = fix_byte(&l->fix);
	put_u32(out + 33, l->fix_step);
	return LIVE_GNSS_LEN;
}

size_t frame_command_put(uint8_t *out, const struct head *h, uint8_t command, int has_arg, uint8_t arg)
{
	size_t n = HEAD_LEN;

	frame_head_put(out, h);
	out[n++] = command;
	if (has_arg) {
		out[n++] = arg;
	}
	memset(out + n, 0, TAG_LEN);
	return n + TAG_LEN;
}

size_t frame_answer_put(uint8_t *out, const struct head *h, const struct answer *a)
{
	frame_head_put(out, h);
	out[6] = a->command;
	out[7] = a->result;
	out[8] = (uint8_t)a->rssi_dbm;
	put_u32(out + 9, a->table_crc);
	memset(out + 13, 0, TAG_LEN);
	return ANSWER_LEN;
}

size_t frame_piece_put(uint8_t *out, const struct head *h, uint32_t whole_crc, uint8_t piece,
		       uint8_t pieces, const uint8_t *payload, size_t len)
{
	size_t max = h->type == FRAME_TABLE ? TABLE_PIECE_TEXT_MAX : LOG_PIECE_RECORDS_MAX * LOG_RECORD_LEN;

	if (len > max) {
		return 0;
	}
	frame_head_put(out, h);
	put_u32(out + 6, whole_crc);
	out[10] = piece;
	out[11] = pieces;
	memcpy(out + PIECE_HEAD_LEN, payload, len);
	memset(out + PIECE_HEAD_LEN + len, 0, TAG_LEN);
	return PIECE_HEAD_LEN + len + TAG_LEN;
}
