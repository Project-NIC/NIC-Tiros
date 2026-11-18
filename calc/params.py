"""NIC-Tiros, level 4 — every input number has its home here, once.

Each value carries where it comes from. "memory" marks a figure not yet read in a sheet: it is to
be confirmed at level 5 before anything is built on it (RULES.md: what is not known is not built).
"""

# --- what is recorded (the requirements' orders; ~4000 a second for motion) ---------
MOTION_HZ = 4000            # requirements.md: up to several thousand a second
MOTION_BYTES = 18           # low-g accel 6 + high-g accel 6 + gyro 6 (three 16-bit axes each)
PRESSURE_HZ = 100           # requirements.md: ~a hundred
PRESSURE_BYTES = 6          # pressure 3 + temperature 3
FIELD_HZ = 200              # a few hundred: the roll, several turns a second (the design's notes)
FIELD_BYTES = 6
AIR_HZ = 1                  # humidity and temperature: ~one
AIR_BYTES = 6
POSITION_HZ = 10            # requirements.md: one to ten
POSITION_BYTES = 32         # time, lat, lon, height, speed, fix, satellites
RECORD_OVERHEAD = 0.10      # a record's type and stamp, as a share of the payload (rough)

# --- memory -----------------------------------------------------------------------------------
BAKER_MEMORY_MB = 128        # the soldered SD NAND, 1 Gbit (as NIC-Strelka's Kryska)
ABLE_CARD_GB = (8, 32)     # cards of 8–32 GB (as NIC-Strelka's bench)
CARD_SPI_MHZ = 32           # the fast SPI, "one HS-SPI up to 32 MHz" (nRF54LM20 datasheet v1.0, p. 1)
SPI_EFFICIENCY = 0.6        # commands, waits, file system — rough
USB_FS_MBIT = 12            # a full-speed USB (STM32U535/U585 sheets), for the comparison only
USB_EFFICIENCY = 0.7        # rough

# --- the radio --------------------------------------------------------------------------------
FREQ_MHZ = 868.0
TX_DBM = (14, 22)           # 14: a common EU setting; 22: the transceiver's top (SX1261/2 datasheet rev 1.2, Table 3-6)
ANT_UNIT_DBI = 0            # a wire in the body
ANT_GROUND_DBI = 2          # a ground-plane or small dipole at Ham
DISTANCE_M = (500, 1000)    # requirements.md: some hundreds of metres to about a kilometre
GROUND_LOSS_DB = 20         # the unit lying in grass — a rough allowance, to be measured
SENSITIVITY_DBM = {         # SX1262, LoRa 125 kHz, Rx boosted (SX1261/2 datasheet rev 1.2, Table 3-8);
    7: -124, 9: -129, 10: -132, 12: -137,   # SF7 and SF12 from the table, SF9 and SF10 between, ~2.5 dB a step
}
LIVE_PAYLOAD_B = 32         # one live-view packet (the design's notes)
BEACON_PAYLOAD_B = 20       # one position packet
PREAMBLE = 8
CODING_RATE = 1             # 4/5
FLIGHT_S = 20               # a flight lasts ~20 s
DUTY_10_PERCENT_S_PER_H = 360   # 869.40–869.65 MHz, 500 mW e.r.p., duty < 10 % (ERC/DEC/(01)04 table;
                                # ČTÚ's VO-R/10/07.2021-8, g7, footnote 6: within any one hour)
DUTY_1_PERCENT_S_PER_H = 36     # the 1 % sub-bands
LOW_POWER_BAND = "869.70–870.00 MHz"   # 5 mW e.r.p., no duty limit (ČTÚ's VO-R/10/07.2021-8, g8)
LOW_POWER_DBM = 7               # 5 mW
SENS_SF7_250 = -121             # SX1261/2 datasheet rev 1.2, Table 3-8
HEIGHTS_M = (0.05, 1.0, 1.5)    # a unit in grass, in a hand, Ham on a tripod
MARGIN_DB = 10

# --- energy -----------------------------------------------------------------------------------
MCU_SLEEP_UA = 4.3          # System ON idle, GRTC on, all 512 KB RAM kept (nRF54LM20 datasheet v1.0, p. 1)
PMIC_QUIESCENT_UA = 0.8     # IQBAT, on the cell, no BUCK load (nPM1300 PS v1.1, Table 4)
RADIO_SLEEP_UA = 1.62       # the Wio-SX1262 module asleep, its TCXO and switch with it (Seeed's sheet V1.0)
LORA_TX_MA = {14: 45, 22: 125}   # 14: SX1262 optimal settings (Semtech rev 1.2, Table 3-6); 22: the Wio module (Seeed V1.0)
LORA_RX_MA = 5.3            # LoRa 125 kHz, Rx boosted, DC-DC (SX1261/2 datasheet rev 1.2, Table 3-5)
LISTEN_S = 0.1              # the window after each packet (architecture.md)
SHIP_UA = 0.37              # nPM1300 ship mode, IQSHIP (nPM1300 PS v1.1)
HALL_UA = 1.6               # DRV5032ZE/AJ, 20 Hz, on the cell at 3 V, typical (TI SLVSDC7H) — as NIC-Strelka's
BEACON_EVERY_S = 60         # landed: once a minute
CELL_MAH = {"Baker": 150, "Able": 500}   # Baker 100–200 mAh (the design's notes); Able — a guess, open

