# NIC-Tiros — requirements

Ranges and orders of magnitude only; no parts. What is not known is marked **open**
and is not filled in by guess.

## The uses

| | Baker — the small one | Able — the big one |
|---|---|---|
| carried by | a model rocket, 0.5 m or so, one to three stages; bodies 20–40 mm inside | a model plane, spans up to some 3 m; anything that flies long |
| flight | ~20 s; the motor burns 0.5–3 s; up to some hundreds of metres | long; its length not fixed — what the memory and the cell hold at the rates chosen |
| after it | lands up to some hundreds of metres away (parachute or streamer, the wind), lies in grass | lands by the pilot, or comes down where it should not |

## What is measured

| quantity | Baker | Able | why |
|---|---|---|---|
| acceleration | tens of g in the boost, hundreds of g in the shocks (ejection, staging, landing) — both at once | a few g to some tens of g | the flight, the burn, the apogee |
| rotation | up to some turns a second (canted fins, the roll) | some turns a second | the attitude |
| magnetic field | the Earth's field, for roll and heading | the same | the attitude |
| air pressure | to a height resolution of decimetres over some hundreds of metres | the same, over the flight's height | the apogee, the height |
| air temperature, humidity | the air outside the body, slow sensors: honest under the parachute only | the air outside, in the flow | the air |
| position | before launch and after landing (to be found) | along the flight | the finding; Able's track |
| the unit itself | its cell, its state | the same | |

**The durations are not fixed**: they follow from the rates chosen and
what the memory and the cell hold; the rates are set to the use, up to what the sensors give.

Rates, as orders: motion ~a thousand samples a second, up to several thousand; pressure ~a hundred; temperature and
humidity ~one; position ~one to ten.

## Recording

- **Everything at full rate into the unit's own memory**; the radio is a bonus.
- Baker: one flight with the wait before and the landing after — minutes, order of megabytes.
- Able: long flights, many of them — hours, onto a card, order of gigabytes.
- **Read out after recovery straight from the unit**: over USB if the
  unit has it, over the air (through Ham) if it has a radio fit for it — never over a bare UART
  that wants an adapter. Over LoRa a few megabytes take minutes; Able's gigabytes want USB, a
  fast radio or the card taken out.
- **A boot log:** the last starts kept through power-off — the reset's cause, each step of the
  start with its time and result, the run before's last words; read on the cable and over the
  radio ([`BOOTLOG.md`](BOOTLOG.md)).

## The radio

- **During the flight:** a live view, some tens of packets a second, to some hundreds of metres
  up to about a kilometre in the open, with plain omni antennas; a lost packet costs nothing.
- **After landing:** the position, slowly and robustly, for the unit lying in grass — some
  hundreds of metres; first every few seconds, then every minute.
- Within the Czech and EU rules for short-range devices (868 MHz: power, duty cycle), with no
  licence.
- **Made ready over the radio**, from a safe distance, and answered; the launch found by the unit
  itself, or started over the radio.
- **Set up over the USB cable, or over the same radio on site** — a free channel at low power.
  Baker and Able carry no Wi-Fi and no Bluetooth; Wi-Fi is Ham's alone, for the phone out of
  doors — Ham joining the phone's network, given it over the cable or Bluetooth.
- **Ham** receives Baker and Able and shows them: at home on a computer, out of doors on a phone
  (in a browser, no app); fed from USB or a cell; a socket for an outside antenna.

## Power

- Baker: the wait on the pad, the flight, then reporting its position for days, on a cell of some
  grams.
- Able: hours of recording, then the same reporting.
- **Charged through its one sealed contact** — USB, or any 5 V: a power bank, a
  plane's supply, an add-on module (a solar one among them). No second input; the unit unchanged.

## Size and mass

- Baker: fits a 20 mm body — a board well under 20 mm wide, longer as it must; around ten grams
  with its cell.
- Able: tens of grams; the size secondary.

## Environment

- Baker: the shocks above; the ejection's hot gas and soot kept off by the body (a bulkhead) — the
  unit itself sealed; landing in wet grass, dew, light rain.
- Able: vibration from the motor; rain; a crash — sealed, its one contact sealed;
  the housing itself a recommendation to the builder.
- **Temperature: the electronics −40 to +85 °C** — the industrial range, every part rated for it. Flying in frost is rare (models have
  no de-icing; icing brings a plane down), but a dark body on a pad in the sun runs far above the
  air. **The cell is narrower:** a lithium cell is not charged below 0 °C and gives little in deep
  cold — its own range from its sheet ([`parts.md`](parts.md)).

## Not wanted

- No steering, no guidance of any kind.
- No price limit: the fewest parts, current parts, nothing older than needed.
