# NIC-Tiros — the processor's pins

For drawing Goddard's board in EasyEDA. The nRF54LM20A in its **QFN52** (6 × 6 mm): 32 GPIO —
P0 six, P1 twenty, P2 six; USB, SWD, the antenna and the 32 MHz crystal on pins of their own.
From its datasheet v1.0, Table 82 and §10.1.2: **every SPI SCK and I2C SCL on a clock pin**, the
data lines on pins close to it; serial instance 00 on P2 alone (the fast SPI), 20–24 on P1, 30 on
P0. **To check against Nordic's QFN52 reference layout** before the board is drawn.

| port, pin | QFN52 pin | what | bus |
|---|---|---|---|
| P2.01 · clock | 1 | memory SCK | SPI00, 32 MHz — the SD NAND or the card |
| P2.02 | 2 | memory SDO (MOSI) | SPI00 |
| P2.04 | 4 | memory SDI (MISO) | SPI00 |
| P2.05 | 5 | memory CS | SPI00 |
| P2.00 | 52 | LoRa NRESET | out |
| P2.03 | 3 | GNSS RESET_N | out |
| P1.03 · clock | 13 | motion SCK | SPI20 |
| P1.02 | 12 | motion SDO | SPI20 |
| P1.01 | 11 | motion SDI | SPI20 (the pin's NFC use left off) |
| P1.00 | 10 | motion CS | SPI20 |
| P1.09 | 24 | motion INT1 | in |
| P1.04 · clock | 14 | LoRa SCK | SPI21 |
| P1.05 | 15 | LoRa MOSI | SPI21 |
| P1.06 | 16 | LoRa MISO | SPI21 |
| P1.31 | 9 | LoRa NSS | SPI21 |
| P1.30 | 8 | LoRa BUSY | in |
| P1.29 | 7 | LoRa DIO1 | in |
| P1.07 · clock | 22 | pressure SCL | I2C22, the barometer alone |
| P1.08 | 23 | pressure SDA | I2C22 |
| P1.18 · clock | 41 | field and air SCL | I2C23, the magnetometer and the humidity sensor |
| P1.16 | 40 | field and air SDA | I2C23 |
| P1.15 | 39 | GNSS TX → its RXD | UART24 |
| P1.19 | 47 | GNSS RX ← its TXD | UART24 |
| P1.24 · clock | 50 | GNSS time pulse (PPS) | in, a timer's capture through GPIOTE20 |
| P1.20, P1.21 | 48, 49 | the 32.768 kHz crystal | XL1, XL2 |
| P0.06 · clock | 27 | power SCL | I2C30, the nPM1300 alone |
| P0.07 · clock | 28 | power SDA | I2C30 |
| P0.08 | 29 | the nPM1300's interrupt | in, GPIOTE30 |
| P0.09 | 30 | pressure INT | in |
| P0.03 · clock | 25 | field INT | in |
| P0.04 · clock | 26 | Able's card-detect, or free | in |
| D+, D− | 19, 20 | USB, the USB4720 | — |
| XC1, XC2 | 37, 38 | the 32 MHz crystal | — |
| ANT | 34 | the radio, unused — terminated as the reference layout says | — |

**Count:** P2 six of six, P1 twenty of twenty, P0 six of six; serial instances seven of seven.
**Not on the processor:** the LEDs and the magnet's Hall switch (the nPM1300's own pins, as NIC-Strelka's
boards); the rails switched by the nPM1300 over I2C — BUCK1 3.0 V for the processor and LoRa,
BUCK2 3.0 V for the memory, load switch 1 for the GNSS, load switch 2 for the sensors
([`parts.md`](parts.md), *The power tree*). The humidity sensor has no interrupt; the GNSS's
EXTINT, not needed, left open.
