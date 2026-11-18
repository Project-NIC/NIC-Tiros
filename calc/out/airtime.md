# NIC-Tiros — the frames' time on air (`calc/airtime.py`)

Semtech SX1261/2 datasheet rev 1.2, 6.1.4: explicit header, CRC, CR 4/5, preamble 8.

| frame | bytes | SF7/250 | SF7/125 | SF9/125 | SF12/125 |
|---|---|---|---|---|---|
| beacon | 26 | 30.8 ms | 61.7 ms | 205.8 ms | 1646.6 ms |
| live | 22 | 28.3 ms | 56.6 ms | 205.8 ms | 1482.8 ms |
| live + GNSS | 37 | 41.1 ms | 82.2 ms | 267.3 ms | 1974.3 ms |
| command | 11 | 20.6 ms | 41.2 ms | 144.4 ms | 1155.1 ms |
| answer | 17 | 25.7 ms | 51.5 ms | 164.9 ms | 1318.9 ms |
| table piece (200 B) | 216 | 171.6 ms | 343.3 ms | 1086.5 ms | 7872.5 ms |

## The live view at SF7/250

| rate | GNSS in | the channel busy | a 20 s flight takes of the hour's 360 s (10 %) | minutes a hour it may run |
|---|---|---|---|---|
| 5 Hz | never | 14 % | 2.8 s | 42.4 |
| 5 Hz | every 10th | 15 % | 3.0 s | 40.6 |
| 5 Hz | every frame | 21 % | 4.1 s | 29.2 |
| 10 Hz | never | 28 % | 5.7 s | 21.2 |
| 10 Hz | every 10th | 30 % | 5.9 s | 20.3 |
| 10 Hz | every frame | 41 % | 8.2 s | 14.6 |
| 20 Hz | never | 57 % | 11.3 s | 10.6 |
| 20 Hz | every 10th | 59 % | 11.8 s | 10.1 |
| 20 Hz | every frame | 82 % | 16.4 s | 7.3 |
| 30 Hz | never | 85 % | 17.0 s | 7.1 |
| 30 Hz | every 10th | 89 % | 17.7 s | 6.8 |
| 30 Hz | every frame | 123 % | 24.7 s | 4.9 |

A table of 450 B (the 33 settings of today) is 3 pieces, 515 ms in the 5 mW band.
A beacon at SF12/125 every 60 s: 2.7 % of the time; every 5 s: 33 %.
