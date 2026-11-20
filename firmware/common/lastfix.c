#include "lastfix.h"

void lastfix_offer(struct lastfix *l, const struct fix *f, float h_acc_m, float limit_m, uint32_t now)
{
	if (f->kind < 2 || h_acc_m > limit_m) {
		return; /* no fix, or a worse one: the good one stays */
	}
	l->have = 1;
	l->fix = *f;
	l->accuracy_m = h_acc_m >= 255 ? 255 : (uint8_t)(h_acc_m + 0.5f);
	l->at_ms = now;
}

void lastfix_beacon(const struct lastfix *l, struct beacon *b, uint32_t now)
{
	if (!l->have) {
		b->fix = (struct fix){ 0 };
		b->accuracy_m = 255;
		b->age_min = 255;
		return;
	}
	uint32_t min = (now - l->at_ms) / 60000u;
	b->fix = l->fix;
	b->accuracy_m = l->accuracy_m;
	b->age_min = min > 254 ? 255 : (uint8_t)min;
}
