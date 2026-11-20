/* NIC-Tiros — the plain-C core tested on the host: the frames against RADIO.md's sizes and bytes,
 * the settings against SETTINGS.md, the machine against flights made up here, the boot log. */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "../common/airtime.h"
#include "../common/bootlog.h"
#include "../common/lastfix.h"
#include "../common/pieces.h"
#include "../common/crc32.h"
#include "../common/frames.h"
#include "../common/hamilton.h"
#include "../common/recfile.h"
#include "../common/listen.h"
#include "../common/seal.h"

int ccm_openssl(const uint8_t key[16], const uint8_t nonce[NONCE_LEN], const uint8_t *aad, size_t aad_len,
		const uint8_t *in, uint8_t *out, size_t len, uint8_t tag[4], int encrypt);
#include "../common/le.h"
#include "../common/settings.h"

static int failed, passed;
#define CHECK(c) do { if (c) passed++; else { failed++; printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); } } while (0)

static void test_crc(void)
{
	CHECK(crc32_update(0, "123456789", 9) == 0xCBF43926u); /* the standard check value */
}

static void test_frames(void)
{
	uint8_t b[256];
	struct head h = { FRAME_BEACON, 0xBEEF, 5, ADDR_ALL, 0x42 };
	struct beacon bc = { { 501234567, 143456789, 312, 3, 17 }, 4, 0, PHASE_LANDED, 3300, -97 };

	CHECK(frame_beacon_put(b, &h, &bc) == BEACON_LEN);
	CHECK(b[0] == 1 && get_u16(b + 1) == 0xBEEF && b[3] == 5 && b[4] == 255 && b[5] == 0x42);
	CHECK(get_i32(b + 6) == 501234567 && get_i32(b + 10) == 143456789 && get_i16(b + 14) == 312);
	CHECK(b[16] == 0x3F);          /* 3D, 15 or more satellites */
	CHECK(b[17] == 4 && b[19] == PHASE_LANDED);
	CHECK(b[20] == 165);           /* 3300 mV / 20 */
	CHECK((int8_t)b[21] == -97);

	struct live l = { 123456, 101325 * 64, 312, -720, PHASE_CLIMB, 0x01, 3300, 0, { 0 }, 0 };
	h.type = FRAME_LIVE;
	CHECK(frame_live_put(b, &h, &l) == LIVE_LEN);
	CHECK(get_u32(b + 6) == 123456 && get_i32(b + 10) == 101325 * 64 && b[21] == 0);
	l.has_gnss = 1;
	l.fix = (struct fix){ 1, 2, 300, 3, 9 };
	l.fix_step = 123400;
	CHECK(frame_live_put(b, &h, &l) == LIVE_GNSS_LEN);
	CHECK(b[21] == 1 && get_u32(b + 33) == 123400 && b[32] == 0x39);

	h.type = FRAME_COMMAND;
	CHECK(frame_command_put(b, &h, CMD_ARM, 0, 0) == COMMAND_LEN(0));
	CHECK(frame_command_put(b, &h, CMD_POWER, 1, 0xF6) == COMMAND_LEN(1) && b[6] == 5 && b[7] == 0xF6);

	struct answer a = { ANSWER_TABLE_RECEIVED, RESULT_DONE, -45, 0xA1B2C3D4 };
	h.type = FRAME_ANSWER;
	CHECK(frame_answer_put(b, &h, &a) == ANSWER_LEN && get_u32(b + 9) == 0xA1B2C3D4);
	CHECK(ANSWER_TABLE_RECEIVED != CMD_LOG_GET); /* the clash RADIO.md has, kept apart here */

	uint8_t text[200];
	memset(text, 'x', sizeof(text));
	h.type = FRAME_TABLE;
	CHECK(frame_piece_put(b, &h, 7, 0, 3, text, 200) == 216); /* calc/airtime.py's 216 B */
	CHECK(frame_piece_put(b, &h, 7, 0, 3, text, 201 > 200 ? 201 : 0) == 0);
	h.type = FRAME_LOG;
	uint8_t recs[192] = { 0 };
	CHECK(frame_piece_put(b, &h, 7, 0, 22, recs, 192) == 208);
}

