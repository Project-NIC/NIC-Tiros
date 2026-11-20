/* NIC-Tiros — LoRa's time on air and the sliding hour's account. */
#include "airtime.h"

uint32_t lora_toa_us(unsigned pl, unsigned sf, unsigned bw_khz)
{
	/* in µs: a symbol is 2^SF / BW */
	uint32_t ts_us = (uint32_t)((1000u << sf) / bw_khz);
	int ldro = ts_us >= 16380;
	int num = 8 * (int)pl + 16 - 4 * (int)sf + 8 + 20; /* CRC on, explicit header */
	int den = 4 * ((int)sf - 2 * ldro);
	int blocks = num > 0 ? (num + den - 1) / den : 0;
	/* (8 + 4.25 + 8) symbols, in quarter symbols to stay in integers */
	uint32_t quarters = 4 * 8 + 17 + 4 * 8 + 4 * (uint32_t)blocks * 5;

	return quarters * ts_us / 4;
}

void duty_init(struct duty *d)
{
	d->head = 0;
	d->n = 0;
}

uint32_t duty_used_us(struct duty *d, uint32_t now)
{
	uint32_t sum = 0;

	/* drop what has left the hour */
	while (d->n && now - d->at_ms[(d->head + DUTY_LOG - d->n) % DUTY_LOG] >= DUTY_WINDOW_MS) {
		d->n--;
	}
	for (unsigned k = 0; k < d->n; k++) {
		sum += d->us[(d->head + DUTY_LOG - 1 - k) % DUTY_LOG];
	}
	return sum;
}

int duty_may_send(struct duty *d, uint32_t now, uint32_t toa_us, int flight)
{
	uint32_t used = duty_used_us(d, now);

	if (flight) {
		return 1;
	}
	return used + toa_us + DUTY_RESERVE_US <= DUTY_BUDGET_US;
}

void duty_sent(struct duty *d, uint32_t now, uint32_t toa_us)
{
	if (d->n == DUTY_LOG) {
		/* the log full: fold the oldest into the next oldest, so nothing is forgotten early */
		unsigned o = (d->head + DUTY_LOG - d->n) % DUTY_LOG, o2 = (o + 1) % DUTY_LOG;
		d->us[o2] += d->us[o];
		d->n--;
	}
	d->at_ms[d->head] = now;
	d->us[d->head] = toa_us;
	d->head = (d->head + 1) % DUTY_LOG;
	d->n++;
}
