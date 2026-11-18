"""NIC-Tiros — the rough sums. Run: python3 calc/budget.py  (writes calc/out/numbers.md).

Every derived number is computed here and nowhere typed.
"""
import math
import os
import params as p


def lora_toa(sf, bw_khz, payload, preamble=p.PREAMBLE, cr=p.CODING_RATE):
    """Time on air, Semtech's formula (AN1200.13): explicit header, CRC on."""
    tsym = (2 ** sf) / (bw_khz * 1000)
    de = 1 if tsym >= 0.016 else 0
    n = 8 + max(math.ceil((8 * payload - 4 * sf + 28 + 16) / (4 * (sf - 2 * de))) * (cr + 4), 0)
    return (preamble + 4.25) * tsym + n * tsym


def fspl_db(d_m, f_mhz):
    return 20 * math.log10(d_m / 1000) + 20 * math.log10(f_mhz) + 32.44


rows = []
def out(section, what, value):
    rows.append((section, what, value))

# memory and data rate
streams = [(p.MOTION_HZ, p.MOTION_BYTES), (p.PRESSURE_HZ, p.PRESSURE_BYTES),
           (p.FIELD_HZ, p.FIELD_BYTES), (p.AIR_HZ, p.AIR_BYTES), (p.POSITION_HZ, p.POSITION_BYTES)]
rate = sum(hz * b for hz, b in streams) * (1 + p.RECORD_OVERHEAD)
out("record", "data rate, all at full rate", f"{rate/1000:.1f} kB/s")
out("record", "per minute", f"{rate*60/1e6:.1f} MB")
out("record", "per hour", f"{rate*3600/1e6:.0f} MB")
out("record", "Baker's memory holds", f"{p.BAKER_MEMORY_MB*1e6/rate/60:.0f} min")
for gb in p.ABLE_CARD_GB:
    out("record", f"Able's {gb} GB card holds", f"{gb*1e9/rate/3600:.0f} h")

# readout
spi = p.CARD_SPI_MHZ * 1e6 / 8 * p.SPI_EFFICIENCY
usb_fs = p.USB_FS_MBIT * 1e6 / 8 * p.USB_EFFICIENCY
out("readout", "card over the fast SPI (the narrow place)", f"{spi/1e6:.1f} MB/s")
out("readout", "1 GB that way", f"{1e9/spi/60:.0f} min")
out("readout", "1 GB over a full-speed USB, for comparison", f"{1e9/usb_fs/60:.0f} min")
out("readout", "Baker's whole memory that way", f"{p.BAKER_MEMORY_MB*1e6/spi:.0f} s")

# radio: airtime
for sf, bw in [(7, 125), (7, 250), (9, 125)]:
    t = lora_toa(sf, bw, p.LIVE_PAYLOAD_B)
    out("radio", f"live packet {p.LIVE_PAYLOAD_B} B, SF{sf}/{bw} kHz", f"{t*1000:.0f} ms → at most {1/t:.0f} a second")
t12 = lora_toa(12, 125, p.BEACON_PAYLOAD_B)
out("radio", f"beacon {p.BEACON_PAYLOAD_B} B, SF12/125 kHz", f"{t12:.2f} s")
out("radio", "beacon once a minute, airtime an hour", f"{t12*3600/p.BEACON_EVERY_S:.0f} s (10 % band: {p.DUTY_10_PERCENT_S_PER_H} s, 1 %: {p.DUTY_1_PERCENT_S_PER_H} s)")
tl = lora_toa(7, 250, p.LIVE_PAYLOAD_B)
out("radio", f"a {p.FLIGHT_S} s flight sent continuously at SF7/250", f"{p.FLIGHT_S:.0f} s of air (10 % band: {p.DUTY_10_PERCENT_S_PER_H} s an hour)")

# radio: link
for d in p.DISTANCE_M:
    loss = fspl_db(d, p.FREQ_MHZ)
    for tx in p.TX_DBM:
        rx = tx + p.ANT_UNIT_DBI + p.ANT_GROUND_DBI - loss
        out("link", f"{d} m, +{tx} dBm: received", f"{rx:.0f} dBm; margin SF7 {rx-p.SENSITIVITY_DBM[7]:.0f} dB, "
            f"SF12 {rx-p.SENSITIVITY_DBM[12]:.0f} dB; lying in grass SF12 {rx-p.GROUND_LOSS_DB-p.SENSITIVITY_DBM[12]:.0f} dB")

