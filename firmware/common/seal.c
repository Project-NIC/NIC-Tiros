/* NIC-Tiros — the signature on a frame, as seal.h and RADIO.md set it out. */
#include <string.h>
#include "le.h"
#include "seal.h"

void seal_nonce(uint8_t nonce[NONCE_LEN], uint16_t set, uint8_t sender, uint32_t count)
{
	memset(nonce, 0, NONCE_LEN);
	put_u16(nonce, set);
	nonce[2] = sender;
	put_u32(nonce + 3, count);
}

int seal_frame(ccm_fn ccm, const uint8_t key[16], uint8_t *frame, size_t len, uint32_t count, int hide)
{
	uint8_t nonce[NONCE_LEN];

	if (len < 6 + 4 || (uint8_t)count != frame[5]) {
		return -1;
	}
	seal_nonce(nonce, get_u16(frame + 1), frame[3], count);
	if (hide) {
		return ccm(key, nonce, frame, 6, frame + 6, frame + 6, len - 10, frame + len - 4, 1);
	}
	return ccm(key, nonce, frame, len - 4, 0, 0, 0, frame + len - 4, 1);
}

int seal_open(ccm_fn ccm, const uint8_t key[16], uint8_t *frame, size_t len, uint32_t count, int hidden)
{
	uint8_t nonce[NONCE_LEN];

	if (len < 6 + 4 || (uint8_t)count != frame[5]) {
		return -1;
	}
	seal_nonce(nonce, get_u16(frame + 1), frame[3], count);
	if (hidden) {
		return ccm(key, nonce, frame, 6, frame + 6, frame + 6, len - 10, frame + len - 4, 0) ? -1 : 0;
	}
	return ccm(key, nonce, frame, len - 4, 0, 0, 0, frame + len - 4, 0) ? -1 : 0;
}

int seal_count(uint32_t last, int have_last, uint8_t low, uint32_t *whole)
{
	uint32_t c;

	if (!have_last) {
		*whole = low;
		return 0;
	}
	c = (last & ~0xFFu) | low;
	if (c <= last) {
		c += 0x100u;
	}
	*whole = c;
	return c > last ? 0 : -1;
}
