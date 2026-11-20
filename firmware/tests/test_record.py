#!/usr/bin/env python3
"""NIC-Tiros — a flight through Hamilton's machine written as Tiros's record with NIC-SDK's writer,
read back with NIC-SDK's reader: python3 tests/test_record.py [SDK]  (SDK: a checkout of
Project-NIC/NIC-SDK, 0.2 or later; by default NIC-SDK beside NIC-Tiros)."""
import os, subprocess, sys, tempfile
here = os.path.dirname(os.path.abspath(__file__))
sdk = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else os.path.join(here, "..", "..", "..", "NIC-SDK"))
sys.path.insert(0, os.path.join(sdk, "tools"))
import muf

c = os.path.join(here, "..", "common")
tmp = tempfile.mkdtemp()
exe, path = os.path.join(tmp, "fly"), os.path.join(tmp, "S00042T01.MUFT")
subprocess.check_call(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-I", os.path.join(sdk, "muf"), "-o", exe,
                       os.path.join(here, "fly.c"), os.path.join(sdk, "muf", "muf.c")] +
                      [os.path.join(c, f) for f in ("record.c", "hamilton.c", "settings.c", "crc32.c")] + ["-lm"])
subprocess.check_call([exe, path])
f = muf.read(path)
fails = 0
def check(ok, what):
    global fails
    if not ok:
        fails += 1
        print("FAIL", what)

check(f["mark"] == "MUFT" and f["name"] == "baker1" and f["session"] == 42, "the head: MUFT, the unit's name")
check(sum(len(s["groups"]) for s in f["streams"]) == 22, "RECORD.md's 22 groups")
fast = f["steps"][0x10]
check(len(fast) == 80000 and fast[-1][0] == 79999, "the fast stream whole, 80 s at 1 kHz")
s10 = next(s for s in f["streams"] if s["kind"] == 0x10)
pa = muf.physical(s10, fast[0][1])["PRESS"]
check(abs(pa - 101325) < 1, "the pressure on the pad, 101 325 Pa (%.1f)" % pa)
ev = {v[1]: v[0] for _, v in f["steps"][0x13]}
print("  events:", ev)
check(ev.get(1) == 10 and ev.get(2) == 3000, "armed, then launched back-dated to 3.000 s")
check(7000 < ev.get(3, 0) < 8000 and 30000 < ev.get(4, 0) < 50000, "the top and the landing")
gnss = f["steps"][0x12]
check(len(gnss) == 800, "the position at 10 Hz")
s12 = next(s for s in f["streams"] if s["kind"] == 0x12)
top = max(muf.physical(s12, v)["GNSS_H"] for _, v in gnss)
print("  the top by the GNSS: %.1f m above the sea" % top)
check(380 < top < 400, "the top some 88 m over the pad at 300 m")
info = f["steps"][0x05]
check(len(info) == 80 and info[10][1][3] == 1763500010, "the time's ties once a second")
check(f["end"]["fast_steps"] == 80000, "the end")
print("%d blocks; %s" % (os.path.getsize(path) // 512, "all good" if not fails else "%d failed" % fails))
sys.exit(1 if fails else 0)