for d in p.DISTANCE_M:
    rx = p.LOW_POWER_DBM + p.ANT_UNIT_DBI + p.ANT_GROUND_DBI - fspl_db(d, p.FREQ_MHZ)
    out("link", f"{d} m, +{p.LOW_POWER_DBM} dBm in {p.LOW_POWER_BAND} (no duty limit)",
        f"{rx:.0f} dBm; margin SF7 {rx-p.SENSITIVITY_DBM[7]:.0f} dB")


# the 5 mW channel's reach: free space, and the plane earth (two rays: L = 40 log d - 20 log h1 h2)
lam = 299.792458 / p.FREQ_MHZ
allowed = p.LOW_POWER_DBM + 2.15 + p.ANT_GROUND_DBI - p.SENS_SF7_250 - p.MARGIN_DB   # e.r.p. → e.i.r.p.
for sf, sens in ((7, p.SENS_SF7_250), (12, p.SENSITIVITY_DBM[12])):
    a = allowed + p.SENS_SF7_250 - sens
    free = 10 ** ((a - 20 * math.log10(p.FREQ_MHZ) + 27.55) / 20)
    out("reach 5 mW", f"SF{sf}, {p.MARGIN_DB} dB margin, free space", f"{free/1000:.0f} km — only up in the air")
    for h1 in p.HEIGHTS_M:
        h2 = 1.0
        d = 10 ** ((a + 20 * math.log10(h1 * h2)) / 40)
        out("reach 5 mW", f"SF{sf}, the unit at {h1} m, Ham at {h2} m, flat open ground", f"{d:.0f} m")

for d in p.DISTANCE_M + (200,):
    rx = p.BLE_TX_DBM + p.ANT_UNIT_DBI + p.ANT_GROUND_DBI - fspl_db(d, p.BLE_FREQ_MHZ)
    m = {k: rx - v for k, v in p.BLE_SENS_DBM.items()}
    out("link 2.4", f"{d} m, nRF54 at +{p.BLE_TX_DBM} dBm", f"{rx:.0f} dBm; margin Coded {m['LE Coded 125 kbit/s']:.0f} dB, "
        f"1M {m['LE 1 Mbit/s']:.0f} dB; lying in grass Coded {m['LE Coded 125 kbit/s']-p.BLE_GROUND_LOSS_DB:.0f} dB")

# energy when landed
for tx in p.TX_DBM:
    per_beacon_mas = p.LORA_TX_MA[tx] * t12 + p.LORA_RX_MA * p.LISTEN_S
    avg_ua = (p.MCU_SLEEP_UA + p.PMIC_QUIESCENT_UA + p.RADIO_SLEEP_UA) + per_beacon_mas * 1000 / p.BEACON_EVERY_S
    for unit, mah in p.CELL_MAH.items():
        out("energy", f"landed, beacon/min at +{tx} dBm, {unit} {mah} mAh",
            f"{avg_ua:.0f} µA average → {mah*1000/avg_ua/24:.0f} days")
sleep_ua = p.MCU_SLEEP_UA + p.PMIC_QUIESCENT_UA + p.RADIO_SLEEP_UA
out("energy", "on, asleep between packets", f"{sleep_ua:.1f} µA → Baker's cell lasts {p.CELL_MAH['Baker']*1000/sleep_ua/24/365:.1f} years")
off_ua = p.SHIP_UA + p.HALL_UA
out("energy", "off: the ship mode and the Hall switch", f"{off_ua:.2f} µA → Baker's cell lasts {p.CELL_MAH['Baker']*1000/off_ua/24/365:.0f} years, its own self-discharge apart")

# position: the pad, the flight and the descent (GNSS on, tracking)
gnss_mah = (p.GNSS_ACQ_MA * p.GNSS_TTFF_COLD_S / 3600
            + p.GNSS_TRACK_MA * (p.PAD_WAIT_MIN + p.DESCENT_MIN) / 60)
out("energy", f"GNSS: a cold fix, {p.PAD_WAIT_MIN} min on the pad, {p.DESCENT_MIN} min of descent",
    f"{gnss_mah:.1f} mAh — {gnss_mah/p.CELL_MAH['Baker']*100:.0f} % of Baker's cell")

# the pad and the flight
pad_ma = (p.GNSS_TRACK_MA + p.LORA_RX_MA + p.IMU_MA + p.MCU_ACTIVE_MA
          + p.MEMORY_WRITE_MA * p.MEMORY_WRITE_SHARE)
