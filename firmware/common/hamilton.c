/* NIC-Tiros — Hamilton's machine of states, as HAMILTON.md sets it out. Every condition is a
 * setting and 0 switches it off; nothing is a rocket's or a plane's. */
#include <math.h>
#include "frames.h"
#include "hamilton.h"

#define STILL_G 0.1f   /* "the acceleration near 1 g": within 0.1 g — to be tried on the bench */
#define STEADY_M 1.0f  /* "the pressure steady": the height within 1 m over the stillness */
#define BOUNCE_MS 5000 /* a spike not followed by stillness within this is forgotten — a motor
                        * burning or a gust is not a landing; bounces after an impact settle in it */

static float S(const struct settings *s, const char *n) { return settings_get(s, n); }

void ham_init(struct hamilton *h, uint32_t t_ms)
{
	*h = (struct hamilton){ .state = ST_ON, .last_heard_t = t_ms };
}

static void emit(struct ham_out *o, enum ham_event e, uint32_t t)
{
	o->event = e;
	o->event_t_ms = t;
}

static void to_ready(struct hamilton *h, uint32_t t)
{
	h->state = ST_READY;
	h->armed_t = t;
	h->departing = 0;
	h->confirming = 0;
}

static void to_flight(struct hamilton *h, uint32_t launch_t, float height)
{
	h->state = ST_CLIMB;
	h->launch_t = launch_t;
	h->top_m = isnan(height) ? -INFINITY : height;
	h->top_t = launch_t;
	h->spiked = 0;
	h->still = 0;
}

static void to_landed(struct hamilton *h, struct ham_out *o, uint32_t t)
{
	h->state = ST_LANDED;
	emit(o, EV_LANDED, t);
}

enum ham_state ham_tick(struct hamilton *h, const struct settings *s, const struct ham_input *in,
			struct ham_out *o)
{
	uint32_t t = in->t_ms;
	float dev = fabsf(in->accel_g - 1.0f);

	o->event = EV_NONE;
	if (in->heard) {
		h->last_heard_t = t;
	}
	if (h->state == ST_OFF) {
		return h->state;
	}

	/* What any awake state obeys. */
	if (in->command == CMD_OFF && h->state != ST_CLIMB && h->state != ST_DESCENT) {
		h->state = ST_OFF;
		emit(o, EV_OFF, t);
		return h->state;
	}

	switch (h->state) {
	case ST_ON: {
		float idle = S(s, "IDLE_OFF_MIN");
		if (in->command == CMD_ARM || (int)S(s, "ARM_BY") == 1) {
			to_ready(h, t);
			emit(o, EV_ARMED, t);
		} else if (idle > 0 && t - h->last_heard_t >= (uint32_t)(idle * 60000.0f)) {
			h->state = ST_OFF;
			emit(o, EV_OFF, t);
		}
		break;
	}
	case ST_READY: {
		float lg = S(s, "LAUNCH_G"), up = S(s, "LAUNCH_UP_M");
		float smax = S(s, "SESSION_MAX_S");

		if (in->command == CMD_DISARM) {
			h->state = ST_ON;
			emit(o, EV_DISARMED, t);
			break;
		}
		if (in->command == CMD_START) {
			to_flight(h, t, in->height_m);
			emit(o, EV_LAUNCHED, t);
			break;
		}
		if (smax > 0 && t - h->armed_t >= (uint32_t)(smax * 1000.0f)) {
			to_ready(h, t);  /* the ring dropped, a new wait begins */
			emit(o, EV_GIVEN_UP, t);
			break;
		}
		if (lg <= 0) {
			break;           /* no launch looked for: START alone */
		}
		if (h->confirming) {
			if (!isnan(in->height_m) && in->height_m >= h->ground_m + up) {
				to_flight(h, h->depart_t, in->height_m);
				emit(o, EV_LAUNCHED, h->depart_t);
			} else if (t - h->depart_t >= (uint32_t)(S(s, "LAUNCH_UP_S") * 1000.0f)) {
				h->confirming = 0; /* a bump: still ready */
				h->departing = 0;
			}
			break;
		}
		if (dev >= lg) {
			if (!h->departing) {
				h->departing = 1;
				h->depart_t = t;
				h->ground_m = in->height_m;
			} else if (t - h->depart_t >= (uint32_t)S(s, "LAUNCH_MS")) {
				if (up <= 0 || isnan(h->ground_m)) {
					to_flight(h, h->depart_t, in->height_m);
					emit(o, EV_LAUNCHED, h->depart_t);
				} else {
					h->confirming = 1;
				}
			}
		} else {
			h->departing = 0;
		}
		break;
	}
	case ST_CLIMB:
	case ST_DESCENT: {
		float apo = S(s, "APOGEE_M"), lg = S(s, "LAND_G"), still_s = S(s, "LAND_STILL_S");
		float smax = S(s, "SESSION_MAX_S");

		if (in->command == CMD_STOP || in->cell_empty ||
		    (smax > 0 && t - h->launch_t >= (uint32_t)(smax * 1000.0f))) {
			to_landed(h, o, t);
			break;
		}
		if (h->state == ST_CLIMB && !isnan(in->height_m)) {
			if (in->height_m > h->top_m) {
				h->top_m = in->height_m;
				h->top_t = t;
			} else if (apo > 0 && in->height_m <= h->top_m - apo) {
				h->state = ST_DESCENT;
				emit(o, EV_APOGEE, h->top_t);
				break;
			}
		}
		if (lg > 0 && !h->spiked && in->accel_g >= lg) {
			h->spiked = 1;
			h->spike_t = t;
			h->still = 0;
			if (still_s <= 0) {
				to_landed(h, o, t);
			}
			break;
		}
		if (still_s <= 0 || (lg > 0 && !h->spiked)) {
			break;           /* no stillness looked for, or the spike not yet come */
		}
		int steady = isnan(in->height_m) || !h->still ||
			     fabsf(in->height_m - h->still_ref_m) <= STEADY_M;
		if (dev < STILL_G && steady) {
			if (!h->still) {
				h->still = 1;
				h->still_since = t;
				h->still_ref_m = in->height_m;
			} else if (t - h->still_since >= (uint32_t)(still_s * 1000.0f)) {
				to_landed(h, o, lg > 0 ? h->spike_t : h->still_since);
			}
		} else {
			h->still = 0;
			if (h->spiked && t - h->spike_t > BOUNCE_MS) {
				h->spiked = 0;
			}
		}
		break;
	}
	case ST_LANDED:
		if (in->command == CMD_ARM) {
			to_ready(h, t);
			emit(o, EV_ARMED, t);
		} else if (in->command == CMD_DISARM) {
			h->state = ST_ON;
			emit(o, EV_DISARMED, t);
		}
		break;
	default:
		break;
	}
	return h->state;
}
