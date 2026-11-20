/* NIC-Tiros — the signature on a frame (RADIO.md, *The key*): AES-128 in CCM with a 4 B tag.
 * The nonce, 13 B: the set u16, the sender u8, the sender's whole 32-bit count u32, all
 * little-endian, then six zero bytes — never the same twice for one key, the count never going
 * back. Signed frames (commands, answers, the pieces, the beacon): the whole frame before its tag
 * authenticated, nothing encrypted; a beacon hidden by BEACON_SECRET has its bytes 6–21 encrypted,
 * the head authenticated. The live view is not signed.
 * The AES-CCM itself is the platform's — CRACEN through PSA on the nRF54, mbedTLS on Ham's ESP32 —
 * given as one function; the frames' rules are here, the same on both. */
#ifndef TIROS_SEAL_H
#define TIROS_SEAL_H
#include <stddef.h>
#include <stdint.h>

#define NONCE_LEN 13u

/* The platform's CCM: tag 4 B. Encrypt: in → out and the tag made; decrypt: in → out and the tag
 * checked, 0 good. len may be 0 (authentication alone). */
typedef int (*ccm_fn)(const uint8_t key[16], const uint8_t nonce[NONCE_LEN], const uint8_t *aad,
		      size_t aad_len, const uint8_t *in, uint8_t *out, size_t len, uint8_t tag[4],
		      int encrypt);

void seal_nonce(uint8_t nonce[NONCE_LEN], uint16_t set, uint8_t sender, uint32_t count);

/* Sign a frame of len bytes (its tag's 4 B the last); hide its bytes 6..len−5 when hide. */
int seal_frame(ccm_fn ccm, const uint8_t key[16], uint8_t *frame, size_t len, uint32_t count, int hide);
/* Check a frame's tag (and open a hidden one in place): 0 good, −1 dropped. */
int seal_open(ccm_fn ccm, const uint8_t key[16], uint8_t *frame, size_t len, uint32_t count, int hidden);

/* The sender's whole count from the low byte in a head and the last whole count heard from it:
 * the next one above the last; 0 good, −1 a replay (not above the last) — the first frame heard
 * from a sender (have_last 0) taken as it comes. */
int seal_count(uint32_t last, int have_last, uint8_t low, uint32_t *whole);

#endif
