# NIC-Tiros — the radio's frames

LoRa between the units (Baker, Able) and up to four Hams. LoRa's own packet already carries a
preamble, a sync word, a length and a CRC-16; these frames are its payload, every number
little-endian. **Copied from the professionals and cut to what Tiros needs:** MAVLink's sender and
target numbers, several ground stations side by side, and its whole-table parameter transfer
(ArduPilot's parameter file over MAVLink's file transfer); LoRaWAN's short device address. The
times on air: `calc/out/airtime.md`.

## The two channels

**Channel B by default, A where reach is wanted**: the low-power channel
carries everything but the beacon after the landing; Ham's page sets channel A for the live view or
the commands where the field is wide, or for finding a unit fallen far.

| | the band | power | duty | what goes there |
|---|---|---|---|---|
| **A** | 869.40–869.65 MHz, one 250 kHz channel at 869.525 | 500 mW e.r.p. | under 10 % | **by default the beacon after the landing alone**; the live view, commands and answers when set for reach (`LIVE_CHANNEL`, `LINK_CHANNEL`) |
| **B** | 869.70–870.00 MHz, at 869.85 | 5 mW e.r.p. | none | **by default everything else**: the live view before the start and in flight, commands and answers; the table of settings always — it reaches some 1 km on open ground at SF7, 250 m from grass (`calc/out/numbers.md`) |

The Czech rule, ČTÚ's general authorisation VO-R/10/07.2021-8, bands g7 and g8: channel A may be
the whole band as one channel (its footnote 10), so 250 kHz is allowed. **The 10 % is counted
within any one hour** (its footnote 6) — a sliding hour, not the clock's: a 20 s flight at 20 live
frames a second takes some 11 s of any hour's 360 s; ten minutes of beacons every 5 s at SF12
some 200 s. **The unit keeps its own account,** as LoRaWAN's stacks do: every frame sent on
channel A added up over the last hour, and the beacons stretched before 360 s is reached. The
live view in flight is never cut by it — the account keeps room for one flight.

**869.525 MHz is also LoRaWAN's** — the gateways' downlinks (RX2) and Class B beacons
(RP002-1.0.3), sent with up to the band's 500 mW. Their sync word is the public one, 0x3444; ours the
private, 0x1424 (SX1261/2 datasheet, the register 0x0740) — so they are not read as ours, but a frame of ours sent while a gateway is talking may be lost;
the count in the head shows it. Channel A carries by default the beacon alone, so a frame lost now and then costs little.

**Ham is the master, the units answer.** Up to four Hams in a set: **Ham 1 the master** — the
only one that sends commands and tables; Hams 2–4 only hear, the backups, each writing down every
frame. Hearing costs nothing. Every sender still listens before it sends (the SX1262's channel
activity detection, datasheet 6.1.5), for the other sets on the field. That alone is not
EN 300 220's LBT, which would lift the 10 % (the authorisation's footnote 8) — its own conditions
are the standard's, not had; the 10 % is kept. One unit's live view at 20 a second fills some 57 % of channel A — **one unit
live at a time at that rate**; rockets are flown one after another anyway.

**A set holds 63 units;** more on one field are more sets, each its own number — a contest in
groups. A unit's name (`NAME`) is in its table: Ham knows it from the pairing, the frames carry
the number alone.

## When a unit listens

A unit listens only when a Ham can expect it to — as LoRaWAN's class A does once it flies:

| the phase | it listens | where |
|---|---|---|
| on, ready | **all the time** | `LINK_CHANNEL` in `LIVE_MODE` — B, SF7/250 by default |
| climb, descent | `LISTEN_MS` after each live frame it sends | that frame's: `LIVE_CHANNEL`, `LIVE_MODE` |
| landed | `LISTEN_MS` after each beacon | that beacon's: `BEACON_CHANNEL`, `BEACON_MODE` — A, SF12/125 by default |

So **a Ham sends a command to a unit on the pad at once; to one flying or landed it holds the
command and sends it the moment one of that unit's frames is heard**, on the same channel and mode,
within its `LISTEN_MS`. A landed unit's radio sleeps between beacons — its cell lasts the days of a
search. **Ham listens on B in the live view's mode while any unit is on the pad or flying, and on
A in the beacon's mode once every unit heard has landed — or a flying one has been silent a minute,
lost on B and looked for by its beacon on A.** One receiver: no listening interleaved. Written once for both sides
(`firmware/common/listen.c`).

## The key — as NIC-Strelka, as LoRaWAN

