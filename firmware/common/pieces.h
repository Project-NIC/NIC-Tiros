/* NIC-Tiros — a table received in pieces (RADIO.md, 5): taken only when every piece has come and
 * the CRC-32 of the whole agrees; the pieces tied together by that CRC. */
#ifndef TIROS_PIECES_H
#define TIROS_PIECES_H
#include <stddef.h>
#include <stdint.h>

#define PIECES_MAX 16u              /* 16 × 200 B = 3.2 kB, some 160 settings */
#define PIECE_TEXT 200u

struct pieces {
	uint32_t crc;
	uint8_t count;
	uint16_t have;              /* a bit a piece */
	uint16_t len[PIECES_MAX];
	char text[PIECES_MAX][PIECE_TEXT];
};

enum pieces_state { PIECES_WAITING, PIECES_WHOLE, PIECES_BAD_CRC, PIECES_REFUSED };

void pieces_reset(struct pieces *p);
/* A piece heard; a piece of another CRC starts the collection again. */
enum pieces_state pieces_add(struct pieces *p, uint32_t crc, uint8_t piece, uint8_t count,
			     const char *text, size_t len);
/* The whole text joined, its length (once PIECES_WHOLE). */
size_t pieces_join(const struct pieces *p, char *out, size_t cap);

#endif