static void test_settings(void)
{
	struct settings s;
	char buf[1024];
	int bad;

	for (int i = 0; i < SETTINGS_COUNT; i++) {
		CHECK(setting_defs[i].name != NULL);
	}
	settings_defaults(&s);
	CHECK(settings_get(&s, "REC_CLIMB") == 1000 && settings_get(&s, "LAUNCH_G") == 1.0f);
	CHECK((int)settings_get(&s, "LIVE_CHANNEL") == 1 && (int)settings_get(&s, "BEACON_CHANNEL") == 0);

	size_t n = settings_render(&s, buf, sizeof(buf));
	printf("  the default table: %zu B, CRC-32 %08X\n", n, settings_crc(&s));
	CHECK(n > 300 && n < 600);

	struct settings t = s;
	CHECK(settings_take(&t, buf, n, &bad) == TABLE_TAKEN && settings_crc(&t) == settings_crc(&s));

	const char *plane = "# a glider\nLAUNCH_G 0\nAPOGEE_M 0\nREC_CLIMB 200\nNAME glider1\r\n";
	CHECK(settings_take(&t, plane, strlen(plane), &bad) == TABLE_TAKEN);
	CHECK(settings_get(&t, "LAUNCH_G") == 0 && settings_get(&t, "REC_CLIMB") == 200);
	CHECK(strcmp(t.text[settings_index("NAME")], "glider1") == 0);

	struct settings before = t;
	const char *bad1 = "REC_CLIMB 300\nLAUNCH_G 0.05\n";   /* 0.05 g: neither 0 nor 0.1–20 */
	CHECK(settings_take(&t, bad1, strlen(bad1), &bad) == TABLE_OUT_OF_RANGE && bad == 2);
	CHECK(memcmp(&t, &before, sizeof(t)) == 0);              /* whole or not at all */
	const char *bad2 = "FOO 1\n";
	CHECK(settings_take(&t, bad2, strlen(bad2), &bad) == TABLE_UNKNOWN_NAME);
	const char *bad3 = "LIVE_GNSS_BY ms\nLIVE_GNSS_N 50\n";
	CHECK(settings_take(&t, bad3, strlen(bad3), &bad) == TABLE_OUT_OF_RANGE);
	const char *bad4 = "SET 1234\n";                         /* given on the cable only */
	CHECK(settings_take(&t, bad4, strlen(bad4), &bad) == TABLE_READ_ONLY);
	const char *bad5 = "CELL nimh\n";
	CHECK(settings_take(&t, bad5, strlen(bad5), &bad) == TABLE_OUT_OF_RANGE);
}

/* A made-up rocket: 3 s on the pad, 0.5 s of motor at 8 g, coasting to ~88 m, a chute at 3 m/s,
 * an impact of 6 g, then still. 1 kHz ticks. */
static float rocket_height(float t, float *acc)
{
	const float burn = 0.5f, a = 8.0f * 9.81f, g = 9.81f;
	float v_b = a * burn, h_b = 0.5f * a * burn * burn, t_top = burn + v_b / g;
	float h_top = h_b + v_b * v_b / (2 * g);

	if (t < 0) { *acc = 1.0f; return 0; }
	if (t < burn) { *acc = 9.0f; return 0.5f * a * t * t; }
	if (t < t_top) { *acc = 0.0f; float u = t - burn; return h_b + v_b * u - 0.5f * g * u * u; }
	float fall = h_top - 3.0f * (t - t_top - 1.0f);
	if (t < t_top + 1.0f) { *acc = 0.0f; return h_top - 0.5f * g * (t - t_top) * (t - t_top); }
	if (fall > 0) { *acc = 1.0f; return fall; }
	float since = -fall / 3.0f;
	*acc = since < 0.02f ? 6.0f : 1.0f;
	return 0;
}

/* A file per phase (recfile.h): at each change of state, counted as a flight goes. */
static void files_on(const struct settings *s, enum ham_state *prev, enum ham_state now,
		     uint32_t *hz, int *files)
{
	if (now == *prev)
		return;
	*prev = now;
	enum recfile_act a = recfile_change(*hz, recfile_rate(s, now), hz);
	if (a == RF_BEGIN || a == RF_NEXT)
		(*files)++;
}

