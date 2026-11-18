# NIC-Tiros — principle (level 4)

The physics of each block and its rough sums. **Every number is in `calc/`:** the inputs in
`calc/params.py`, each with its source, the results in `calc/out/numbers.md`, made by
`python3 calc/budget.py` and never typed by hand. Inputs marked as assumptions there are to be read
in their sheets.

## The record
All the sensors at full rate make a stream of some tens of kB a second, motion the most of it.
Baker's soldered memory holds an hour of it at the full rate — hundreds of flights
(`calc/out/record.md`); Able's card, days. While ready, the last seconds are kept in a ring in
RAM and written at the launch.

## The readout
The narrow place is the bus to the memory, not the USB: the fast SPI gives a few MB a second; a
full-speed USB would give less than half of it. Baker's memory empties in under a minute, a
gigabyte of Able's in minutes; Able's card in a computer's reader is faster still.

## The radio
LoRa's time on air (Semtech's formula, AN1200.13) sets what can be sent: in the fast mode a live
packet of some tens of bytes takes tens of milliseconds, so some tens a second; in the slow,
robust mode a short position packet takes over a second. A whole flight sent without pause is
within the 10 % band's hour; a position once a minute is too, but not within a 1 % band.
The link in open air leaves tens of dB to spare at a kilometre; lying in grass, an allowance of
some 20 dB is taken from it — to be measured.

## The energy
Landed, the unit sleeps with its RAM kept and wakes once a minute to send its position and listen
a moment: the transmission dominates. At the common EU power Baker's small cell lasts some days,
Able's larger one weeks; at the transceiver's top power, a third of that. Off, in the power
block's ship mode, the cell lasts years. The GNSS's draw, the sensors' in flight and the charging are next.

## The pad and the flight
Ready on the pad with everything running and the radio listening, the unit draws some tens of mA:
half an hour there costs a tenth of Baker's cell; the flight and the descent with the live view,
a few mAh more. **Landed** (`HAMILTON.md`): the impact's spike and then
stillness; the GNSS takes its fix and is switched off, the last position sent from then on. An assumption marked in `calc/params.py`: the memory's write current.

## The pressure port and the apogee
`calc/pressure.py`, its picture `calc/out/apogee.svg` — a sketch, since every rocket and motor
differs. Laminar flow through a hole of 0.5–2 mm into a chamber of a few to a hundred cm³ lags by
microseconds to some milliseconds: the port does not decide the apogee's timing. What does is how
flat the pressure is at the top — within a quarter of a second of it the rocket is only some 0.3 m
lower — against the barometer's noise; fused with the accelerometer, the top is found to some
tenths of a second. The real flow round a hole lags more than the laminar sum; the order stands.

## The contact's magnets and the magnetometer
`calc/magnet.py`, a dipole sketch with an assumed Ø 3 × 2 mm magnet (the real ones asked of the
seller). A fixed offset is calibrated out (hard iron); what is not is its drift with temperature —
NdFeB loses some 0.12 % a kelvin. Within ~15 mm the magnetometer saturates; at 20 mm the offset is
some seven times the Earth's field and its drift over 20 K some 9° of heading; at **50 mm and
beyond, under half a degree**. So the magnetometer at the board's far end from the contact, some
5 cm and more; the pair's opposite poles help a little; the offset calibrated with the unit
assembled; the drift measured on the bench.

## Warmth
Working, a unit dissipates some 0.1–0.2 W; inside a few millimetres of foam that keeps it some
degrees to some twenty above the air (`calc/out/numbers.md`, *heat*) — enough to keep the parts
rated to −30 °C working in −40 °C air. Asleep there is no warmth; the memory is then
switched off and only stored.

## The buses
The shared I2C (the field and the air's humidity) and the barometer's own run at a few per cent
of 400 kHz.

## Done at this level
The record, the readout, the radio and the bands, the energy landed, on the pad and in flight, the
GNSS, the FIFO and the RAM ring, the buses, the pressure port — all in `calc/`.