# --- the shared I2C bus -----------------------------------------------------------------------
I2C_HZ = 400_000            # "six TWI up to 400 kHz" (nRF54LM20 datasheet v1.0, p. 1)
I2C_READ_OVERHEAD_B = 4     # address, register, repeated start, address

# --- the motion sensor's FIFO and the memory's pauses -------------------------------------------
IMU_FIFO_BYTES = 4500       # "Smart FIFO up to 4.5 KB" (ST DS14623 rev 3, LSM6DSV320X, p. 1)
IMU_FIFO_WORD_BYTES = 7     # one tag byte + six data bytes a word (ST's FIFO format — memory)
IMU_WORDS_PER_SAMPLE = 3    # low-g, high-g, gyro
MCU_READ_LATENCY_S = 0.005  # the longest the processor may leave the FIFO unread — a design aim
CARD_PAUSE_S = {"Strelka's pass mark": 0.050, "SD standard's write busy (memory)": 0.250,
                "SDXC's (memory)": 0.500}

# --- position (MIA-M10Q data sheet UBX-22015849 R08) -----------------------------------------------
GNSS_V = 3.0                # Table 16's supply
GNSS_ACQ_MA = 13 + 2.4      # acquisition, GPS+GAL+BDS (the default), VCC + V_IO (Table 16)
GNSS_TRACK_MA = 10 + 2.4    # continuous tracking, the same (Table 16)
GNSS_TTFF_COLD_S = 28       # §1.2, Table 2, the default constellations
GNSS_TTFF_HOT_S = 1
PAD_WAIT_MIN = 30           # a wait on the pad — an assumption, not a requirement
DESCENT_MIN = 2             # parachute from some hundreds of metres at 4–5 m/s (the design's notes)

# --- the pad and the flight -----------------------------------------------------------------------
IMU_MA = 0.8                # 9 axes, combo high-performance (ST DS14623 rev 3, Table "IddHPC9")
MCU_ACTIVE_MA = 2.6         # CPU running (nRF54LM20 datasheet v1.0, p. 1, CoreMark from RRAM)
MEMORY_WRITE_MA = 25        # the SD NAND or card writing — an assumption, its sheet to be read
MEMORY_WRITE_SHARE = 0.3    # share of the time it is busy at ~80 kB/s — an assumption
LIVE_SHARE = 0.9            # the live view sending most of the time in flight (SF7/250, ~25 a second)
FLIGHT_AND_DESCENT_MIN = 3
LANDED_STILL_S = 60         # still this long after the impact's spike → landed

# --- the pressure port ----------------------------------------------------------------------------
AIR_VISCOSITY = 1.8e-5      # Pa·s, air at ~20 °C
AIR_PRESSURE = 101325       # Pa
PORT_D_MM = (0.5, 1.0, 2.0) # one hole's diameter
PORT_L_MM = 1.0             # the wall's thickness
CHAMBER_CM3 = (2, 20, 100)  # a small chamber round the barometer … a whole bay
G = 9.81
SCALE_HEIGHT_M = 8400       # p falls ~1/8400 per metre near the ground
BARO_NOISE_PA = 1.0         # an order for a small barometer in a fast mode — its sheet at level 5

# --- Able on a long flight ----------------------------------------------
ABLE_LONG_MAH = 600        # an example cell
ABLE_PACKET_B = 16         # a packet once a minute, an example
ABLE_PACKET_EVERY_S = 60


# --- the contact's magnets near the magnetometer --------------------------------------------------
MAGNET_D_MM = 3.0           # an assumed disc, Ø 3 × 2 mm, N45 — the real ones asked of the seller
MAGNET_H_MM = 2.0
MAGNET_BR_T = 1.3           # N45 remanence, ~1.3 T (a handbook figure — memory)
MAGNET_PAIR_GAP_MM = 27.2   # the Belka's contact pair is 27.2 mm long, a magnet at each end, poles opposite (NIC-Strelka's contact)
MAGNET_TEMPCO = -0.0012     # NdFeB, about −0.12 % per K (a handbook figure — memory)
EARTH_FIELD_UT = 49         # here, roughly
MAG_RANGE_UT = 800          # the magnetometers' range, ±800 µT (RM3100, MMC5983MA — memory, their sheets at level 5)
MAG_DISTANCE_MM = (15, 20, 30, 50, 70)
TEMP_SWING_K = 20

# --- self-heating inside an insulated box --------------------------------
CELL_V = 3.2                # LiFePO4, nominal
INSULATION_K = 0.035        # W/(m·K), expanded polystyrene (a handbook figure — memory)
INSULATION_MM = (3, 5, 10)
BOX_AREA_CM2 = {"Baker": 30, "Able": 80}   # outer surface, rough

# --- the nRF54LM20's own 2.4 GHz radio instead of LoRa ---------
BLE_FREQ_MHZ = 2440
BLE_TX_DBM = 7              # QFN package's top (nRF54LM20 datasheet v1.0, p. 1: "8 dBm/7 dBm (CSP/QFN)")
BLE_SENS_DBM = {"LE Coded 125 kbit/s": -103, "LE 1 Mbit/s": -95.5}   # datasheet, §PSENS (p. ~1240)
BLE_GROUND_LOSS_DB = 30     # lying in grass at 2.4 GHz — rougher and larger than at 868 MHz; to be measured