- **The set's key, AES-128, drawn at random by Ham** with the set's number, and **given on
  the cable** as NIC-Strelka gives its set: a short text file on the unit's disk, `TIROS.SET` —
  `set =`, `key =` 32 hex digits, `number =` — which the unit takes, keeps, overwrites and
  deletes; its light green 2 s, or red 2 s for a file refused. Hams 2–4 the same way. The key
  never goes on the air. **Ham's password**, at least twelve characters, guards Ham's page,
  checked by a challenge and its answer and never sent, Ham keeping its SHA-256 — as Strelka's
  Povodok does (`HAM.md`).
- **Signed: commands, answers, the table's pieces and the beacon** — 4 B more at the end, the
  tag of **AES-CCM** (the nRF54's CRACEN, as Strelka signs its frames); the head is signed with
  them. A frame whose tag fails is dropped without an answer.
- **The beacon also hidden if set** (`BEACON_SECRET`, on by default): its fields encrypted by the
  same CCM, read only by the set's Hams. Off, anyone may read it, but no one can forge it.
- **The live view open:** nothing in it to hide, and every byte costs at 20 a second.
- **No frame twice:** each sender counts its frames in 32 bits — kept through a power-off, as
  LoRaWAN's frame counter — and the count's low byte rides in the head; **the nonce, 13 B: the set
  u16, the sender u8, the whole count u32, little-endian, then six zero bytes**. A new set starts
  every count at 0. Ham takes the whole count as the next above the last it heard from that sender
  (the low byte's round); a replay has the wrong whole count and its tag fails. A Ham that joins a
  set late tries the rounds upward until a tag holds. **What the tag covers:** the whole frame
  before it, nothing encrypted; a hidden beacon has its bytes 6–21 encrypted and the head
  authenticated (`firmware/common/seal.c`).
- **The set's number and the signal threshold stay:** the number sorts the teams, the threshold
  keeps a table from being sent to the wrong rocket by mistake; the key is the lock.

## The head — every frame, 6 B

| bytes | the field | what for |
|---|---|---|
| 0 | the type u8 | 1 beacon · 2 live · 3 command · 4 answer · 5 a piece of the table · 6 a piece of the boot log |
| 1–2 | the set u16 | chosen at random when units and Hams are paired on the cable; a frame of another set is ignored — two teams on one field do not mix |
| 3 | the sender u8 | 1–63 a unit; 64–67 Ham 1–4, 64 the master |
| 4 | the target u8 | as the sender; 255 all of the set |
| 5 | the count u8 | the low byte of the sender's 32-bit count: a lost frame is seen, a replayed one refused |

## 1 The beacon — the unit's position, 26 B, channel A

| bytes | the field | |
|---|---|---|
| 0–5 | the head | type 1, to 255 |
| 6–9 | latitude i32, 10⁻⁷ deg | **the last good fix** — below |
| 10–13 | longitude i32, 10⁻⁷ deg | |
| 14–15 | height i16, m | above the sea |
| 16 | the fix u8 | the high nibble its kind (0 none, 2 2D, 3 3D), the low the satellites (15 = 15 or more) |
| 17 | its accuracy u8, m | the GNSS's own horizontal estimate (u-blox's hAcc), 255 worse |
| 18 | the fix's age u8 | minutes, 255 older |
| 19 | the phase u8 | 0 on (awake, not ready), 1 ready, 2 climb, 3 descent, 4 landed |
| 20 | the cell u8 | 20 mV a step |
| 21 | the last signal heard from a Ham i8, dBm | −128 none |
| 22–25 | the tag, 4 B | AES-CCM; bytes 6–21 encrypted when `BEACON_SECRET` is on |

**The last good fix is kept and sent again.** A fix within `BEACON_ACC_M` replaces the one held;
a worse one, or none — the unit in the grass, the sky hidden — does not, and the beacon goes on
sending the good one, its age growing. The trackers' way (APRS, the radiosondes'): the last
known position, never a wandering one.

## 2 The live view — 22 B, with the GNSS 37 B

Before the start on channel B, if set (`LIVE_READY_HZ`); in flight on channel B or A (`LIVE_CHANNEL`);
**and one more at the landing, its phase 4** — so a Ham turns to the beacon's channel at once.

| bytes | the field | |
|---|---|---|
| 0–5 | the head | type 2, to 255 |
| 6–9 | the step u32 | of the fast stream: the time since the session's start |
| 10–13 | the pressure i32, 1/64 Pa | the barometer's own number, as the record has it (`RECORD.md`, `PRESS`) |
| 14–15 | the acceleration i16, 0.01 g | its magnitude |
| 16–17 | the roll rate i16, deg/s | about the body's axis |
| 18 | the phase u8 | as the beacon's |
| 19 | the events u8, bits | the record's event n as bit n−1: 0 armed, 1 launched, 2 apogee, 3 landed; held once set |
| 20 | the cell u8 | 20 mV a step |
| 21 | the contents u8, bits | bit 0: the GNSS follows |

