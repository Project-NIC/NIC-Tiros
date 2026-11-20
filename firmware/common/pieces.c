#include <string.h>
#include "crc32.h"
#include "pieces.h"

void pieces_reset(struct pieces *p)
{
	memset(p, 0, sizeof(*p));
}

size_t pieces_join(const struct pieces *p, char *out, size_t cap)
{
	size_t n = 0;

	for (unsigned i = 0; i < p->count; i++) {
		if (n + p->len[i] > cap) {
			return 0;
		}
		memcpy(out + n, p->text[i], p->len[i]);
		n += p->len[i];
	}
	return n;
}

enum pieces_state pieces_add(struct pieces *p, uint32_t crc, uint8_t piece, uint8_t count,
			     const char *text, size_t len)
{
	if (count == 0 || count > PIECES_MAX || piece >= count || len > PIECE_TEXT) {
		return PIECES_REFUSED;
	}
	if (p->count == 0 || crc != p->crc || count != p->count) {
		pieces_reset(p);
		p->crc = crc;
		p->count = count;
	}
	memcpy(p->text[piece], text, len);
	p->len[piece] = (uint16_t)len;
	p->have |= (uint16_t)(1u << piece);
	if (p->have != (uint16_t)((1u << count) - 1u)) {
		return PIECES_WAITING;
	}
	char whole[PIECES_MAX * PIECE_TEXT];
	size_t n = pieces_join(p, whole, sizeof(whole));
	if (crc32_update(0, whole, n) != crc) {
		pieces_reset(p);
		return PIECES_BAD_CRC;
	}
	return PIECES_WHOLE;
}
