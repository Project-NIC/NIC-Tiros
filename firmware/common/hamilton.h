/* NIC-Tiros — Hamilton's machine of states (HAMILTON.md), plain C: fed a tick at a time with what
 * the sensors and the radio say, it moves between the states and tells the events, each with
 * the time it truly fell at — a launch back-dated to the moment the acceleration first departed.
 */
#ifndef TIROS_HAMILTON_H
#define TIROS_HAMILTON_H
#include <stdint.h>
#include "settings.h"

enum ham_state { ST_OFF = -1, ST_ON = 0, ST_READY = 1, ST_CLIMB = 2, ST_DESCENT = 3, ST_LANDED = 4 };

/* The record's event numbers (RECORD.md, EVENT) and the session's end (17). */
enum ham_event { EV_NONE = 0, EV_ARMED = 1, EV_LAUNCHED = 2, EV_APOGEE = 3, EV_LANDED = 4,
		 EV_GIVEN_UP = 17, EV_OFF = 100, EV_DISARMED = 101 };

struct ham_input {
	uint32_t t_ms;        /* the unit's time line */
	float accel_g;        /* the acceleration's magnitude */
	float height_m;       /* from HEIGHT_BY; NAN when not known */
	uint8_t command;      /* a command heard and its tag good, 0 none (frames.h) */
	int heard;            /* any frame of the set heard this tick */
	int cell_empty;       /* under the cell's end for 10 s (as NIC-Strelka's) */
};

struct ham_out {
	enum ham_event event;
	uint32_t event_t_ms;  /* when it truly fell */
};

struct hamilton {
	enum ham_state state;
	/* the launch */
	int departing;
	uint32_t depart_t, launch_t;
	float ground_m;
	int confirming;
	/* the top */
	float top_m;
	uint32_t top_t;
	/* the landing */
	int spiked;
	uint32_t spike_t, still_since;
	float still_ref_m;
	int still;
	/* the session and the idle */
	uint32_t armed_t, last_heard_t;
};

void ham_init(struct hamilton *h, uint32_t t_ms);
/* One tick; returns the state after it and fills *out with an event, if one fell. */
enum ham_state ham_tick(struct hamilton *h, const struct settings *s, const struct ham_input *in,
			struct ham_out *out);

#endif
