# NIC-Tiros — the boot log (level 6)

**The last 64 starts, kept through power-off**: why the
unit started, how each step of the start went and how long it took, and the last words of the run
before. Copied from the professionals: a PLC's **diagnostic buffer** (a ring of events, each with
its time), Linux's **journal per boot** and `systemd-analyze blame` (each unit's start time),
**pstore** (the last words of a run that fell), Zephyr's **reset cause** and fatal error's
reason.

## Where it lives

- **In the processor's own RRAM, not on the card:** the card may be the very thing that fails.
  64 records × 64 B = 4 kB, a ring; the newest is the one with the highest start number.
- **Written as the start goes:** the record is written at the start and again after each step,
  so **a start that hangs shows the step it hung in**, and the watchdog's reset that follows is
  the next record's cause. RRAM bears 10 000 writes of each 128-bit unit (nRF54LM20 datasheet
  v1.0, *nENDURANCE*); some 13 writes a start, each slot every 64th start: 49 000 starts.
- **The last words** are left in RAM kept through a reset (Zephyr's retained memory) by the fault
  handler or the watchdog's warning, and taken into the next start's record; after a power-off
  there are none, and the record says so.
- **Read out:** on the cable, the unit's disk shows `BOOT.LOG` (the records as they are) — the
  converter on the computer prints them as text; over the radio, below.

## The record, 64 B

| bytes | the field | |
|---|---|---|
| 0–3 | the start's number u32 | counts every start since the unit was made |
| 4–7 | the reset's cause u32 | the processor's RESETREAS as read (nRF54LM20 datasheet 5.8.11.1: the pin, watchdog 0 and 1, the debugger's resets, a soft reset, a CPU lockup, a wake from System OFF, …); none of its bits set: a power-on or a brown-out — to be confirmed on the kit |
| 8–11 | the firmware u32 | the first 8 hex digits of its git commit |
| 12–15 | UTC u32, s since 1970 | 0 until the GNSS gives the time — then written in |
| 16–17 | the cell u16, mV | at the start |
| 18 | the temperature i8, °C | the processor's own sensor |
| 19 | the steps done u8 | of the twelve below; 12 a whole start |
| 20–43 | the steps, 12 × u16 | each: the top 3 bits its result — 0 good, 1 absent, 2 a wrong answer, 3 no answer in time, 4 an error, 7 not reached; the low 13 bits its time, ms (8 191 = 8.19 s or more) |
| 44–45 | the start's whole time u16, 10 ms | to 655 s |
| 46 | the run before: its end u8 | 0 none known (a power-off), 1 a fault, 2 the watchdog, 3 the firmware's own stop (an assert), 4 an orderly off |
| 47 | the run before: Zephyr's fatal reason u8 | as Zephyr names it (K_ERR_…), 255 none |
| 48–51 | the run before: the fault's address u32 | the program counter, 0 none |
| 52–55 | the run before: how long it ran u32, s | |
| 56 | the run before: its phase u8 | as the radio's: on, ready, climb, descent, landed |
| 57–59 | free | zeros |
| 60–63 | CRC-32 | of bytes 0–59 |

## The twelve steps of a start

| # | the step | good when |
|---|---|---|
| 0 | the clocks | the crystal runs |
| 1 | the power | the nPM1300 answers, the cell read |
| 2 | the settings | the table in RRAM, its CRC good |
| 3 | the card | it answers and starts |
| 4 | the file system | mounted, the free space known |
| 5 | the motion sensor | it answers with its own name (WHO_AM_I) |
| 6 | the barometer | the same |
| 7 | the magnetometer | the same |
| 8 | the humidity sensor | its serial number read |
| 9 | the GNSS | it answers on its UART |
| 10 | the radio | the SX1262 answers, its status good |
| 11 | USB | enumerated, if a cable is in; else 7, not reached |

## Over the radio — two additions to `RADIO.md`, into the room it left

| | | |
|---|---|---|
| command **9 LOG_GET** | u8 how many records, 0 all | channel B, signed |
| frame **type 6, a piece of the log** | head 6 B · CRC-32 of the whole 4 B · piece u8 · pieces u8 · up to 3 records 192 B · tag 4 B | 206 B; 64 records are 22 pieces, some 3.6 s at SF7/250 |
