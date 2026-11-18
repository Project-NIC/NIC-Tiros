# NIC-Tiros — why things changed

## 15 Nov 2026 — the purpose changed: the standing station out, Able the long recorder

The project had wandered: Baker is the small unit for rockets and the like; Able the bigger one, Strelka's pattern, with a card for long records in planes;
Ham the receiver. The standing station in an orchard goes to NIC-Heimdall as its outside unit
(a thought kept for it). Any such device ends the same way: a small one
for a mouse and a big one for an elephant.

**What stops holding:** the purpose's station (outdoors for months, the weather, the gardener)
and Able's sun and outside-sensor connector. Nothing below the purpose was begun; the station's
thoughts were set apart.

## 15–16 Nov 2026 — why this hardware: what was weighed and set aside

Every figure names its maker's document where it is used.

- **The rocket's own radio link over Wi-Fi and directional antennas** — at 500 m the free-space
  loss is small; what loses a link is the antennas' nulls (a wire along the rocket is blind along
  its axis, a vertical antenna blind straight up). LoRa at 868 MHz with plain antennas has tens
  of dB to spare (`calc/out/numbers.md`).
- **433 MHz** — the EU band allows ~10 mW there against 500 mW at 869.40–869.65 MHz, and its
  antenna is twice as long; the older SX1278 serves 433 MHz only and draws more.
- **Bluetooth and Wi-Fi on Baker and Able** — a short reach where one should stand far from a
  rocket or a plane being started; the start, the setting up and the position go over LoRa, the
  log over USB. Wi-Fi is Ham's alone, for the phone.
- **Espressif's C5, C6, C61, C3** (their sheets) — USB only as a serial port, never a disk; no SD
  host; one general SPI to share. **The classic ESP32** — no USB at all. **The S3** — older. **The
  S31** — everything, but far more than a unit asleep on a field needs. Ham, a receiver beside the
  phone, takes a bought S3 board after all (18 Nov 2026), for simplicity.
- **One-chip LoRa (STM32WL)** — no USB: the log could leave only over a UART and an adapter.
- **The STM32H5** (its sheet, Table 33) — asleep with its RAM kept 0.15–0.27 mA, fifty to a
  hundred times the U5's; USB full-speed only.
- **The STM32U535 / U585** (their sheets) — sound: asleep with the RAM 2.2 / 4.3 µA, USB, SDMMC,
  the same 7 × 7 mm. Set aside for the **nRF54LM20** (its sheet): a smaller package (QFN52,
  6 × 6 mm), a high-speed USB — the readout bounded by the card's bus, some three times the U5's
  full-speed (`calc/out/numbers.md`) — 512 KB of RAM, Zephyr with the drivers for every block
  (checked in Zephyr's tree, 16 Nov 2026), and a toolchain and a power chip (the nPM1300) already
  brought up on the bench; its radio unused, which costs nothing asleep. Its seven serial
  interfaces and ten clock pins were counted against the blocks before it was chosen
  in the parts.
- **Modules for the processor** — Strelka's 10 × 14 mm module and its antenna's clearance would
  not fit a 20 mm body: the chip goes on the board bare.
- **The standing weather station** — a different use; it went to NIC-Heimdall (above).

## 16 Nov 2026 — the requirements changed: Able powered from outside on long flights

Asked: for long flights the bigger unit should take power from outside — a solar panel.
**What stops holding:** the requirements' power ("charged from USB" alone); the architecture's
power block (an input from outside); the energy sums for Able. The plane's own 5 V added beside
the panel, where a plane has it. Baker unchanged.
**Closed again the same day**: Able, sealed, takes **any 5 V through its
one sealed contact** — not only a USB host — and no second input;
the sun is no requirement of Tiros — a solar add-on module may feed that input, and balloons that
float for weeks are another device.

- **An active GNSS helix (Ø 18 × 50 mm)** — the passive Maxtena M1516HCT is smaller (Ø 13.2 ×
  33 mm as the U.FL one), needs no feed nor current, and its SMA version screws onto a sealed box;
  the active one is set aside with the MIA-M10C it wanted. **A cable through the box's wall** —
  harder to seal than a bulkhead SMA. **The Linx flexible antenna** — linear and wider.

- **PNI's RM3100** — lower noise (15–30 nT against 40), but three coils and a chip against a
  3 × 3 mm package, and a third of the speed on three axes; the MMC5983MA is good enough and
  undoes a strong field's mark by its SET/RESET.

## 18 Nov 2026 — the architecture's states detailed and changed

`architecture.md` had the launch given by "start" with the motion as a fallback. `HAMILTON.md`
makes **the launch, the top and the landing the unit's own finding**, each condition a setting
that 0 switches off, START and STOP beside them. **No profiles in the firmware:** one machine and
one default table; a rocket's or a plane's way is the user's own table. **The radio's channels:**
the low-power one by default for everything on the field, the 500 mW one for the beacon and for
reach. **A stored unit listening** on the radio now and then was drafted and **struck the same
day**: no one leaves a unit asleep to send and hear — **stored is off, as NIC-Strelka's**, the
nPM1300's ship mode, 370 nA, woken by a magnet on a Hall switch or by the cable; once woken, the
unit is on and hears ARM. What stops holding below: nothing drawn yet.

- **A reed switch for waking** (Coto CT05, 6 mm of glass; Standex MK24) — it draws nothing where
  the Hall switch draws 1.6 µA, but against the ship mode's 370 nA both last years and the cell's
  own self-discharge is the larger; the reed's glass is rated 100 G (CT05) or 30 g for 11 ms
  (MK24), which a rocket's landing may pass, while the Hall switch is solid. **The Hall switch
  kept,** as NIC-Strelka's; a cheap board's own leakage would swallow the difference anyway. The
  nPM1300's 10 s hold on SHPHLD is left on, as NIC-Strelka leaves it: the
  rescue of a unit that hangs in its sealed case.

## 18 Nov 2026 — the units' names turned round

Able, the rhesus monkey of the Jupiter flight of 28 May 1959, was the bigger; Baker, the squirrel
monkey, the smaller. The names had been given the other way round. **Able is now the big unit**
— the plane's, the long records on a card, in Gemini, the two-seat capsule — **and Baker the small
one**, the rocket's, in Mercury, the one-seat. Every page, script and sum turned with them; the
boards, the parts and the firmware are as they were.

*Any resemblance to NIC-Strelka is purely coincidental.*
