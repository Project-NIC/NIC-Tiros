/* NIC-Tiros — the beacon's last good fix (RADIO.md, 1): a fix within BEACON_ACC_M replaces the one
 * held; a worse one, or none, does not — the beacon goes on with the good one, its age growing. */
#ifndef TIROS_LASTFIX_H
#define TIROS_LASTFIX_H
#include <stdint.h>
#include "frames.h"

struct lastfix {
	int have;
	struct fix fix;
	uint8_t accuracy_m;
	uint32_t at_ms;
};

void lastfix_offer(struct lastfix *l, const struct fix *f, float h_acc_m, float limit_m, uint32_t now_ms);
/* The beacon's fields from it: age in minutes, 255 older; accuracy 255 worse. */
void lastfix_beacon(const struct lastfix *l, struct beacon *b, uint32_t now_ms);

#endif