/* The frames read back, as Ham reads them; the signature and the count (seal.h). */
static void test_decode_seal(void)
{
	const uint8_t key[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }, bad[16] = { 0 };
	uint8_t f[64], g[64];
	struct head h = { FRAME_BEACON, 4711, 1, ADDR_ALL, 0x05 };
	struct beacon b = { { 491951234, 166078901, 312, 3, 11 }, 4, 0, PHASE_LANDED, 4020, -71 }, b2;
	struct live l = { 1234, 101325 * 64, 950, -20, PHASE_CLIMB, 0x01, 3900, 1,
			  { 491951234, 166078901, 400, 3, 9 }, 1200 }, l2;
	struct answer a = { CMD_ARM, RESULT_DONE, -60, 0x9A41C2E0u }, a2;
	uint32_t whole;
	size_t n;

	n = frame_beacon_put(f, &h, &b);
	CHECK(frame_beacon_get(f, n, &b2) == 0 && b2.fix.lat_e7 == b.fix.lat_e7 && b2.fix.kind == 3 &&
	      b2.fix.satellites == 11 && b2.cell_mv == 4020 && b2.rssi_dbm == -71 && b2.phase == PHASE_LANDED);
	CHECK(frame_live_get(f, n, &l2) == -1);                                   /* not a live frame */
	/* signed and hidden; read only with the key and the whole count */
	CHECK(seal_frame(ccm_openssl, key, f, n, 0x305, 1) == 0);
	memcpy(g, f, n);
	CHECK(f[6] != (uint8_t)(b.fix.lat_e7 & 0xFF) || f[7] != (uint8_t)(b.fix.lat_e7 >> 8)); /* hidden */
	CHECK(seal_open(ccm_openssl, bad, g, n, 0x305, 1) == -1);                 /* another set's key */
	memcpy(g, f, n);
	CHECK(seal_open(ccm_openssl, key, g, n, 0x205, 1) == -1);                 /* the wrong count */
	memcpy(g, f, n);
	g[3] ^= 1;
	CHECK(seal_open(ccm_openssl, key, g, n, 0x305, 1) == -1);                 /* the head forged */
	CHECK(seal_open(ccm_openssl, key, f, n, 0x305, 1) == 0 && frame_beacon_get(f, n, &b2) == 0 &&
	      b2.fix.lon_e7 == b.fix.lon_e7 && b2.accuracy_m == 4);
	/* an answer, signed in the open */
	h.type = FRAME_ANSWER; h.target = ADDR_HAM_MASTER; h.count = 0x07;
	n = frame_answer_put(f, &h, &a);
	CHECK(seal_frame(ccm_openssl, key, f, n, 0x07, 0) == 0 && f[6] == CMD_ARM);
	CHECK(seal_open(ccm_openssl, key, f, n, 0x07, 0) == 0 && frame_answer_get(f, n, &a2) == 0 &&
	      a2.table_crc == 0x9A41C2E0u && a2.rssi_dbm == -60);
	f[7] = RESULT_REFUSED;
	CHECK(seal_open(ccm_openssl, key, f, n, 0x07, 0) == -1);                  /* the result forged */
	/* the live view, open, with and without its fix */
	h.type = FRAME_LIVE; h.target = ADDR_ALL;
	n = frame_live_put(f, &h, &l);
	CHECK(n == LIVE_GNSS_LEN && frame_live_get(f, n, &l2) == 0 && l2.has_gnss && l2.fix_step == 1200 &&
	      l2.pressure_64pa == 101325 * 64 && l2.roll_dps == -20 && l2.cell_mv == 3900);
	l.has_gnss = 0;
	n = frame_live_put(f, &h, &l);
	CHECK(n == LIVE_LEN && frame_live_get(f, n, &l2) == 0 && !l2.has_gnss);
	/* the count: the next above the last; a replay refused */
	CHECK(seal_count(0x1FE, 1, 0xFF, &whole) == 0 && whole == 0x1FF);
	CHECK(seal_count(0x1FF, 1, 0x00, &whole) == 0 && whole == 0x200);
	CHECK(seal_count(0x200, 1, 0x00, &whole) == 0 && whole == 0x300);       /* not the same: the next round */
	CHECK(seal_count(0, 0, 0x2A, &whole) == 0 && whole == 0x2A);
}

