/* NIC-Tiros — when a unit listens, as listen.h and RADIO.md set it out. */
#include "listen.h"

static void mode(int index, int beacon, struct radio_use *u)
{
	/* SETTINGS.md: LIVE_MODE SF7/250, SF7/125, SF8/125, SF9/125; BEACON_MODE SF9…SF12/125 */
	static const uint8_t live_sf[] = { 7, 7, 8, 9 };
	if (beacon) {
		u->sf = (uint8_t)(9 + (index & 3));
		u->bw_khz = 125;
	} else {
		u->sf = live_sf[index & 3];
		u->bw_khz = index == 0 ? 250 : 125;
	}
}

struct radio_use listen_when(const struct settings *s, enum phase ph)
{
	struct radio_use u = { 0 };
	const char *channel = ph <= PHASE_READY ? "LINK_CHANNEL" : ph == PHASE_LANDED ? "BEACON_CHANNEL" : "LIVE_CHANNEL";

	u.freq_hz = (int)settings_get(s, channel) == 0 ? CHANNEL_A_HZ : CHANNEL_B_HZ;
	if (ph == PHASE_LANDED) {
		mode((int)settings_get(s, "BEACON_MODE"), 1, &u);
	} else {
		mode((int)settings_get(s, "LIVE_MODE"), 0, &u);
	}
	u.continuous = ph <= PHASE_READY;
	u.after_ms = u.continuous ? 0 : (uint16_t)settings_get(s, "LISTEN_MS");
	return u;
}

int listen_held(enum phase ph)
{
	return ph >= PHASE_CLIMB;
}
