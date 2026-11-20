/* NIC-Tiros — a made-up rocket flown through Hamilton's machine and written as Tiros's record:
 * fly FILE. The flight of tests/test_host.c; read back by tests/test_record.py with NIC-SDK's reader. */
#include <math.h>
#include <stdio.h>
#include "../common/frames.h"
#include "../common/hamilton.h"
#include "../common/record.h"
#include "../common/settings.h"

static int to_file(void *ctx, const uint8_t b[MUF_BLOCK]) { return fwrite(b, MUF_BLOCK, 1, ctx) == 1 ? 0 : -1; }

static float rocket(float t, float *acc)
{
	const float burn = 0.5f, a = 8.0f * 9.81f, g = 9.81f;
	float v_b = a * burn, h_b = 0.5f * a * burn * burn, t_top = burn + v_b / g, h_top = h_b + v_b * v_b / (2 * g);

	if (t < 0) { *acc = 1.0f; return 0; }
	if (t < burn) { *acc = 9.0f; return 0.5f * a * t * t; }
	if (t < t_top) { *acc = 0.0f; float u = t - burn; return h_b + v_b * u - 0.5f * g * u * u; }
	if (t < t_top + 1.0f) { *acc = 0.0f; return h_top - 0.5f * g * (t - t_top) * (t - t_top); }
	float fall = h_top - 4.9f - 3.0f * (t - t_top - 1.0f);
	if (fall > 0) { *acc = 1.0f; return fall; }
	*acc = -fall / 3.0f < 0.02f ? 6.0f : 1.0f;
	return 0;
}

static int16_t clamp16(float x) { return x > 32767 ? 32767 : x < -32768 ? -32768 : (int16_t)lrintf(x); }

int main(int argc, char **argv)
{
	static struct muf_writer w;
	struct settings s;
	struct hamilton h;
	struct ham_out o;
	const int32_t off[3] = { 1200, -800, 300 }, mat[9] = { 1000000, 0, 0, 0, 1000000, 0, 0, 0, 1000000 };
	FILE *f = fopen(argc > 1 ? argv[1] : "fly.muft", "wb");

	settings_defaults(&s);
	ham_init(&h, 0);
	if (!f || rec_begin(&w, to_file, f, 42, 1, "baker1", 1000, 1763500000000000ull,
			    "kind;name;motor;mass\nrocket;test;C6-3;85\n", off, mat)) {
		return 1;
	}
	for (uint32_t ms = 0; ms < 80000; ms++) {
		float t = ms / 1000.0f - 3.0f, acc;
		float hgt = rocket(t, &acc);
		float p = 101325.0f * powf(1.0f - 2.25577e-5f * hgt, 5.25588f);
		struct ham_input in = { ms, acc, hgt, ms == 10 ? CMD_ARM : 0, 0, 0 };

		ham_tick(&h, &s, &in, &o);
		if (o.event) {
			rec_event(&w, o.event_t_ms, (uint8_t)o.event); /* at 1 kHz a step is a ms */
		}
		struct rec_fast fs = { { 0, 0, clamp16(acc / 488e-6f) }, { 0, 0, clamp16(acc / 10417e-6f) },
				       { 0, 0, 0 }, { 800, -200, 1600 }, (int32_t)lrintf(p * 64.0f) };
		rec_fast(&w, &fs);
		if (ms % 100 == 0) {
			struct rec_slow sl = { 1850, 6200, 1800 };
			rec_slow(&w, &sl);
			float drift = t > 4.5f ? (t - 4.5f) * 2.0f : 0;     /* the wind: 2 m/s east under the chute */
			struct rec_gnss g = { 501000000, 144000000 + (int32_t)lrintf(drift / 0.00715f), /* 10⁻⁷ deg of longitude is 7.15 mm at 50° */
					      (int32_t)lrintf((300.0f + hgt) * 1000.0f), { 0, 200, 0 }, 3, 14, 18, 5 };
			rec_gnss(&w, &g);
		}
		if (ms % 1000 == 0) {
			struct rec_info i = { 2400, 3300, -80, 1763500000u + ms / 1000, ms, 0 };
			rec_info(&w, &i);
		}
	}
	muf_end(&w, 16);
	fclose(f);
	return w.error;
}
