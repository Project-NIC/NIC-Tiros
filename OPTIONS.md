# NIC-Tiros — the tree of options

Every block, what was chosen, and what else was weighed. **No figures here:** each lives once —
in [`parts.md`](parts.md), the sums in [`calc/`](calc/), the reasons in [`WHY.md`](WHY.md).
✔ chosen · ○ an alternative that still fits · ✗ set aside.

## The units
- ✔ **Baker** — the small one: a rocket's body of 20–40 mm.
- ✔ **Able** — the big one: planes, long records, a card; size and cell no worry.
- ✔ **Ham** — the receiver on the ground; a bought board, the rest software.
- ✗ a standing weather station — moved to NIC-Heimdall as a thought.

## Processor
- ✔ Nordic nRF54LM20A, the bare chip in QFN52.
- ○ ST STM32U535 / U575 / U585 — lower sleep, SDMMC; full-speed USB only.
- ✗ ST STM32H5 (sleep with RAM), STM32WL (no USB), Espressif C3/C5/C6/C61 (no USB disk, no SD host,
  one SPI), classic ESP32 (no USB), S3 (older), S31 (more than needed).
- ✗ Fanstel's BM20M module — too wide for a 20 mm body, and its 2.4 GHz antenna unused.

## Power
- ✔ Nordic nPM1300; off is its ship mode, woken by a magnet on a Hall switch (as NIC-Strelka's);
  charged through the one sealed contact — USB or any 5 V. Its rails: BUCK1 the
  processor and LoRa, BUCK2 the memory, one load switch the GNSS, the other the sensors.
- ○ a solar add-on module on the same 5 V (an MPPT current source capped at 5.1 V).
- ✗ the nPM1304 (hardly at JLCPCB); a panel straight on the input (cut off above 5.5 V); a
  supercapacitor (a hundredth of the energy).

## Cell
- ✔ LiFePO4: Baker a cylindrical 10440; Able LTOPOW LFP103042 (the Belka's).
- ○ a narrow LiFePO4 pouch for Baker, if offered.
- ✗ lithium-polymer (LiFePO4 for the shocks and its safety).

## Contact
- ✔ **M8, D-coded, four poles, male on the unit, all plastic with brass gold-plated pins** — as
  Heimdall's field units' round connectors; screwed on, potted from inside; one cable for both units.
- ✗ GCT USB4720, a sealed USB-C (not at LCSC or JLCPCB; any phone cable fits, but it is not
  screwed on); ✗ Samtec BRU, Amphenol LTW UC-00PFFP-QS8001.
- ✗ a magnetic contact (its magnets beside the magnetometer); round connectors with their own
  leads (Binder 620 short of −40 °C, Switchcraft EN3, Bulgin PXP).

## Memory
- ✔ Baker: CS's SD NAND, soldered (Strelka's). Able: a microSD socket and ATP's industrial card.
- ✔ **MUF**, Project-NIC's record format ([`MUF.md`](https://github.com/Project-NIC/NIC-SDK/blob/main/MUF.md)), Tiros's profile in [`RECORD.md`](RECORD.md):
  streams at their own rates, everything described in the file, blocks of a sector with a CRC.
- ✔ The ring: the last seconds in RAM, written at the launch (`PRE_S`).
- ✗ a file a sensor; Strelka's version-4 record as it was; ULog on the unit (ULog is an export).

## Motion
- ✔ ST LSM6DSV320X — low and high range at once, gyro.

## Magnetic field
- ✔ Memsic MMC5983MA.
- ✗ PNI RM3100 — lower noise, larger and slower.

## Pressure
- ✔ Bosch BMP585, gel-filled, behind a port to the outside.
- ✗ BMP581 (not gel-filled); Infineon DPS368, ST LPS22DF/LPS28DFW, TE MS5611 — not read.
- ✔ The port: a few holes round the body into one chamber (a static ring), never facing the flow.

## The air
- ✔ Sensirion SHT45-AD1F, on a thin tongue at a hole, its heater against dew and creep.
- ✗ combined chips (BME280, BME688) — a weaker barometer.

## Position
- ✔ u-blox MIA-M10Q; switched off after landing, once a good fix is had.
- ✔ Antenna: Maxtena's passive helix M1516HCT-P — Baker the U.FL version, its coax sealed where it
  leaves the box; Able the SMA version on a bulkhead.
- ○ Johanson 1575AT43A0040001E chip (the fallback); Linx ANT-GNFPC-SHL1 flexible; u-blox SAM-M10Q
  with its patch; small patches on U.FL.
- ✗ an active helix (Ø 18 × 50 mm) with the MIA-M10C; Telit SE868K7-A (obsolete).

## Radio
- ✔ LoRa at 868 MHz: Seeed Wio-SX1262, a module with an IPEX socket; +14 dBm on a LiFePO4 cell.
- ✔ Ham finds a lost unit by its signal: Ham's whip, its own null, or the body's shade; the unit's
  power lowered by command near it.
- ✔ Two channels ([`RADIO.md`](RADIO.md)): the 5 mW one without a duty limit by default for
  everything on the field — the live view, commands, the table of settings; the 500 mW one for
  the beacon after the landing and wherever reach is set.
- ✔ The settings sent as one whole table, from close by (the set, the signal) or on the cable;
  commanding frames signed by the set's key (AES-CCM), given on the cable and never on the air;
  the beacon hidden if set.
- ○ A directional antenna at Ham; an 868 MHz quadrifilar helix in the rocket (Sangshin's RFID one
  is flat, 48 × 40 mm).
- ○ M5Stack Stamp LoRa-1262 I; the bare SX1262.
- ✔ Ham: Seeed's XIAO ESP32-S3 & Wio-SX1262 kit, for simplicity; other SX1262 boards as well.
- ✗ Wi-Fi on the units; 433 MHz (its EU power); the SX1278 (older); Bluetooth and
  Nordic's ESB on 2.4 GHz (lost in grass after landing).
- ✔ Antenna 868 MHz: by length, the builder's choice — from a 17 cm dipole to SMD chips
  ([`parts.md`](parts.md)).

## Housing — a recommendation, not a design
- ✔ Mercury (Baker's, [`pictures/Mercury.jpg`](pictures/Mercury.jpg)) and Gemini (Able's, [`pictures/Gemini.jpg`](pictures/Gemini.jpg)): plastic, the antennas inside or on bulkheads; sealed by
  potting, cold-shrink or glands; the port and the probe behind membranes; the magnetometer far
  from the contact and the cell; some foam for warmth.

## Firmware — Hamilton
- ✔ Zephyr, as NIC-Strelka's; the record as one file; USB as a disk; the power, the magnet and
  the light as Strelka's.
- ✔ One machine of states and one default table ([`HAMILTON.md`](HAMILTON.md)): the launch, the
  top and the landing found by conditions that are settings, each off at 0; START and STOP over
  LoRa beside them.
- ✗ Profiles built into the firmware (a rocket's, a plane's) — a profile is the user's own table.
- ✔ A log of the last 64 starts ([`BOOTLOG.md`](BOOTLOG.md)).
- ✔ Ham's page as the application: HTTP, nothing installed, every system ([`HAM.md`](HAM.md)).
- ○ a position limit stricter than COCOM on what the outputs give, written openly.
