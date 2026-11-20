/* NIC-Tiros — a file per phase (RECORD.md, *A file per phase*): a MUF stream has one rate, so a
 * phase whose rate differs from the running one ends the file and begins the next; an equal rate
 * keeps it; a rate of 0 records nothing. A rocket with the default table writes two files, a plane
 * (APOGEE_M 0) one. Plain C, no MUF here — the record's writer is record.c. */
#ifndef TIROS_RECFILE_H
#define TIROS_RECFILE_H
#include <stdint.h>
#include "hamilton.h"

/* The fast stream's rate in a state (REC_READY, REC_CLIMB, REC_DESC, REC_LANDED), 0 none. */
uint32_t recfile_rate(const struct settings *s, enum ham_state st);

/* What to do with the file at a change of state: */
enum recfile_act { RF_KEEP, RF_BEGIN, RF_NEXT, RF_END };
/* running_hz the open file's rate (0 none); returns the act and sets *hz to the rate after it. */
enum recfile_act recfile_change(uint32_t running_hz, uint32_t next_hz, uint32_t *hz);

/* The name of the session's index-th file (0 the first): S00007T01.MUFT, S00007T01B.MUFT, …;
 * out holds 17 bytes at least. Returns -1 past the 26th. */
int recfile_name(char *out, uint16_t session, uint8_t unit, unsigned index);

#endif
