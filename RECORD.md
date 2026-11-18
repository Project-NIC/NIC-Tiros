# NIC-Tiros — the record: its profile in MUF

Tiros writes **MUF**, Project-NIC's format ([`MUF.md`](https://github.com/Project-NIC/NIC-SDK/blob/main/MUF.md), its core); this is Tiros's profile —
its streams, its groups, its fields. The mark and the extension **`MUFT`**; a file
`S00007T01.MUFT`, the session, then the unit's number.

## The streams

| kind | stream | rate | steps a block | a step | a block every |
|---|---|---|---|---|---|
| 0x10 | **fast data** — the motion, the field, the pressure | as set per phase (`REC_CLIMB`, `REC_DESC`, …): **1000 Hz** and **100 Hz** by default | 17 | 28 B | 17 ms at 1000 Hz |
| 0x11 | **slow data** — the air and the barometer's temperature | 10 Hz | 80 | 6 B | 8 s, written at least every 3 s |
| 0x12 | **position** — the GNSS's fixes | 10 Hz | 21 | 22 B | 2.1 s |
| 0x13 | **events** — irregular | — | 96 | 5 B | at each event |
| 0x05 | **information** — the device and the time | 1 Hz | 28 | 17 B | 28 s, written at least every 3 s |
| 0x06 | **constants** — the magnetometer's calibration | once | 1 | 48 B | after the head |

Every sensor runs on its own clock; the firmware filters each onto its stream's grid, and lays the
grids in the GNSS's time — it measures its own clock against the GNSS's pulse and places the steps
on true seconds (in software, no PLL); without a fix they run on the crystal, the head's byte 23
saying which.

## The groups — 22: eight in the head, thirteen and one in two continuations

One group, one unit: latitude and longitude apart from the height.

| name | stream | first, count | type (letter) | unit | scale: mantissa × 10^exp10 | what |
|---|---|---|---|---|---|---|
| `ACC` | 0x10 | 0, 3 | `h` | g | 488 × 10⁻⁶ | the accelerometer, ±16 g |
| `ACC_HG` | 0x10 | 3, 3 | `h` | g | 10417 × 10⁻⁶ | the high-g channel, ±320 g |
| `GYRO` | 0x10 | 6, 3 | `h` | dps | the sheet's, by range | the gyroscope |
| `MAG` | 0x10 | 9, 3 | `h` | T | 25 × 10⁻⁹ | the magnetometer, 0.25 mG |
| `PRESS` | 0x10 | 12, 1 | `i` | Pa | 15625 × 10⁻⁶ | the pressure, 1/64 Pa |
| `PTEMP` | 0x11 | 0, 1 | `h` | °C | 1 × 10⁻² | the barometer's temperature |
| `RH` | 0x11 | 1, 1 | `h` | % | 1 × 10⁻² | the humidity |
| `AIR_T` | 0x11 | 2, 1 | `h` | °C | 1 × 10⁻² | the air's temperature |
| `GNSS_LL` | 0x12 | 0, 2 | `i` | deg | 1 × 10⁻⁷ | latitude, longitude, 10⁻⁷ deg |
| `GNSS_H` | 0x12 | 2, 1 | `i` | m | 1 × 10⁻³ | height, mm |
| `GNSS_VEL` | 0x12 | 3, 3 | `h` | m/s | 1 × 10⁻² | north, east, down |
| `GNSS_FIX` | 0x12 | 6, 4 | `B` | — | — | the fix's kind (0 none, 2 2D, 3 3D), the satellites, the accuracy (dm, 255 worse), the age (10 ms) |
| `T_STEP` | 0x13 | 0, 1 | `I` | — | — | the fast stream's step the event fell in |
| `EVENT` | 0x13 | 1, 1 | `B` | — | — | 1 armed, 2 launched, 3 apogee, 4 landed, 5 a command received, 6 the heater on, 7 off, 8 the fix had, 9 the fix lost |
| `IMU_T` | 0x05 | 0, 1 | `h` | °C | 1 × 10⁻² | the IMU's temperature |
| `CELL` | 0x05 | 1, 1 | `H` | V | 1 × 10⁻³ | the cell, mV |
| `RSSI` | 0x05 | 2, 1 | `b` | dBm | 1 × 10⁰ | Ham's last signal, −128 none |
| `TIME_UTC` | 0x05 | 3, 1 | `I` | s | 1 × 10⁰ | the last whole second of the GNSS, 0 no fix |
| `TIME_STP` | 0x05 | 4, 1 | `I` | — | — | the fast stream's step its pulse fell in |
| `TIME_NS` | 0x05 | 5, 1 | `i` | s | 1 × 10⁻⁹ | its place in that step |
| `MAG_OFF` | 0x06 | 0, 3 | `i` | T | 1 × 10⁻⁹ | the magnetometer's offset, nT — its hard iron |
| `MAG_MAT` | 0x06 | 3, 9 | `i` | — | 1 × 10⁻⁶ | its matrix ×10⁻⁶, row by row — its soft iron |

## The set's table — Tiros's fields

CSV: `kind` (rocket, plane, drone, …), `name`, `motor`, `mass`, `place`, `notes`.

## The end — Tiros's reasons, after the core's

16 landed and reported, 17 the launch never came (a ready session given up).

## Before the launch

Not the format's matter but the firmware's ([`HAMILTON.md`](HAMILTON.md)): the last seconds kept in
a ring in RAM (`PRE_S`, 3 s by default); at the launch written first, their steps continuing; the
file stays straight.

## The sizes

Computed by `calc/record.py` (`calc/out/record.md`): at 1000 Hz some 60 blocks a second, **~31 kB/s**,
Baker's 128 MB some **1.2 hours** of it; at 200 Hz ~6.4 kB/s, some 5.6 hours; Able's card of 8 GB
some 73 hours at 1000 Hz. A rocket's flight with the default table — the ring, 8 s up at 1000 Hz,
a minute down at 100 Hz — is **some 0.5 MB**.
