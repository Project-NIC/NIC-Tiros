# NIC-Tiros — the settings

**Every setting of the firmware is open to the user** — set before a flight, kept in the unit.
The professionals' way, ArduPilot's and PX4's: **named parameters**, each a name, a value and its
range — here always set as a whole table. Two ways to set it:

- **On the cable:** the unit's disk holds `PARAMS.TXT`, one setting a line, `NAME value`, as
  ArduPilot's parameter files — opened in any editor, read by the unit at its next start; the
  unit writes back what it holds, so the file is always the truth.
- **Over the radio, on site:** Ham sends the whole table, the same text, in the 5 mW band without a
  duty limit (channel B, `RADIO.md`); never in flight, never one setting alone. The unit takes it
  only whole and within the ranges, and answers with its table's CRC-32.

A setting out of its range is refused and the old value kept.

## The flight's phases and their rates

A flight goes **ready → climb → descent → landed**; each phase may record at its own rate, or not
at all. A MUF stream has one rate, so a phase whose rate differs begins a new file, an equal rate
keeping the one (`RECORD.md`, *A file per phase*): a rocket two files, a plane (`APOGEE_M 0`) one.

| name | default | range | what |
|---|---|---|---|
| `REC_READY` | 0 | 0, 1–4000 Hz | recording while ready (0: only the ring below) |
| `REC_CLIMB` | 1000 | 0, 1–4000 Hz | from the launch to the apogee (the whole flight if `APOGEE_M` is 0) |
| `REC_DESC` | 100 | 0, 1–4000 Hz | from the apogee to the landing; a rate other than the climb's begins a new file (`RECORD.md`, *A file per phase*) |
| `REC_LANDED` | 0 | 0, 1–100 Hz | after the landing (0: stop, the file closed) |
| `PRE_S` | 3 | 0–10 s | the ring kept before the launch, written at it |
| `SLOW_HZ` | 10 | 1–100 Hz | the air's stream (humidity, temperatures) |
| `GNSS_HZ` | 10 | 1–20 Hz | the GNSS's fixes while flying |

## Finding the phases

The machine and its conditions: `HAMILTON.md`. **0 switches a condition off**; all off, a flight
starts by START and ends by STOP or the time.

| name | default | range | what |
|---|---|---|---|
| `ARM_BY` | radio | radio, auto | how a woken unit is made ready: by ARM, or at once |
| `LAUNCH_G` | 1.0 | 0, 0.1–20 g | the acceleration's departure from 1 g, any axis, that may be a launch … |
| `LAUNCH_MS` | 50 | 10–2000 ms | … held this long … |
| `LAUNCH_UP_M` | 5 | 0, 1–100 m | … confirmed by the height rising this much … |
| `LAUNCH_UP_S` | 10 | 1–120 s | … within this time; else a bump |
| `APOGEE_M` | 3 | 0, 1–100 m | the top: the height this far below its highest |
| `LAND_G` | 3.0 | 0, 1.1–100 g | the impact's spike … |
| `LAND_STILL_S` | 10 | 0, 2–600 s | … then still this long: landed |
| `HEIGHT_BY` | baro | baro, gnss, either | where the height comes from |
| `IDLE_OFF_MIN` | 10 | 0, 1–1440 min | awake and not ready, nothing heard this long: off (the ship mode); 0 never |
| `SESSION_MAX_S` | 0 | 0, 60–86 400 s | a session's longest time, 0 none |

## The radio

| name | default | range | what |
|---|---|---|---|
| `LIVE` | on | on, off | the live view in flight |
| `LIVE_CHANNEL` | B | A, B | the live view in flight: B at 5 mW without a limit, or A at up to 500 mW and 10 % for reach |
| `LINK_CHANNEL` | B | A, B | commands and answers: B, or A for reach and for finding a unit fallen far |
| `BEACON_CHANNEL` | A | A, B | the beacon after the landing |
| `LIVE_HZ` | 20 | 1–30 | its packets a second in flight |
| `LIVE_MODE` | SF7/250 | SF7/250–SF9/125 | its LoRa mode |
| `LIVE_GNSS_BY` | frames | off, frames, ms | the position in the live view … |
| `LIVE_GNSS_N` | 10 | 1–255 frames, 100–10 000 ms | … in every n-th frame, or every n ms |
| `LIVE_READY_HZ` | 0 | 0–10 | the live view before the start, channel B; 0 off |
| `BEACON_EVERY_S` | 60 | 5–3600 s | the position once landed … |
| `BEACON_FAST_MIN` | 10 | 0–120 min | … every 5 s for this long first |
| `BEACON_MODE` | SF12/125 | SF9–SF12 | its LoRa mode |
| `BEACON_ACC_M` | 10 | 2–100 m | a fix this good replaces the one the beacon sends; a worse one does not |
| `TX_DBM` | 14 | 0–14 dBm (22 on a fitting cell) | the power — Ham may lower it while finding |
| `LISTEN_MS` | 100 | 0–1000 ms | listening after each packet sent |
| `UNIT` | 1 | 1–63 | the unit's number in its set (Hams are 64–67) |
| `SET` | — | 1–65 535 | the set's number, given with its key on the cable (`TIROS.SET`, `RADIO.md`) |
| `NAME` | — | 8 characters | the unit's name, shown by Ham |
| `TABLE_RSSI` | −40 | −90 to −20 dBm | a table over the radio is taken only from a Ham heard this strong — close by |
| `BEACON_SECRET` | on | on, off | the beacon encrypted for the set's Hams alone; off, readable by anyone, still signed |

## The rest

| name | default | range | what |
|---|---|---|---|
| `GNSS_OFF` | on | on, off | the GNSS off once a fix within `BEACON_ACC_M` is had after the landing |
| `BUZZER` | on | on, off | a beep once landed |
| `HEATER` | auto | auto, off | the humidity sensor's heater against dew |
| `CELL` | lfp | lfp, lipo | the cell's kind — the charge's end set by it (3.50 V / 4.10 V); never guessed |

To be widened as the firmware is written; a name once given keeps its meaning.