/* When a unit listens (listen.h): by phase, by its table. */
static void test_listen(void)
{
	struct settings s;
	struct radio_use u;
	int bad;

	settings_defaults(&s);
	u = listen_when(&s, PHASE_ON);
	CHECK(u.continuous && u.freq_hz == CHANNEL_B_HZ && u.sf == 7 && u.bw_khz == 250);
	u = listen_when(&s, PHASE_CLIMB);
	CHECK(!u.continuous && u.after_ms == 100 && u.freq_hz == CHANNEL_B_HZ && u.sf == 7);
	u = listen_when(&s, PHASE_LANDED);
	CHECK(!u.continuous && u.after_ms == 100 && u.freq_hz == CHANNEL_A_HZ && u.sf == 12 && u.bw_khz == 125);
	CHECK(!listen_held(PHASE_READY) && listen_held(PHASE_DESCENT) && listen_held(PHASE_LANDED));
	const char *far = "LIVE_CHANNEL A\nLIVE_MODE SF9/125\nBEACON_MODE SF10/125\nLISTEN_MS 300\n";
	CHECK(settings_take(&s, far, strlen(far), &bad) == TABLE_TAKEN);
	u = listen_when(&s, PHASE_DESCENT);
	CHECK(u.freq_hz == CHANNEL_A_HZ && u.sf == 9 && u.bw_khz == 125 && u.after_ms == 300);
	u = listen_when(&s, PHASE_LANDED);
	CHECK(u.sf == 10 && u.after_ms == 300);
}

static void test_recfile(void)
{
	char n[17];
	uint32_t hz;
	CHECK(recfile_name(n, 7, 1, 0) == 0 && !strcmp(n, "S00007T01.MUFT"));
	CHECK(recfile_name(n, 7, 1, 1) == 0 && !strcmp(n, "S00007T01B.MUFT"));
	CHECK(recfile_name(n, 65535, 99, 25) == 0 && !strcmp(n, "S65535T99Z.MUFT"));
	CHECK(recfile_name(n, 7, 1, 26) == -1);
	CHECK(recfile_change(0, 1000, &hz) == RF_BEGIN && hz == 1000);
	CHECK(recfile_change(1000, 1000, &hz) == RF_KEEP);
	CHECK(recfile_change(1000, 100, &hz) == RF_NEXT && hz == 100);
	CHECK(recfile_change(100, 0, &hz) == RF_END && hz == 0);
	CHECK(recfile_change(0, 0, &hz) == RF_KEEP);
}

static void test_rocket(void)
{
	enum ham_state prev = ST_ON;
	uint32_t hz = 0;
	int files = 0;
	struct settings s;
	struct hamilton h;
	struct ham_out o;
	uint32_t ev_t[200] = { 0 };
	int seen[200] = { 0 };

	settings_defaults(&s);
	ham_init(&h, 0);
	for (uint32_t ms = 0; ms < 80000; ms++) {
		float t = ms / 1000.0f - 3.0f, acc;
		float height = rocket_height(t, &acc);
		struct ham_input in = { ms, acc, height, ms == 10 ? CMD_ARM : 0, 0, 0 };
		ham_tick(&h, &s, &in, &o);
		files_on(&s, &prev, h.state, &hz, &files);
		if (o.event && !seen[o.event]) {
			seen[o.event] = 1;
			ev_t[o.event] = o.event_t_ms;
			printf("  rocket: event %d at %.3f s (the state %d)\n", o.event, o.event_t_ms / 1000.0, h.state);
		}
	}
	CHECK(seen[EV_ARMED] && seen[EV_LAUNCHED] && seen[EV_APOGEE] && seen[EV_LANDED]);
	CHECK(ev_t[EV_LAUNCHED] == 3000);                     /* back-dated to the departure */
	CHECK(h.state == ST_LANDED);
	float acc;
	(void)rocket_height(0, &acc);
	CHECK(ev_t[EV_APOGEE] > 7000 && ev_t[EV_APOGEE] < 8000);    /* the top ≈ 0.5 + 4 s after the start */
	CHECK(ev_t[EV_LANDED] > 30000 && ev_t[EV_LANDED] < 40000);  /* at the impact's spike, back-dated */
	CHECK(files == 2 && hz == 0);   /* the climb at 1000 Hz, the descent at 100 Hz, closed on landing */
}


/* A made-up plane, the user's own table: a take-off run at 0.4 g, climbing to 60 m, minutes of a
 * shaking flight, a landing roll, the motor stopped. No top looked for, no spike. */
