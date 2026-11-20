/* NIC-Tiros — LoRa's time on air and the 10 % hour's account (RADIO.md, The two channels).
 * The formula: Semtech's SX1261/2 datasheet rev 1.2, 6.1.4 — the same as calc/airtime.py. */
#ifndef TIROS_AIRTIME_H
#define TIROS_AIRTIME_H
#include <stdint.h>

/* Time on air in µs: explicit header, CRC on, coding rate 4/5, a preamble of 8 symbols; the low
 * data rate optimisation on where a symbol lasts 16.38 ms or more. */
uint32_t lora_toa_us(unsigned payload, unsigned sf, unsigned bw_khz);

/* Channel A's account: what was sent within any one hour (ČTÚ's VO-R/10, footnote 6), at most
 * 10 % of it. The live view in flight is never refused; everything else must leave RESERVE for
 * one flight. */
#define DUTY_WINDOW_MS 3600000u
#define DUTY_BUDGET_US 360000000u     /* 10 % of an hour */
#define DUTY_RESERVE_US 30000000u     /* room kept for a flight's live view: 30 s */
#define DUTY_LOG 512u

struct duty {
	uint32_t at_ms[DUTY_LOG];     /* when each send ended */
	uint32_t us[DUTY_LOG];        /* how long it took */
	unsigned head, n;
};

void duty_init(struct duty *d);
uint32_t duty_used_us(struct duty *d, uint32_t now_ms);
/* May a frame of toa_us go now? flight: the live view in flight, never refused. */
int duty_may_send(struct duty *d, uint32_t now_ms, uint32_t toa_us, int flight);
void duty_sent(struct duty *d, uint32_t now_ms, uint32_t toa_us);

#endif