The height and the vertical speed are Ham's: from the pressures' differences, against the
pressure at the start.

**The GNSS, 15 B more** — in every n-th frame or every n ms, as set (`LIVE_GNSS_BY`, `LIVE_GNSS_N`):

| bytes | the field | |
|---|---|---|
| 22–25 | latitude i32, 10⁻⁷ deg | |
| 26–29 | longitude i32, 10⁻⁷ deg | |
| 30–31 | height i16, m | above the sea |
| 32 | the fix u8 | as the beacon's |
| 33–36 | the fix's step u32 | **its own time**, on the same line as the frame's step and the record's |

**One time line.** The position rides in whichever frame is going out, but it carries its own
time — the step of the fast stream at the fix's PPS, the same number the record gives it. A frame
sent 20 ms after the fix shows no second, different time: every number on the screen and in the
record agrees. Where the line itself stands against UTC does not matter to that — shifted by
30 s or by five years, the times within it stay exact; the record's `TIME_UTC` ties it to the
world.

## 3 A command — a Ham to a unit (255 all), 11–12 B, channel B, or A if set

| bytes | the field | |
|---|---|---|
| 0–5 | the head | type 3 |
| 6 | the command u8 | below |
| 7 | its argument | if it has one |
| last 4 | the tag | AES-CCM |

| command | argument | what |
|---|---|---|
| 1 ARM | — | make ready |
| 2 DISARM | — | back to on, not ready |
| 3 START | — | start recording now ("start measuring") |
| 4 STOP | — | end the session |
| 5 POWER | i8 dBm | set the sending power — for finding |
| 6 BEEP | u8 seconds | the buzzer |
| 7 PING | — | answer at once — for finding by signal |
| 8 TABLE_GET | — | send the whole table of settings — channel B only |
| 9 LOG_GET | u8 how many records, 0 all | send the boot log (`BOOTLOG.md`) — channel B |
| 10 OFF | — | switch off: the ship mode, woken again only by the magnet or the cable |

## 4 The answer — a unit to the Ham that asked, 17 B

| bytes | the field | |
|---|---|---|
| 0–5 | the head | type 4 |
| 6 | the command it answers u8 | **128** for a table received (the commands keep 1–127) |
| 7 | the result u8 | 0 done ("measuring" after START), 1 refused, 2 unknown, 3 out of range, 4 pieces missing, 5 too far |
| 8 | the signal it heard the Ham at i8, dBm | for finding both ways |
| 9–12 | the CRC-32 of the table it holds now | so the Ham sees at once whether its table is the unit's |
| 13–16 | the tag | AES-CCM |

## 5 The table of settings — whole, never one by one, channel B only

**Settings are changed only as a whole table** — the same text as the disk's `PARAMS.TXT`
(`SETTINGS.md`), on the cable or on channel B on site; never on channel A, never in flight. **Over
the radio three things must hold**: the set's number, channel B, and **Ham
heard at least as strong as `TABLE_RSSI`** — some −40 dBm, some 10 m in free space at 5 mW, less
in the field; weaker, the unit answers 5, too far. **On the cable always.** Reading a table
(TABLE_GET) asks the set's number alone. A safeguard against a mistake, not a lock against a
stranger. A Ham
sends it in pieces; the unit takes it only when every piece has come, the CRC-32 agrees and every
value is within its range — else it keeps its old table whole and answers why. Asked by
TABLE_GET, the unit sends its own the same way.

| bytes | the field | |
|---|---|---|
| 0–5 | the head | type 5 |
| 6–9 | the CRC-32 of the whole table u32 | the same in every piece; ties the pieces together |
| 10 | the piece u8 | 0, 1, … |
| 11 | the pieces u8 | how many |
| 12–… | the text, up to 200 B | `NAME value` lines, as `PARAMS.TXT` — never the password, which is not a setting |
| last 4 | the tag | AES-CCM |

The 33 settings of today are some 450 B: three pieces, some 0.5 s at SF7/250; a hundred some 1.5 kB, eight pieces.

## 6 A piece of the boot log — a unit to the Ham that asked, up to 206 B, channel B

| bytes | the field | |
|---|---|---|
| 0–5 | the head | type 6 |
| 6–9 | the CRC-32 of all the records sent u32 | the same in every piece |
| 10 | the piece u8 | 0, 1, … |
| 11 | the pieces u8 | how many |
| 12–… | up to three records, 64 B each | as `BOOTLOG.md`, the newest first |
| last 4 | the tag | AES-CCM |

All 64 records are 22 pieces, some 3.6 s at SF7/250.
