/* NIC-Tiros — the settings table (SETTINGS.md): definitions, defaults, ranges, the text form. */
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crc32.h"
#include "settings.h"

static const char *const W_ARM[] = { "radio", "auto", NULL };
static const char *const W_HEIGHT[] = { "baro", "gnss", "either", NULL };
static const char *const W_ONOFF[] = { "on", "off", NULL };
static const char *const W_AB[] = { "A", "B", NULL };
static const char *const W_LIVEMODE[] = { "SF7/250", "SF7/125", "SF8/125", "SF9/125", NULL };
static const char *const W_GNSSBY[] = { "off", "frames", "ms", NULL };
static const char *const W_BEACONMODE[] = { "SF9/125", "SF10/125", "SF11/125", "SF12/125", NULL };
static const char *const W_HEATER[] = { "auto", "off", NULL };
static const char *const W_CELL[] = { "lfp", "lipo", NULL };

#define N(name, d, lo, hi, z) { name, KIND_NUMBER, d, lo, hi, z, 0, NULL }
#define W(name, d, w) { name, KIND_WORD, d, 0, 0, 0, 0, w }

/* In SETTINGS.md's order. */
const struct setting_def setting_defs[SETTINGS_COUNT] = {
	N("REC_READY", 0, 1, 4000, 1),
	N("REC_CLIMB", 1000, 1, 4000, 1),
	N("REC_DESC", 100, 1, 4000, 1),
	N("REC_LANDED", 0, 1, 100, 1),
	N("PRE_S", 3, 0, 10, 0),
	N("SLOW_HZ", 10, 1, 100, 0),
	N("GNSS_HZ", 10, 1, 20, 0),
	W("ARM_BY", 0, W_ARM),
	N("LAUNCH_G", 1.0f, 0.1f, 20, 1),
	N("LAUNCH_MS", 50, 10, 2000, 0),
	N("LAUNCH_UP_M", 5, 1, 100, 1),
	N("LAUNCH_UP_S", 10, 1, 120, 0),
	N("APOGEE_M", 3, 1, 100, 1),
	N("LAND_G", 3.0f, 1.1f, 100, 1),
	N("LAND_STILL_S", 10, 2, 600, 1),
	W("HEIGHT_BY", 0, W_HEIGHT),
	N("IDLE_OFF_MIN", 10, 1, 1440, 1),
	N("SESSION_MAX_S", 0, 60, 86400, 1),
	W("LIVE", 0, W_ONOFF),
	W("LIVE_CHANNEL", 1, W_AB),
	W("LINK_CHANNEL", 1, W_AB),
	W("BEACON_CHANNEL", 0, W_AB),
	N("LIVE_HZ", 20, 1, 30, 0),
	W("LIVE_MODE", 0, W_LIVEMODE),
	W("LIVE_GNSS_BY", 1, W_GNSSBY),
	N("LIVE_GNSS_N", 10, 1, 10000, 0), /* 1–255 frames or 100–10 000 ms, checked by its BY */
	N("LIVE_READY_HZ", 0, 0, 10, 0),
	N("BEACON_EVERY_S", 60, 5, 3600, 0),
	N("BEACON_FAST_MIN", 10, 0, 120, 0),
	W("BEACON_MODE", 3, W_BEACONMODE),
	N("BEACON_ACC_M", 10, 2, 100, 0),
	N("TX_DBM", 14, 0, 22, 0),        /* 22 only on a fitting cell — the firmware caps it */
	N("LISTEN_MS", 100, 0, 1000, 0),
	N("UNIT", 1, 1, 63, 0),
	{ "SET", KIND_NUMBER, 0, 1, 65535, 1, 1, NULL },
	{ "NAME", KIND_TEXT, 0, 0, 0, 0, 0, NULL },
	N("TABLE_RSSI", -40, -90, -20, 0),
	W("BEACON_SECRET", 0, W_ONOFF),
	W("GNSS_OFF", 0, W_ONOFF),
	W("BUZZER", 0, W_ONOFF),
	W("HEATER", 0, W_HEATER),
	W("CELL", 0, W_CELL),
};

