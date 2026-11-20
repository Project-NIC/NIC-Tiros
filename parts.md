# NIC-Tiros — parts

Each part named against the requirement it meets. **Sheet:** where its figures were read (the
maker's documents), or **to read** — nothing is built on a part whose
sheet is not read. No pins here ([`PINS.md`](PINS.md)).

## Goddard — Baker and Able

| block | part | package | meets | sheet |
|---|---|---|---|---|
| processor | **Nordic nRF54LM20A** | QFN52, 6 × 6 mm | USB high-speed for the readout; sleep 4.3 µA with all RAM; 7 serial interfaces, 10 clock pins for the buses; −40 to +85 °C | read |
| power | **Nordic nPM1300** | QFN32, 5 × 5 mm | charging from the contact's 5 V (USB or any), 32–800 mA; dynamic power path (a weak source taken as it comes); the rails, the cell's gauge, LEDs, load switches; IQ 0.8 µA | read |
| wake | **TI DRV5032ZE** (as NIC-Strelka's), or the AJ at 9.5 mT for a weaker magnet — a Hall switch, open-drain, on the nPM1300's SHPHLD | SOT-23 / X2SON | a magnet held to the sealed case wakes the unit from the ship mode (370 nA); 1.6 µA typical at 3 V, 20 Hz; no glass to break — a reed (Coto CT05, 100 G) weighed, `WHY.md` | read (NIC-Strelka's sheet) |
| contact | **M8, D-coded, four poles** (IEC 61076-2-104), a panel socket, **male on the unit** — Baker and Able alike, one cable | Ø 8 mm thread; some Ø 10–13 mm over its nut, 15–20 mm deep with its solder cups | USB 2.0 (VBUS, D+, D−, GND) and any 5 V; IP67 mated, capped when not; screwed on; soldered from inside and potted | **all plastic** — body and coupling nut; the pins **brass, gold-plated** or otherwise finished, **no iron or steel** beside the magnetometer (the nickel under the gold a few µm); to be found among the makers' parts |
| motion | **ST LSM6DSV320X** | LGA-14 | ±16 g and ±320 g at once, gyro ±4000 °/s, to 7.68 kHz, FIFO 4.5 KB; 0.8 mA | read |
| magnetometer | **Memsic MMC5983MA** | MMC: LGA 3 × 3 mm; RM3100: three coils + its chip | both ±800 µT. MMC: 40 nT RMS, to 1000 Hz, SET/RESET, −40 to +105 °C. RM3100: 15–30 nT, three axes ~147–533 Hz, −40 to +85 °C | read both |
| pressure | **Bosch BMP585** | LGA-8, 3.25 × 3.25 mm | the apogee: 0.15–0.78 Pa RMS by OSR, to 622 Hz; gel-filled for the wet port; −40 to +85 °C | read |
| air | **Sensirion SHT45-AD1F** (with its membrane) | DFN 1.5 × 1.5 mm | humidity and temperature on the probe, ±1.0 %RH, ±0.1 °C, −40 to 125 °C; τ63 4 s / 2 s; the heater against dew and creep in cloud (Sensirion's note) | read |
| memory, Baker | **CS CSNP1GCR01-BOW** SD NAND, 1 Gbit | LGA-8, 8 × 6 × 0.75 mm | the whole flight; served as a card | read — operating −30 °C, **accepted**: the parts warm each other in an insulated box (`calc/out/numbers.md`, *heat*: some +4 to +19 K), and asleep the memory is switched off, its storage rated to −40 °C; standby < 200 µA, so switched off by a load switch as in Strelka |
| memory, Able | **microSD socket** and **ATP's industrial microSDHC** | — | hours to days; **operating −40 to 85 °C**, non-operating −55 to 90 °C, vibration 6 Grms (its spec v1.0, §3.6) | read |
| position | **u-blox MIA-M10Q** | LGA53, 4.5 × 4.5 mm | position for the finding and Able's track; 4 g / 500 m/s limits; ~12 mA tracking; −40 to +85 °C | read |
| position antenna, Baker | **Maxtena M1516HCT-P-UFL** — fallback **Johanson 1575AT43A0040001E** | helix 33 × Ø 13.2 mm on a U.FL coax, the coax sealed where it leaves the box / chip 7 × 2 mm | circular, no ground plane, 3 g | read — whether it can be bought off a shelf, **to find out** |
| position antenna, Able | **Maxtena M1516HCT-P-SMA** on a bulkhead SMA through the box | Ø 18.5 mm, IP67 with its O-ring | the same, screwed on outside | read |
| radio | **a SX1262 module with an antenna socket** — proposed **Seeed Wio-SX1262** with IPEX, 11.6 × 11 × 2.95 mm, 862–930 MHz, 22 dBm; else M5Stack's Stamp LoRa-1262 I, 14 × 10 × 2.4 mm, its socket IPEX-4 (MHF4) | LGA/stamp | LoRa 868 MHz; the chip's figures from Semtech's sheet: −124 / −137 dBm, 45 mA sending, 5.3 mA listening, 0.6 µA asleep | read — −40 to +85 °C; VCC 1.8–3.6 V, so straight from the LiFePO4 cell or a rail; 125 mA at 22 dBm; asleep 1.62 µA |
| radio antenna | **chosen** (below): Baker in a 20 mm body ETEILY ET-868HCA-5L10C-U, in 30–40 mm or the nose Linx ANT-868-WRT-UFL; Able Linx ANT-868-WRT-UFL inside its box; all on U.FL, nothing soldered | — | the link's margins in `calc/out/numbers.md` | — |
| cell, Able | **LTOPOW LFP103042, LiFePO4** | pouch 10 × 30 × 42 mm, 14 g | 720 mAh at 3.2 V; LiFePO4 for the shocks and its safety; charge 0–45 °C, **discharge −10 to 80 °C** — the cold limit is the cell's, not the electronics' | read (LTOPOW's product page) |
| cell, Baker | **LiFePO4, a cylindrical 10440** — (Ø 10 × 44 mm, AAA-sized, some 200 mAh as sellers give it), a steel can that bears the shocks better than a pouch; a narrow pouch if one is offered (LTOPOW's list has none under 25 mm wide) | — | Baker's energy (`calc/`); its steel can a soft-iron mass, kept from the magnetometer and calibrated | **to choose**, its sheet to read |
| clocks | a 32 MHz crystal (the nRF54LM20 needs it), a 32.768 kHz one | 2016, 2012 | the radio's clock; the time between GNSS fixes | read — below |

## The power tree — proposed, as NIC-Strelka's Belka's and Kryska's

From the sheets: the nPM1300 has **two 200 mA bucks** and **two load switches of 100 mA (or 50 mA
LDOs)**, each switched by the processor over I2C or by the nPM1300's GPIOs (its PS v1.1); the
magnetometer needs **2.8–3.6 V** (MMC5983MA, its specifications), the GNSS **1.76–3.6 V**, the LoRa
module **1.8–3.6 V**, the SD NAND 2.7–3.6 V — so one system rail of **3.0 V**.

| rail | feeds | switched |
|---|---|---|
| BUCK1, 3.0 V | the processor, the LoRa module (asleep 1.62 µA, so left on); the inputs of both load switches | always on |
| BUCK2, 3.0 V | **the memory** — SD NAND or Able's card: a card in SPI mode may draw **up to 100 mA** (ATP's spec, §4.2), more than a load switch should carry | off asleep, as in Strelka |
| load switch 1 (as an LDO or a switch) | **the GNSS** (~15 mA acquiring, its Table 16) | off after the landing's fix, or the GNSS kept in software standby (120 nA at VCC) for a 1 s hot start against a 28 s cold one |
| load switch 2 | the motion, field, pressure and air sensors | off when landed |
| the cell | on a JST SH, three pins — plus, NTC, minus — as Strelka's boards | the charge's end set for LiFePO4 |

At +22 dBm the LoRa module's 125 mA and the rest share BUCK1's 200 mA; at +14 dBm (45 mA) with
room.

## Order numbers — for drawing in EasyEDA

As Strelka's rule: **JLCPCB's basic parts first, then the preferred extended**; a part named by its
maker's number and JLCPCB's (LCSC's) code. Those not found are filled in EasyEDA, which searches LCSC itself; no stock or price
while a concept.
The ICs below are extended parts at best; the passives (0402 resistors and capacitors, the
ferrites) basic ones, a value missing made of basic ones in parallel, a capacitor chosen by what
it keeps at its working voltage (Strelka's rules).

| part | maker's number | from | LCSC |
|---|---|---|---|
| processor | **nRF54LM20A-QGAA-R7** (QFN52, reel; -R the smaller reel) | its datasheet v1.0 | **C52078754** — listed at LCSC, out of stock on 18 Nov 2026; DigiKey NRF54LM20A-QGAA-R7, 16 weeks |
| power | **nPM1300-QEAA-R7** (QFN32) | its PS v1.1 | **C7501206** |
| motion | **LSM6DSV320X** | ST DS14623 | **C26633007** (Strelka's bench) |
| magnetometer | **MMC5983MA** | Memsic rev A | **C404329** (not the MMC5983MA-B, C17241892) |
| pressure | **BMP585**, Bosch sales number 0 273 017 038 | its DS003 | **C18184976** |
| air | **SHT45-AD1F-R2** | Sensirion V7.3 | **C5360602** (JLCPCB's parts list, 18 Nov 2026) |
| position | **MIA-M10Q-00B** (-01 or -02, the ROM's revision; -01B- another) | u-blox R08 | **C6990279** (the 00B) |
| memory, Baker | **CSNP1GCR01-BOW** | CS V1.1 | Strelka's code, in its pages |
| memory, Able | a microSD socket, push-push — a basic part if one is | — | in EasyEDA |
| radio | **Seeed Wio-SX1262**, the IPEX version | Seeed V1.0 | Seeed's own; not at LCSC as far as known |
| contact | **M8 D-coded four-pole male panel socket, all plastic, brass gold-plated pins** — the maker's part to be found against that | the chosen maker's | panel-mounted and hand-wired, not placed by JLCPCB |
| GNSS socket | a U.FL receptacle (Hirose U.FL-R-SMT-1 or its LCSC equal) | — | in EasyEDA |
| cell | **JST SM03B-SRSS-TB** (SH, 3 pins, side entry) | NIC-Strelka's boards | its LCSC equal, as there |
| coils for the bucks | as Strelka's: the MLP2016 power inductor | NIC-Strelka's boards | as there |
| crystal 32 MHz | SMD **2016**, **CL 8 pF**, total tolerance **±40 ppm** | the nRF54LM20 datasheet v1.0, §10.3, the reference BOM (X1) | in EasyEDA |
| crystal 32.768 kHz | SMD **2012**, **CL 9 pF**, total tolerance **±20 ppm** | the same (X2) | in EasyEDA |
| the processor's own DC/DC coil | **4.7 µH** as the reference asks (its L1: 0603, 120 mA, 0.65 Ω) — **in a larger case, lower DC resistance** (the least voltage lost), e.g. 0805–1008 or the MLP2016 class | the same (L1) | in EasyEDA |
| the processor's decoupling | as the reference BOM: 10 µF and 4.7 µF at the supply, 100 nF at each VDD, 2.2 µF at DECRF | the same | basic parts where they are |

## The 868 MHz antenna — chosen

All on the Wio-SX1262's **U.FL** socket, a bought antenna on its lead, nothing soldered.

| where | the antenna | size, lead | figures | why |
|---|---|---|---|---|
| **Baker in a 20 mm body** (inside some 17–18 mm) | **ETEILY ET-868HCA-5L10C-U**, a coil | 32 mm, Ø ~5; 10 cm of 1.13 mm coax, U.FL | 864–868 MHz; "3 dBi" the seller's — a coil is some −3 to −6 dB in truth | the only one that fits and comes on U.FL |
| **Baker in a 30–40 mm body, or in the nose** | **Linx ANT-868-WRT-UFL**, a dipole puck | Ø 19 × 10 mm; 216 mm, U.FL; its counterpoise in the lead, no ground plane | VSWR ≤ 1.4, peak 3.8 dBi, 48 % (its sheet) | the one professional piece with measured figures |
| **Able, in its sealed box** | **Linx ANT-868-WRT-UFL** under the lid | as above | as above | nothing through the wall; **should the reach not do**, Ebyte TX868-JZ-5 outside on an SMA bulkhead (2.0 dBi, SWR ≤ 1.5, its sheet), inside a U.FL–SMA *female* lead |
| **Ham** | the antenna in Seeed's kit (2 dBi, U.FL) | — | — | for finding, a rod on a U.FL–SMA lead, turned by hand |

The pattern in the rocket is not to be decided on paper: the first flight's log measures it — the
signal at Ham against the unit's attitude from its IMU.

## The 868 MHz antenna — the others weighed, by length

The builder chooses by the room in the rocket or the box; each step shorter costs some dB, and the
link has some 40–50 dB to spare (`calc/out/numbers.md`). Figures from makers' and distributors'
pages, their sheets to read before one is named in the guide; efficiencies from different test
boards, compared loosely.

| length | kind | examples | what it costs |
|---|---|---|---|
| 17–18 cm | half-wave rod on an SMA, for Able | Taoglas TI.08.A.0111 (168 mm); Novapart W5017 (179 mm, 0.9 dBi) | as the dipole, outside the box |
| ~17 cm | λ/2 wire dipole, two arms of ~8 cm on the module's IPEX | a wire, or a ready dipole on a U.FL lead | the reference, ~2 dBi, no ground plane |
| 15–16 cm | a ready wire or silicone dipole on IPEX (the drones' receivers') | RadioMaster Y (163 mm, ~1.2 g), RadioMaster and TBS T-dipoles | as the wire dipole |
| ~8 cm | λ/4 wire, the board as its counterweight | a wire | a little less; the small board a poor ground |
| 8 × 1 cm | a flexible printed antenna, stuck inside a non-conducting nose | Molex 211140-0100 (79 × 10 mm, IPEX, 100 mm lead) | — |
| ~5 cm | short rod on an SMA | **Ebyte TX868-JZ-5** (Ø 9 × 50 mm, 6 g, 2.0 dBi, SWR ≤ 1.5, its sheet read); Abracon AEACAC053010-S868 (53 mm, 2 dBi as listed) | — |
| a few cm | a helix (a coiled wire, "spring") | Linx ANT-868-CW-RH-SMA (−1.5 dBi); Linx ANT-868-VHETH (42 %, −3.8 dBi average) | some 3–5 dB |
| ~3 cm | a short rod on its own thin coax and IPEX | Sectron's 868 MHz rod, 30 mm of 1.13 mm coax | — |
| under 2 cm | a stub on an SMA | Linx ANT-868-NUB (average −2.9 dBi) | some 5 dB |
| on the board | SMD chips and loops, with a copper-free end | Pulse ILA.02 loop (10 × 3.2 × 0.5 mm, 60 %); Johanson 0868AT43A0020; Abracon AANI-CH-0211 (12 × 4 × 1.6 mm); Pulse W3016 (keep-out 11.5 × 7 mm); Vishay VJ5301M868 (35 × 5 mm, 1.96 dBi peak); Linx ANT-868-uSP410 (20 %) | some 2–7 dB; tuned on the board by measuring |

A wire need not be straight: it may follow the inside of the body, or be folded or coiled
(meander, helix) where the rocket is short — each fold costs a little and shifts the tuning.

## Ham

**Seeed's XIAO ESP32-S3 & Wio-SX1262 kit** — one bought board, chosen for simplicity: an SX1262 as
the units', a U.FL/IPEX socket for the 868 MHz antenna, Wi-Fi, a cell charged by the board itself
(100 mA), USB-C; Meshtastic runs on it, so it is known in the field. The rest is software
([`HAM.md`](HAM.md)), a box and a good antenna. The S3 is an older chip, but Ham stands beside the
phone: no 5 GHz, no newer Wi-Fi is wanted. **Why not the newer ESP32-C5:** no board was found
with the LoRa built on it sensibly; the S3's kit is bought whole — the power brought to it, the
board put in a neat box, its two antennas (868 MHz and Wi-Fi) clicked on, nothing else done.
**Others will do as well** — new chips bring new
boards (an ESP32-C5 or C6 with an SX1262: LilyGO's T-LoRa C6, M5Stack's Unit C6L, Heltec's
RadioCore RCC6); any board with an SX1262 at 868 MHz, an antenna socket and a cell's charger.

## Open

1. **Baker's 10440:** sellers give some 200 mAh (Keeppower, Orange; a Russian spec 280 mAh at 0.2 C —
   promelec.ru), none a temperature range; a maker's own sheet to be had.
2. The 868 MHz antennas: chosen (above); ETEILY's own sheet still to be had — its figures are the seller's.
3. The small parts — the microSD socket, the SMA bulkheads, the U.FL sockets — by JLCPCB's codes.
   The datasheet's reference circuit is for the CSP98; the QFN52's own reference layout (Nordic's
   product page) is wanted when the board is drawn — for its pins and the unused
   antenna pin's termination.
4. The sheets marked "to read".

## The contact — M8, as NIC-Heimdall's field units

**An M8 D-coded four-pole socket**, the industry's small round connector, as Heimdall's field units
take M8 and M12: cheap, made by many, an O-ring in its face, **soldered from inside and potted**, so
water past the seal stops at the potting. **Four poles are USB 2.0 exactly** — the data on one of the
D-coding's two pairs, **1 D+, 3 D−**, the supply on the other, **2 VBUS, 4 GND** — the record out, the setting up, the charge and any 5 V (a power
bank, the plane's supply). **Screwed on**, it does not shake loose when a plane feeds Able in
flight.

- **D-coded, not A-coded:** the coding keys a connector, not its pin count — a three-pole plug
  enters a four-pole socket of the same coding. Heimdall's probes are on M8 A-coded; Tiros's USB on
  D-coded never meets them, so a laptop's 5 V never reaches a probe's rail.
- **Male on the unit**, the fed side, as Heimdall's gender rule; the cable female; a cap on the unit
  when nothing is plugged.
- **All plastic, nothing magnetic:** the body and the coupling nut plastic, the pins **brass and
  gold-plated** (or otherwise finished) — **no iron, no steel**, the magnetometer being on the same
  board. The part is to be found against that; a stainless body is refused.
- **The speed:** USB full speed goes through any connector — Baker's 128 MB in some 2 min. Able's
  card wants high speed (480 Mbit/s), which the D-coding's shielded pairs and a short cable carry;
  tried on the bench. At full speed alone Able's 8 GB would take some 2½ hours.
- **The room:** Baker's 20 mm body (17–18 mm inside) takes the socket **through an end**, not the
  side; Able's box anywhere.
- **The cable:** M8 D-coded female to USB-C, the project's own — bought as an M8 cordset with open
  ends and a USB-C plug soldered on, or bought whole where offered.
