# NIC-Tiros — Ham, the receiver on the ground

<p align="center"><img src="pictures/Ham.jpg" width="500" alt="Ham"/></p>

**A bought board and software** (`parts.md`, *Ham*): Seeed's XIAO ESP32-S3 & Wio-SX1262 kit — LoRa
at 868 MHz with an antenna socket, Wi-Fi, a cell and its charger, USB; others as well. Copied from the professionals: the
**radiosonde hunters' ESP32 receivers** (rdzTTGOsonde: a LoRa board that hears a sonde, shows it on
its own web page and a map, and leads the hunter to it), **Meshtastic** and **ExpressLRS** (a LoRa
device serving its own page over its own Wi-Fi). **No app:** the phone's or the computer's
browser shows Ham's page (`requirements.md`).

## What Ham does

- **Hears every frame** of its set on both channels (`RADIO.md`), checks the tags, and **writes
  down every frame heard** with its arrival time and signal into a file of its own — what arrived,
  even what was lost by the unit itself.
- **Commands** — Ham 1 alone, the master; Hams 2–4 hear and write down, the backups.
- **Draws the set:** its number and key, at random; gives them on the cable as `TIROS.SET`,
  offered for download on its page, copied onto the unit's disk.
- **Keeps the user's tables** — the profiles, named ("rocket C6", "glider") — sends one whole to a
  unit (`RADIO.md`, 5) and reads it back by its CRC.
- **Reads the boot log** of a unit (LOG_GET) and shows it.

## One page for every system — nothing to install

**Ham's page is the application:** plain HTML and JavaScript over HTTP,
the same on Linux, Windows, macOS, Android and an iPhone, nothing installed, its files free to
download and open. It leans only on what every browser has — Apple's included: a page, a form, a
file chosen and a file saved. **Not leaned on,** since Safari has none of them: Web Bluetooth, WebUSB,
Web Serial — Web Bluetooth only gives Ham its Wi-Fi where a browser has it, the cable doing the
same everywhere. The unit on its cable is a disk, which every system opens, a phone's too.

**The same page opened from a file,** with no Ham at all: it reads a record or Ham's log from the
disk and turns it into CSV, GPX or ULog for the free tools that already exist — the page manages
the units, shows and converts; the analysis is the free tools'.

**The record shown, as an oscilloscope shows it:** a window of the record — 10 s, or any length
from 1 s to the whole — scrolled from side to side; the chosen values drawn over each other in
colours (the acceleration's axes and its magnitude, the rotation, the pressure, …). Each column of
pixels drawn as the lowest and the highest value it covers, the envelope — Audacity's and the
oscilloscopes' peak detect — so a peak of a millisecond shows in half an hour; zoomed in, the
samples themselves. MUF's index (every 200 blocks) lets the page read only the window it shows,
so a long record scrolls as a short one does.

## The page — five views and the setting-up

**A mockup to click through:** [`ham/mockup.html`](ham/mockup.html) — made-up data, opened from the
file in any browser; the real page grows from it. Laid out for a phone held upright, a computer's
window the same wider: **a bar at the top** — Ham's name, the set, how it is reached (Wi-Fi as
`ham.local`, or the cable), Ham's own cell; **five views at the bottom**, under the thumb.