int settings_index(const char *name)
{
	for (int i = 0; i < SETTINGS_COUNT; i++) {
		if (strcmp(setting_defs[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
}

void settings_defaults(struct settings *s)
{
	memset(s, 0, sizeof(*s));
	for (int i = 0; i < SETTINGS_COUNT; i++) {
		s->v[i] = setting_defs[i].def;
	}
}

float settings_get(const struct settings *s, const char *name)
{
	int i = settings_index(name);

	return i < 0 ? NAN : s->v[i];
}

static int word_index(const char *const *w, const char *word)
{
	for (int k = 0; w[k]; k++) {
		if (strcmp(w[k], word) == 0) {
			return k;
		}
	}
	return -1;
}

static int in_range(const struct setting_def *d, float x)
{
	if (d->zero_ok && x == 0.0f) {
		return 1;
	}
	return x >= d->min && x <= d->max;
}

enum table_result settings_take(struct settings *s, const char *text, size_t len, int *bad_line)
{
	struct settings t = *s;
	size_t i = 0;
	int line = 0;

	while (i < len) {
		char buf[64], name[32], value[32];
		size_t n = 0;

		line++;
		while (i < len && text[i] != '\n' && n < sizeof(buf) - 1) {
			buf[n++] = text[i++];
		}
		while (i < len && text[i] != '\n') {
			i++; /* a line too long is cut, then refused below */
		}
		i++;
		buf[n] = '\0';
		if (n && buf[n - 1] == '\r') {
			buf[--n] = '\0';
		}
		char *p = buf;
		while (isspace((unsigned char)*p)) {
			p++;
		}
		if (*p == '\0' || *p == '#') {
			continue;
		}
		if (sscanf(p, "%31s %31s", name, value) != 2) {
			*bad_line = line;
			return TABLE_BAD_LINE;
		}
		int k = settings_index(name);
		if (k < 0) {
			*bad_line = line;
			return TABLE_UNKNOWN_NAME;
		}
		const struct setting_def *d = &setting_defs[k];
		if (d->kind == KIND_NUMBER) {
			char *end;
			float x = strtof(value, &end);
			if (*end != '\0' || !in_range(d, x)) {
				*bad_line = line;
				return TABLE_OUT_OF_RANGE;
			}
			if (d->read_only && x != s->v[k]) {
				*bad_line = line;
				return TABLE_READ_ONLY;
			}
			t.v[k] = x;
		} else if (d->kind == KIND_WORD) {
			int w = word_index(d->words, value);
			if (w < 0) {
				*bad_line = line;
				return TABLE_OUT_OF_RANGE;
			}
			t.v[k] = (float)w;
		} else {
			if (strlen(value) > TEXT_MAX) {
				*bad_line = line;
				return TABLE_OUT_OF_RANGE;
			}
			strcpy(t.text[k], value);
		}
	}
	/* LIVE_GNSS_N's range follows LIVE_GNSS_BY (SETTINGS.md): 1–255 frames, 100–10 000 ms. */
	int by = (int)t.v[settings_index("LIVE_GNSS_BY")];
	float n = t.v[settings_index("LIVE_GNSS_N")];
	if ((by == 1 && n > 255) || (by == 2 && n < 100)) {
		*bad_line = 0;
		return TABLE_OUT_OF_RANGE;
	}
	*s = t;
	*bad_line = 0;
	return TABLE_TAKEN;
}

size_t settings_render(const struct settings *s, char *out, size_t cap)
{
	size_t n = 0;

	for (int i = 0; i < SETTINGS_COUNT; i++) {
		const struct setting_def *d = &setting_defs[i];
		int w;

		if (d->kind == KIND_NUMBER) {
			w = snprintf(out + n, cap - n, "%s %g\n", d->name, (double)s->v[i]);
		} else if (d->kind == KIND_WORD) {
			w = snprintf(out + n, cap - n, "%s %s\n", d->name, d->words[(int)s->v[i]]);
		} else {
			if (s->text[i][0] == '\0') {
				continue; /* no name given yet */
			}
			w = snprintf(out + n, cap - n, "%s %s\n", d->name, s->text[i]);
		}
		if (w < 0 || (size_t)w >= cap - n) {
			return 0;
		}
		n += (size_t)w;
	}
	return n;
}

uint32_t settings_crc(const struct settings *s)
{
	char buf[1024];
	size_t n = settings_render(s, buf, sizeof(buf));

	return crc32_update(0, buf, n);
}
