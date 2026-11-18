"""NIC-Tiros — the consistency check. Run: python3 tools/check.py   (from the project's root)
Every byte table counts on from where the last row ended and reaches the size its heading names;
every setting the pages name is in SETTINGS.md; calc/airtime.py's frames are RADIO.md's; every
link in the pages leads to a file. Prints what is wrong; exits 1 if anything is.
"""
import os, re, sys

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
pages = sorted(f for f in os.listdir(root) if f.endswith(".md"))
text = {f: open(os.path.join(root, f), encoding="utf-8").read() for f in pages}
bad = []

def tables(md):
    """(heading, [first cells]) for each table whose first column is 'bytes'."""
    out, head, rows, on = [], "", [], False
    for line in md.splitlines():
        if line.startswith("#"):
            head = line
        if line.startswith("| bytes |"):
            on, rows = True, []
            continue
        if on and line.startswith("|"):
            if not line.startswith("|---"):
                rows.append(line.split("|")[1].strip())
            continue
        if on:
            out.append((head, rows)); on = False
    if on:
        out.append((head, rows))
    return out

# 1. the byte tables
for f in ("RADIO.md", "BOOTLOG.md"):
    for head, rows in tables(text[f]):
        nxt, open_end = None, False
        for c in rows:
            if c.startswith("last"):
                n = int(c.split()[1]); nxt = (nxt or 0) + n if not open_end else None
                continue
            m = re.fullmatch(r"(\d+)(?:–(\d+|…))?", c)
            if not m:
                bad.append(f"{f}: {head}: a row I cannot read: {c!r}"); continue
            a = int(m.group(1)); b = m.group(2)
            if nxt is not None and a != nxt and not open_end:
                bad.append(f"{f}: {head}: row {c} begins at {a}, the row before ended at {nxt - 1}")
            if b == "…":
                open_end, nxt = True, None
            else:
                nxt = (int(b) if b else a) + 1
        sizes = re.findall(r"(\d+) B\b", head)          # the first for a frame, the last for its tail
        if sizes and nxt is not None and not open_end:
            want = int(sizes[0] if rows and rows[0].startswith("0") else sizes[-1])
            if want != nxt:
                bad.append(f"{f}: {head}: the heading says {want} B, the table makes {nxt} B")

# 2. the settings named in the pages
settings = set(re.findall(r"^\| `([A-Z][A-Z0-9_]+)` \|", text["SETTINGS.md"], re.M))
known = {"PARAMS", "TIME_UTC", "TIME_STP", "TIME_NS", "T_STEP", "EVENT", "PRESS", "PTEMP", "CELL",
         "RSSI", "IMU_T", "GNSS_FIX", "WHO_AM_I", "RESETREAS", "BOOT", "K_ERR_", "TABLE_GET",
         "LOG_GET", "PARAM_SET", "PARAM_VALUE", "LICENSE"}
for f in ("README.md", "RADIO.md", "HAMILTON.md", "BOOTLOG.md", "SETTINGS.md"):
    for name in set(re.findall(r"`([A-Z][A-Z0-9_]{2,})`", text[f])):
        if name not in settings and name not in known and not name.startswith("K_ERR"):
            bad.append(f"{f}: `{name}` is not in SETTINGS.md")

# 3. calc/airtime.py's frames against RADIO.md's headings
air = open(os.path.join(root, "calc", "airtime.py"), encoding="utf-8").read()
frames = dict((k, int(v)) for k, v in re.findall(r'"([^"]+)": (\d+)', air))
for name, pat in (("beacon", r"## 1 The beacon[^\n]*?(\d+) B"), ("live", r"## 2 The live view — (\d+) B"),
                  ("live + GNSS", r"## 2 The live view — \d+ B, with the GNSS (\d+) B"),
                  ("answer", r"## 4 The answer[^\n]*?(\d+) B")):
    m = re.search(pat, text["RADIO.md"])
    if not m or frames.get(name) != int(m.group(1)):
        bad.append(f"calc/airtime.py: {name} is {frames.get(name)} B, RADIO.md says {m and m.group(1)}")

# 4. links
for f, md in text.items():
    for target in re.findall(r"\]\(([^)#:]+)\)", md):
        if not os.path.exists(os.path.join(root, target)):
            bad.append(f"{f}: the link to {target} leads nowhere")

print("\n".join(bad) if bad else "all good")
sys.exit(1 if bad else 0)
