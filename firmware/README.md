# Hamilton — NIC-Tiros's firmware

As NIC-Strelka's: **the protocol's and the record's code in plain C, compiled and tested on the
host** before any board exists; the nRF Connect SDK's project on Zephyr (target
`nrf54lm20dk/nrf54lm20a/cpuapp`) around it later.

## Written

| file | what | from |
|---|---|---|
| `common/frames.[ch]` | every radio frame encoded byte for byte, its size asserted by the tests; the tag's 4 B kept for CRACEN's AES-CCM | `RADIO.md` |
| `common/settings.[ch]` | the 42 settings, their defaults and ranges; a table taken whole or not at all; the text form and its CRC-32 | `SETTINGS.md` |
| `common/hamilton.[ch]` | the machine of states: the launch back-dated to the departure, confirmed by the height; the top; the landing by a spike and stillness; every condition 0-off; OFF, idle, the session's limit | `HAMILTON.md` |
| `common/bootlog.[ch]` | the boot log's 64 B record, its CRC, the ring's slot | `BOOTLOG.md` |
| `common/airtime.[ch]` | LoRa's time on air (Semtech's 6.1.4, the same figures as `calc/airtime.py`) and channel A's sliding hour: at most 10 %, 30 s kept for a flight, the live view in flight never refused | `RADIO.md` |
| `common/lastfix.[ch]` | the beacon's last good fix: a worse one never replaces it; its age and accuracy | `RADIO.md`, 1 |
| `common/pieces.[ch]` | a table received in pieces, out of order, taken when whole and its CRC-32 agrees | `RADIO.md`, 5 |
| `common/recfile.[ch]` | a file per phase: a rate other than the running one begins the next file, lettered `S00007T01B.MUFT`, … | `RECORD.md`, *A file per phase* |
| `common/crc32.[ch]`, `common/le.h` | CRC-32 (zlib's), little-endian | — |
| `common/record.[ch]` | **the record**: Tiros's profile in MUF — its 6 streams, its 22 groups — written through NIC-SDK's writer (`muf.c`, from a checkout of `Project-NIC/NIC-SDK` 0.2) | `RECORD.md` |
| `tests/fly.c`, `tests/test_record.py` | a rocket flown through the machine and written as a record, read back by NIC-SDK's reader: the head `MUFT`, the 22 groups, 80 s at 1 kHz whole, the pressure on the pad, the events back-dated (armed 0.010 s, launched 3.000 s, the top, the landing), the GNSS's 800 fixes and its top, the time's ties, the end | |
| `tests/test_host.c` | 119 checks: the frames' bytes, the settings, a file per phase, a rocket (two files), a plane (one), a bump on the pad, everything off, the idle off, the airtimes, an hour of beacons every 5 s, the last fix, a table in pieces, the boot log | |

Run: `make test`; the record: `python3 tests/test_record.py [SDK]` — `SDK` a checkout of
[NIC-SDK](https://github.com/Project-NIC/NIC-SDK), by default `NIC-SDK` beside `NIC-Tiros`.

## Found on the way

- **A MUF stream has one rate**, so a rate per phase is **a file per phase** (`RECORD.md`):
  `common/recfile.[ch]` decides when the next file begins and names it. `tests/fly.c` writes the
  one-rate case, one file; the next file written at a change of phase comes with the Zephyr project.
- **The answer to a table received is 128** — RADIO.md's first 9 had become LOG_GET's.
- **A motor's thrust is a spike too**: a spike not followed by stillness within 5 s is forgotten
  (`BOUNCE_MS`), or the burn would count as the impact.
- "The acceleration near 1 g" is taken as within 0.1 g, "the pressure steady" as the height within
  1 m (`STILL_G`, `STEADY_M`) — to be tried on the bench; settings if they prove to matter.
- A table missing some names keeps their values; an unknown name or a value out of range refuses
  the whole; `SET` is read-only (given by `TIROS.SET` on the cable).

## Not yet

The Zephyr project and its devicetree; the drivers' calls (the IMU's FIFO, the barometer, the
GNSS, the nPM1300, the SX1262); the
radio's timing; AES-CCM through the SDK's PSA crypto; the USB disk.
