/* NIC-Tiros — the settings: one named table (SETTINGS.md), set only whole.
 * Text form, as PARAMS.TXT and the radio's table pieces: "NAME value" a line.
 */
#ifndef TIROS_SETTINGS_H
#define TIROS_SETTINGS_H
#include <stddef.h>
#include <stdint.h>

enum setting_kind { KIND_NUMBER, KIND_WORD, KIND_TEXT };

struct setting_def {
	const char *name;
	enum setting_kind kind;
	float def, min, max;     /* a number's default and range */
	int zero_ok;             /* 0 allowed below min: "0 switches it off" */
	int read_only;           /* given on the cable (TIROS.SET), never by a table */
	const char *const *words; /* a word's choices; its default is words[(int)def] */
};

#define SETTINGS_COUNT 42
#define TEXT_MAX 8

struct settings {
	float v[SETTINGS_COUNT];          /* a number's value, or a word's index */
	char text[SETTINGS_COUNT][TEXT_MAX + 1];
};

extern const struct setting_def setting_defs[SETTINGS_COUNT];

int settings_index(const char *name);
void settings_defaults(struct settings *s);
float settings_get(const struct settings *s, const char *name);

enum table_result { TABLE_TAKEN = 0, TABLE_UNKNOWN_NAME = 2, TABLE_OUT_OF_RANGE = 3, TABLE_BAD_LINE = 4,
		    TABLE_READ_ONLY = 5 };

/* Take a table whole or not at all: on any error *s is left as it was and the line is told.
 * A name missing from the text keeps its value; an unknown name refuses the whole. */
enum table_result settings_take(struct settings *s, const char *text, size_t len, int *bad_line);

/* The unit's table as text; returns its length (0 if out is too small). */
size_t settings_render(const struct settings *s, char *out, size_t cap);
uint32_t settings_crc(const struct settings *s);

#endif