| view | what it shows | what it does |
|---|---|---|
| **Field** | every unit of the set as a card: its name and number, the phase in colour, the cell, the signal both ways (Ham hears it, it hears Ham), the fix and its age, how long since it was heard | ARM, DISARM, START, STOP, OFF — each unit's and the set's at once; **STOP and OFF held for a second** (gloved, out of doors, a touch in passing ends nothing); the unit's answer beside the button — done, refused, out of range |
| **Flight** | the unit chosen: **the height above the start and the vertical speed in large figures**, the highest, the acceleration, the roll, the frames lost; the height drawn as it comes; the events with their times (launched, apogee — the next file begins, landed); the map when fixes are sent | — |
| **Find** | the arrow or the map (below); the last fix with its kind, satellites, accuracy and age | **copy** the coordinates, **open in maps** (the phone's own map application, which knows where the phone is); BEEP, PING, the power stepped down; a tone rising with the signal |
| **Record** | the oscilloscope (above) over the files of a session — **the files of the phases laid on one time line**, as they join; the channels as coloured chips switched on and off; the events as dashed lines | drag to scroll; pinch or the wheel to zoom the time, with Shift the values; − + on each axis and *whole*; CSV, GPX, ULog |
| **Unit** | its table of settings with each one's range, a value out of it red; whether its CRC is Ham's; its boot log | read the table, send it **whole** (a profile, named — "rocket C6", "glider"); LOG_GET; download the set's `TIROS.SET`, draw a new set (held) |
| *setting up* | Ham's network and its lock — reached from *Unit* | the Wi-Fi's name and password given over the cable or Bluetooth (below); the page's password |

**What a page from Ham cannot do:** Ham serves it over plain HTTP, and a browser lends the phone's
position and Bluetooth only to a secure page (HTTPS, or a file). So **the page shows no distance
from the phone to the unit** — *open in maps* hands the fix to the phone's map application, which
has the phone's position — and **Bluetooth is used from the setting-up page** served over HTTPS
from the project's pages, or opened from a file.

**Its words** in English, Czech and Russian, as the profile's pages; the language chosen on the
page, its tables in Ham.

## The finding — as Apple's *Find My* finds a tag

The simplest, after Apple's precision finding: **Ham held under the phone in one hand,** its
antenna — or the arrow drawn on its box — pointing ahead, and one walks.

- **No compass, the signal alone:** as one turns round with Ham, the signal changes — the body
  shading it from behind, the antenna's own null — and the strongest way is the unit's.
- **The arrow,** a plain blunt one, pointing up the screen where Ham's antenna points; **its length
  and colour the signal's strength**, with *near — far* beside it, the number and a tone whose
  pitch rises with it, as the fox hunters' receivers do. One turns until the arrow is longest, and
  walks. Or **the arrow on the map**, marking the unit's last fix. The two to choose between.
- Near the unit Ham lowers its power step by step (POWER); BEEP for the last metres; PING for a
  quick answer.

**The map,** for getting near — by car, by bike, on foot: **OpenStreetMap's tiles, or ČÚZK's WMS
for Czechia — the orthophoto, the base map (ZTM), ZABAGED's planimetry, and ZABAGED's contours
(close up) and the cadastre over any of them**, switched on the
map, their detail following the zoom by themselves; the map library (Leaflet) carried in Ham;
every fix received set on it by itself, the last one marked. The tiles and the WMS's images come
over the phone's own mobile data — without it, the coordinates alone, to be copied into any map
application to navigate by. ČÚZK's terms of use to be read.

The height and the vertical speed are Ham's, from the pressures (`RADIO.md`, 2); the unit sends
the barometer's own number.

## How a phone or a computer reaches Ham — as NIC-Strelka's Povodok

**Ham joins the user's own network** — the phone's hotspot out of doors, the home Wi-Fi at home —
so the phone keeps its internet and its map; a phone joining a device's own network loses both,
and the systems leave it again. The network's name and password are given to Ham once, by
**either of two ways**:

- **over the USB cable** — Ham a USB network adapter (CDC-NCM) beside its disk, its page opened over
  the cable; the way that holds on every system with a USB network — Povodok's tested on an iPhone
  with USB-C, Android, Windows, macOS, Linux;
- **over Bluetooth LE** — from a browser that has Web Bluetooth (Chrome and Edge on Android,
  Windows, macOS). Web Bluetooth wants a secure page: the setting-up page served over HTTPS from the
  project's own pages, or opened from a file. Safari has none — an iPhone takes the cable.

Then Ham is found on the network as `ham.local` (mDNS), its address told over Bluetooth and on the
cable as well. Ham's ESP32-S3 hears **2.4 GHz only**: a phone's hotspot set to 2.4 GHz (an
iPhone's *Maximise Compatibility*).

## The page's lock — as NIC-Strelka's Povodok

The user's network with its own password (WPA2), and the page's **password, at least twelve
characters, checked by a challenge and its answer**, never sent — over the network and over the
cable alike; Ham keeps its SHA-256.

## Open

- The board — when the units are built.
