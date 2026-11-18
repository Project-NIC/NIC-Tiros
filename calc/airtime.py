"""NIC-Tiros — the radio frames' time on air. Run: python3 calc/airtime.py
Writes calc/out/airtime.md. The formula: Semtech SX1261/2 datasheet rev 1.2, 6.1.4 (SF7-SF12;
explicit header, CRC on, coding rate 4/5, preamble 8 symbols; the low-data-rate optimisation on
where a symbol lasts 16.38 ms or more, as 6.1.1 advises). The frame sizes: RADIO.md.
"""
import math, os

here = os.path.dirname(os.path.abspath(__file__))
FRAMES = {"beacon": 26, "live": 22, "live + GNSS": 37, "command": 11, "answer": 17, "table piece (200 B)": 216}
MODES = [(7, 250), (7, 125), (9, 125), (12, 125)]
DUTY_S_HOUR = 360     # 10 % of an hour, 869.40-869.65 MHz — the hour as the observation time to be confirmed in EN 300 220
FLIGHT_S = 20

def toa_ms(pl, sf, bw_khz, pre=8, cr=1, crc=1, header=1):
    ts = 2 ** sf / bw_khz                       # ms
    ldro = ts >= 16.38
    n = pre + 4.25 + 8 + math.ceil(max(8 * pl + 16 * crc - 4 * sf + 8 + 20 * header, 0) / (4 * (sf - 2 * ldro))) * (cr + 4)
    return n * ts

out = ["# NIC-Tiros — the frames' time on air (`calc/airtime.py`)", "",
       "Semtech SX1261/2 datasheet rev 1.2, 6.1.4: explicit header, CRC, CR 4/5, preamble 8.", "",
       "| frame | bytes | " + " | ".join(f"SF{s}/{b}" for s, b in MODES) + " |",
       "|---|---|" + "---|" * len(MODES)]
for name, pl in FRAMES.items():
    out.append(f"| {name} | {pl} | " + " | ".join(f"{toa_ms(pl, s, b):.1f} ms" for s, b in MODES) + " |")
out += ["", "## The live view at SF7/250", "",
        "| rate | GNSS in | the channel busy | a 20 s flight takes of the hour's 360 s (10 %) | minutes a hour it may run |",
        "|---|---|---|---|---|"]
for hz in (5, 10, 20, 30):
    for every in (0, 10, 1):
        t = toa_ms(22, 7, 250) if every == 0 else ((every - 1) * toa_ms(22, 7, 250) + toa_ms(37, 7, 250)) / every
        busy = hz * t / 1000
        gn = "never" if every == 0 else ("every frame" if every == 1 else f"every {every}th")
        out.append(f"| {hz} Hz | {gn} | {busy*100:.0f} % | {busy*FLIGHT_S:.1f} s | {DUTY_S_HOUR/busy/60:.1f} |")
pl, t = 450, toa_ms(216, 7, 250)
out += ["", f"A table of {pl} B (the 33 settings of today) is {math.ceil(pl/200)} pieces, {math.ceil(pl/200)*t:.0f} ms in the 5 mW band.",
        f"A beacon at SF12/125 every 60 s: {toa_ms(26,12,125)/600:.1f} % of the time; every 5 s: {toa_ms(26,12,125)/50:.0f} %."]
open(os.path.join(here, "out", "airtime.md"), "w").write("\n".join(out) + "\n")
print("\n".join(out))