fly_ma = (p.GNSS_TRACK_MA + p.LORA_TX_MA[14] * p.LIVE_SHARE + p.IMU_MA + p.MCU_ACTIVE_MA
          + p.MEMORY_WRITE_MA * p.MEMORY_WRITE_SHARE)
out("energy", f"ready on the pad, all running, listening", f"{pad_ma:.0f} mA → {p.PAD_WAIT_MIN} min = {pad_ma*p.PAD_WAIT_MIN/60:.0f} mAh")
out("energy", f"flight and descent, live view at +14 dBm", f"{fly_ma:.0f} mA → {p.FLIGHT_AND_DESCENT_MIN} min = {fly_ma*p.FLIGHT_AND_DESCENT_MIN/60:.1f} mAh")
out("energy", "one flight in all, of Baker's cell", f"{(pad_ma*p.PAD_WAIT_MIN+fly_ma*p.FLIGHT_AND_DESCENT_MIN)/60/p.CELL_MAH['Baker']*100:.0f} %")

# Able on a long flight: everything recorded, the GNSS on, a short packet once a minute
t16 = lora_toa(9, 125, p.ABLE_PACKET_B)
pkt_ua = (p.LORA_TX_MA[14] * t16 + p.LORA_RX_MA * p.LISTEN_S) * 1000 / p.ABLE_PACKET_EVERY_S
rec_ma = p.GNSS_TRACK_MA + p.IMU_MA + p.MCU_ACTIVE_MA + p.MEMORY_WRITE_MA * p.MEMORY_WRITE_SHARE + pkt_ua / 1000
out("energy", f"Able recording all, GNSS on, {p.ABLE_PACKET_B} B (SF9) a minute", f"{rec_ma:.0f} mA → {p.ABLE_LONG_MAH} mAh lasts {p.ABLE_LONG_MAH/rec_ma:.0f} h")
out("energy", "the same with the live view sending all the time", f"{fly_ma:.0f} mA → {p.ABLE_LONG_MAH/fly_ma:.0f} h")

# self-heating in an insulated box: ΔT = P · t / (k · A)
for unit, area in p.BOX_AREA_CM2.items():
    for mode, ma in (("ready", pad_ma), ("flight", fly_ma)):
        pw = ma / 1000 * p.CELL_V
        rises = ", ".join(f"{t} mm: +{pw * (t/1000) / (p.INSULATION_K * area/1e4):.0f} K" for t in p.INSULATION_MM)
        out("heat", f"{unit}, {mode}, {pw*1000:.0f} mW inside foam", rises)
out("heat", "landed, asleep", "µW — no warmth; the memory is switched off then, rated to −40 °C in storage")

# the shared I2C bus
bits = lambda hz, b: hz * (b + p.I2C_READ_OVERHEAD_B) * 9
load = bits(p.FIELD_HZ, p.FIELD_BYTES) + bits(p.AIR_HZ, p.AIR_BYTES)
out("buses", "shared I2C: magnetometer + humidity", f"{load/p.I2C_HZ*100:.0f} % of {p.I2C_HZ//1000} kHz")
out("buses", "barometer alone", f"{bits(p.PRESSURE_HZ, p.PRESSURE_BYTES)/p.I2C_HZ*100:.0f} % of its bus")

# the motion sensor's FIFO and the RAM ring
fifo_rate = p.MOTION_HZ * p.IMU_WORDS_PER_SAMPLE * p.IMU_FIFO_WORD_BYTES
out("buffers", "motion FIFO fills in", f"{p.IMU_FIFO_BYTES/fifo_rate*1000:.0f} ms at {p.MOTION_HZ} Hz "
    f"— the processor reads it every {p.MCU_READ_LATENCY_S*1000:.0f} ms at worst, the card's pauses never reach it")
for what, t in p.CARD_PAUSE_S.items():
    out("buffers", f"RAM ring for a card pause, {what}", f"{rate*t/1000:.0f} kB (double it for writing in large pieces)")

os.makedirs(os.path.join(os.path.dirname(__file__), "out"), exist_ok=True)
lines = ["# NIC-Tiros — the numbers", "",
         "Generated by `calc/budget.py` from `calc/params.py`; never edited by hand.", "",
         "| | what | value |", "|---|---|---|"]
lines += [f"| {a} | {b} | {c} |" for a, b, c in rows]
open(os.path.join(os.path.dirname(__file__), "out", "numbers.md"), "w").write("\n".join(lines) + "\n")
print("\n".join(lines))
