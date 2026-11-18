"""NIC-Tiros — the record's size. Run: python3 calc/record.py
Writes calc/out/record.md. The streams, their steps a block and rates: RECORD.md; the block of
512 B and the index block after every 200: NIC-SDK's MUF.md; the defaults: SETTINGS.md.
"""
import os
import params as p

here = os.path.dirname(os.path.abspath(__file__))
BLOCK = 512
INDEX_EVERY = 200
# (stream, rate Hz, steps a block) — RECORD.md, "The streams"; the fast stream's rate is the variable
OTHERS = [("slow", 10, 80), ("position", 10, 21), ("information", 1, 28)]
FAST_STEPS = 17

def blocks_per_s(fast_hz):
    b = fast_hz / FAST_STEPS + sum(hz / n for _, hz, n in OTHERS)
    return b * (1 + 1 / INDEX_EVERY)

out = ["# NIC-Tiros — the record's size (`calc/record.py`)", "",
       "| the fast stream | blocks a second | kB/s | Baker's 128 MB | Able's 8 GB | Able's 32 GB |",
       "|---|---|---|---|---|---|"]
for hz in (1000, 500, 200, 100):
    b = blocks_per_s(hz); kbs = b * BLOCK / 1000
    able_h = p.BAKER_MEMORY_MB * 1e6 / (kbs * 1000) / 3600
    out.append(f"| {hz} Hz | {b:.1f} | {kbs:.1f} | {able_h:.1f} h | {8e9/(kbs*1000)/3600:.0f} h | {32e9/(kbs*1000)/3600:.0f} h |")
# a rocket's flight with the default table: 3 s of ring, 8 s up at REC_CLIMB, 60 s down at REC_DESC
ring, up, down = 3, 8, 60
mb = (blocks_per_s(1000) * (ring + up) + blocks_per_s(100) * down) * BLOCK / 1e6
out += ["", f"A rocket's flight with the default table — {ring} s of the ring and {up} s up at 1000 Hz, "
        f"{down} s down at 100 Hz: **{mb:.1f} MB**; Baker holds some {p.BAKER_MEMORY_MB/mb:.0f} such flights."]
open(os.path.join(here, "out", "record.md"), "w").write("\n".join(out) + "\n")
print("\n".join(out))
