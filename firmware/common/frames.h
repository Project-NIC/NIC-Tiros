/* NIC-Tiros — the radio's frames, byte for byte as RADIO.md sets them out.
 * Every frame: a 6 B head; commanding frames end in a 4 B AES-CCM tag, written by the
 * radio's crypto (CRACEN) after encoding — here its room is kept and left zero.
 */
#ifndef TIROS_FRAMES_H
#define TIROS_FRAMES_H
#include <stddef.h>
#include <stdint.h>

enum frame_type {
	FRAME_BEACON = 1, FRAME_LIVE = 2, FRAME_COMMAND = 3, FRAME_ANSWER = 4,
	FRAME_TABLE = 5, FRAME_LOG = 6,
};

enum command {
	CMD_ARM = 1, CMD_DISARM = 2, CMD_START = 3, CMD_STOP = 4, CMD_POWER = 5, CMD_BEEP = 6,
	CMD_PING = 7, CMD_TABLE_GET = 8, CMD_LOG_GET = 9, CMD_OFF = 10,
	/* In an answer to a table received: 128, the commands keeping 1–127 (RADIO.md, 4). */
	ANSWER_TABLE_RECEIVED = 128,
};

enum result {
	RESULT_DONE = 0, RESULT_REFUSED = 1, RESULT_UNKNOWN = 2, RESULT_OUT_OF_RANGE = 3,
	RESULT_PIECES_MISSING = 4, RESULT_TOO_FAR = 5,
};

enum phase { PHASE_ON = 0, PHASE_READY = 1, PHASE_CLIMB = 2, PHASE_DESCENT = 3, PHASE_LANDED = 4 };

#define ADDR_HAM_MASTER 64u /* 64–67 Ham 1–4 */
#define ADDR_ALL 255u
#define TAG_LEN 4u

/* The sizes, with the tag where the frame has one (RADIO.md's headings). */
#define HEAD_LEN 6u
#define BEACON_LEN 26u
#define LIVE_LEN 22u
#define LIVE_GNSS_LEN 37u
#define COMMAND_LEN(has_arg) ((has_arg) ? 12u : 11u)
#define ANSWER_LEN 17u
#define PIECE_HEAD_LEN 12u /* head, CRC-32 of the whole, piece, pieces */
#define TABLE_PIECE_TEXT_MAX 200u
#define LOG_PIECE_RECORDS_MAX 3u
#define LOG_RECORD_LEN 64u

struct head {
	uint8_t type;
	uint16_t set;
	uint8_t sender, target;
	uint8_t count; /* the low byte of the sender's 32-bit count */
};

struct fix {
	int32_t lat_e7, lon_e7; /* 10⁻⁷ deg */
	int16_t height_m;
	uint8_t kind;           /* 0 none, 2 2D, 3 3D */
	uint8_t satellites;     /* 15 = 15 or more */
};

struct beacon {
	struct fix fix;
	uint8_t accuracy_m;     /* 255 worse */
	uint8_t age_min;        /* 255 older */
	uint8_t phase;
	uint16_t cell_mv;       /* sent at 20 mV a step */
	int8_t rssi_dbm;        /* −128 none */
};

struct live {
	uint32_t step;
	int32_t pressure_64pa;  /* 1/64 Pa, the barometer's own number */
	int16_t accel_cg;       /* 0.01 g, its magnitude */
	int16_t roll_dps;
	uint8_t phase, events;
	uint16_t cell_mv;
	int has_gnss;
	struct fix fix;
	uint32_t fix_step;      /* the fix's own step, on the record's time line */
};

struct answer {
	uint8_t command, result;
	int8_t rssi_dbm;
	uint32_t table_crc;
};

size_t frame_head_put(uint8_t *out, const struct head *h);
int frame_head_get(const uint8_t *in, size_t len, struct head *h);
size_t frame_beacon_put(uint8_t *out, const struct head *h, const struct beacon *b);
size_t frame_live_put(uint8_t *out, const struct head *h, const struct live *l);
size_t frame_command_put(uint8_t *out, const struct head *h, uint8_t command, int has_arg, uint8_t arg);
size_t frame_answer_put(uint8_t *out, const struct head *h, const struct answer *a);
/* A piece of the table (type 5) or of the boot log (type 6): payload up to its limit. */
size_t frame_piece_put(uint8_t *out, const struct head *h, uint32_t whole_crc, uint8_t piece,
		       uint8_t pieces, const uint8_t *payload, size_t len);

/* The other way, for Ham: each checks the length and the type, 0 good, −1 not this frame. The tag
 * is checked first, by seal.h. A beacon hidden by BEACON_SECRET is opened before it is read. */
int frame_beacon_get(const uint8_t *in, size_t len, struct beacon *b);
int frame_live_get(const uint8_t *in, size_t len, struct live *l);
int frame_answer_get(const uint8_t *in, size_t len, struct answer *a);
/* A piece: its whole's CRC, its number and count; *payload points into in, *plen its length. */
int frame_piece_get(const uint8_t *in, size_t len, uint32_t *whole_crc, uint8_t *piece,
		    uint8_t *pieces, const uint8_t **payload, size_t *plen);

#endif