static void test_plane(void)
{
	enum ham_state prev = ST_ON;
	uint32_t hz = 0;
	int files = 0;
	struct settings s;
	struct hamilton h;
	struct ham_out o;
	int bad, seen[200] = { 0 };
	const char *tab = "LAUNCH_G 0.3\nLAUNCH_MS 1000\nLAUNCH_UP_M 5\nLAUNCH_UP_S 20\nAPOGEE_M 0\nLAND_G 0\nLAND_STILL_S 10\n";

	settings_defaults(&s);
	CHECK(settings_take(&s, tab, strlen(tab), &bad) == TABLE_TAKEN);
	ham_init(&h, 0);
	for (uint32_t ms = 0; ms < 400000; ms += 10) {
		float t = ms / 1000.0f, acc = 1.0f, hgt = 0;
		if (t >= 5 && t < 10) { acc = 1.4f; }                       /* the take-off run */
		if (t >= 8 && t < 300) { hgt = t < 20 ? (t - 8) * 5 : 60; acc = 1.0f + 0.2f * sinf(t * 50); }
		if (t >= 300 && t < 320) { hgt = 0; acc = 1.0f + 0.15f * sinf(t * 80); } /* rolling, the motor on */
		struct ham_input in = { ms, acc, hgt, ms == 100 ? CMD_ARM : 0, 0, 0 };
		ham_tick(&h, &s, &in, &o);
		files_on(&s, &prev, h.state, &hz, &files);
		if (o.event) {
			seen[o.event] = 1;
			printf("  plane: event %d at %.2f s\n", o.event, o.event_t_ms / 1000.0);
		}
	}
	CHECK(seen[EV_LAUNCHED] && !seen[EV_APOGEE] && seen[EV_LANDED] && h.state == ST_LANDED);
	CHECK(files == 1);              /* no top looked for: one file from the launch to the end */
}

static void test_bump(void)
{
	struct settings s;
	struct hamilton h;
	struct ham_out o;
	int launched = 0;

	settings_defaults(&s);
	ham_init(&h, 0);
	for (uint32_t ms = 0; ms < 30000; ms++) {
		float acc = (ms > 5000 && ms < 5300) ? 2.5f : 1.0f;  /* knocked on the pad, no height gained */
		struct ham_input in = { ms, acc, 0.0f, ms == 10 ? CMD_ARM : 0, 0, 0 };
		ham_tick(&h, &s, &in, &o);
		launched |= o.event == EV_LAUNCHED;
	}
	CHECK(!launched && h.state == ST_READY);
}

static void test_all_off(void)
{
	struct settings s;
	struct hamilton h;
	struct ham_out o;
	int bad;
	const char *off = "LAUNCH_G 0\nAPOGEE_M 0\nLAND_G 0\nLAND_STILL_S 0\n";

	settings_defaults(&s);
	CHECK(settings_take(&s, off, strlen(off), &bad) == TABLE_TAKEN);
	ham_init(&h, 0);
	uint8_t cmd[] = { [10] = CMD_ARM, [1000] = CMD_START, [5000] = CMD_STOP };
	int events = 0;
	for (uint32_t ms = 0; ms < 6000; ms++) {
		struct ham_input in = { ms, ms > 1000 && ms < 2000 ? 5.0f : 1.0f, 0.0f,
					ms < sizeof(cmd) ? cmd[ms] : 0, 0, 0 };
		ham_tick(&h, &s, &in, &o);
		events += o.event != EV_NONE;
	}
	CHECK(events == 3 && h.state == ST_LANDED); /* armed, launched by START, landed by STOP */
}

static void test_idle_off(void)
{
	struct settings s;
	struct hamilton h;
	struct ham_out o;

	settings_defaults(&s);
	ham_init(&h, 0);
	for (uint32_t ms = 0; ms <= 10u * 60000u; ms += 100) {
		struct ham_input in = { ms, 1.0f, NAN, 0, 0, 0 };
		ham_tick(&h, &s, &in, &o);
	}
	CHECK(h.state == ST_OFF);
}


static void test_airtime(void)
{
	/* calc/out/airtime.md's figures, to the tenth of a ms */
	CHECK((lora_toa_us(26, 12, 125) + 50) / 100 == 16466);   /* the beacon, 1646.6 ms */
	CHECK((lora_toa_us(22, 7, 250) + 50) / 100 == 283);      /* the live view, 28.3 ms */
	CHECK((lora_toa_us(37, 7, 250) + 50) / 100 == 411);      /* with the GNSS, 41.1 ms */
	CHECK((lora_toa_us(216, 7, 250) + 50) / 100 == 1716);    /* a table's piece, 171.6 ms */

	static struct duty d;
	uint32_t b = lora_toa_us(26, 12, 125), sent = 0, refused_at = 0;
	duty_init(&d);
	for (uint32_t t = 0; t < 3600000u; t += 5000) {   /* a beacon every 5 s for an hour */
		if (duty_may_send(&d, t, b, 0)) {
			duty_sent(&d, t, b);
			sent++;
		} else if (!refused_at) {
			refused_at = t;
		}
	}
	printf("  the hour's account: %u beacons sent, the first refused at %.0f s\n", sent, refused_at / 1000.0);
	CHECK(sent * b <= DUTY_BUDGET_US - DUTY_RESERVE_US + b);
	CHECK(refused_at > 600000);                       /* ten minutes of fast beacons fit */
	CHECK(duty_may_send(&d, 3600000u, lora_toa_us(22, 7, 250), 1)); /* a flight never refused */
	CHECK(duty_used_us(&d, 3600000u + 3600000u) == 0); /* an hour later all has left the window */
}

