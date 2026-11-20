/* NIC-Tiros — a file per phase, as RECORD.md sets it out. */
#include <stdio.h>
#include "recfile.h"

uint32_t recfile_rate(const struct settings *s, enum ham_state st)
{
	const char *name = st == ST_READY ? "REC_READY" : st == ST_CLIMB ? "REC_CLIMB" :
			   st == ST_DESCENT ? "REC_DESC" : st == ST_LANDED ? "REC_LANDED" : 0;
	return name ? (uint32_t)settings_get(s, name) : 0;
}

enum recfile_act recfile_change(uint32_t running_hz, uint32_t next_hz, uint32_t *hz)
{
	*hz = next_hz;
	if (next_hz == running_hz)
		return RF_KEEP;
	if (!next_hz)
		return RF_END;
	return running_hz ? RF_NEXT : RF_BEGIN;
}

int recfile_name(char *out, uint16_t session, uint8_t unit, unsigned index)
{
	if (index > 25)
		return -1;
	if (index)
		snprintf(out, 17, "S%05uT%02u%c.MUFT", (unsigned)session, (unsigned)unit, 'A' + index);
	else
		snprintf(out, 17, "S%05uT%02u.MUFT", (unsigned)session, (unsigned)unit);
	return 0;
}
