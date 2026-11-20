/* NIC-Tiros — when a unit listens (RADIO.md, *When a unit listens*), shared by the units and Ham:
 * on and ready, all the time, on the link's channel in the live view's mode; in flight and landed,
 * LISTEN_MS after each frame it sends, on that frame's channel and mode — a Ham's command to it is
 * sent the moment one of its frames is heard. */
#ifndef TIROS_LISTEN_H
#define TIROS_LISTEN_H
#include <stdint.h>
#include "frames.h"
#include "settings.h"

#define CHANNEL_A_HZ 869525000u
#define CHANNEL_B_HZ 869850000u

struct radio_use {
	uint32_t freq_hz;
	uint8_t sf;
	uint16_t bw_khz;
	int continuous;         /* 1 all the time; 0 after_ms after each frame sent */
	uint16_t after_ms;
};

/* How a unit in phase ph listens, by its table. */
struct radio_use listen_when(const struct settings *s, enum phase ph);
/* Whether a command to a unit in phase ph waits for one of its frames (1) or goes at once (0). */
int listen_held(enum phase ph);

#endif