static void test_lastfix(void)
{
	struct lastfix l = { 0 };
	struct beacon b;
	struct fix good = { 501000000, 140000000, 300, 3, 12 }, bad = { 502000000, 141000000, 280, 3, 4 };

	lastfix_beacon(&l, &b, 0);
	CHECK(b.age_min == 255 && b.accuracy_m == 255);
	lastfix_offer(&l, &good, 3.2f, 10, 1000);
	lastfix_offer(&l, &bad, 45.0f, 10, 61000);         /* in the grass: worse, not taken */
	lastfix_beacon(&l, &b, 1000 + 7 * 60000);
	CHECK(b.fix.lat_e7 == 501000000 && b.accuracy_m == 3 && b.age_min == 7);
}

static void test_pieces(void)
{
	struct settings s, t;
	static struct pieces p;
	char text[1024], joined[3200];
	int bad;

	settings_defaults(&s);
	const char *mine = "REC_CLIMB 800\nNAME able7\n";
	CHECK(settings_take(&s, mine, strlen(mine), &bad) == TABLE_TAKEN);
	size_t n = settings_render(&s, text, sizeof(text));
	uint32_t crc = crc32_update(0, text, n);
	uint8_t count = (uint8_t)((n + 199) / 200);
	pieces_reset(&p);
	enum pieces_state st = PIECES_WAITING;
	for (int k = count - 1; k >= 0; k--) {             /* out of order */
		size_t len = (size_t)k * 200 + 200 > n ? n - (size_t)k * 200 : 200;
		st = pieces_add(&p, crc, (uint8_t)k, count, text + k * 200, len);
		CHECK(st == (k ? PIECES_WAITING : PIECES_WHOLE));
	}
	size_t m = pieces_join(&p, joined, sizeof(joined));
	settings_defaults(&t);
	CHECK(st == PIECES_WHOLE && m == n && settings_take(&t, joined, m, &bad) == TABLE_TAKEN);
	CHECK(settings_crc(&t) == settings_crc(&s));
	pieces_reset(&p);
	CHECK(pieces_add(&p, crc ^ 1, 0, 1, text, 200) == PIECES_BAD_CRC);
	CHECK(pieces_add(&p, crc, 3, 2, text, 10) == PIECES_REFUSED);
}

static void test_bootlog(void)
{
	struct boot_record r = { 1234, 0x4, 0xC2156C34, 0, 3310, 21, 12, { 0 }, 37, END_WATCHDOG, 255, 0, 3600,
				 PHASE_READY };
	uint8_t b[BOOT_RECORD_LEN];
	struct boot_record q;

	r.step[STEP_CARD] = boot_step_word(STEP_TIMEOUT, 9000);
	boot_record_put(b, &r);
	CHECK(boot_record_get(b, &q) == 0 && q.number == 1234 && q.prev_end == END_WATCHDOG);
	CHECK(q.step[STEP_CARD] >> 13 == STEP_TIMEOUT && (q.step[STEP_CARD] & 0x1FFF) == 8191);
	CHECK(b[57] == 0 && b[58] == 0 && b[59] == 0);
	b[10] ^= 1;
	CHECK(boot_record_get(b, &q) == -1);
	CHECK(boot_slot(64) == 0 && boot_slot(130) == 2);
}

int main(void)
{
	test_crc();
	test_frames();
	test_settings();
	test_decode_seal();
	test_listen();
	test_recfile();
	test_rocket();
	test_plane();
	test_bump();
	test_all_off();
	test_idle_off();
	test_airtime();
	test_lastfix();
	test_pieces();
	test_bootlog();
	printf("%d passed, %d failed\n", passed, failed);
	return failed != 0;
}
