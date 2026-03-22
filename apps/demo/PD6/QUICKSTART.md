# PD6 Quick Start Guide

Ātrā pamācība, kā uzbūvēt un ielādēt PD6 multihop tīklu.

## 📋 Nepieciešamais

- Vismaz **3 TelosB motes**:
  - 1x Sensor
  - 1x Relay
  - 1x Gateway
- Ieteicams: **4-5 motes** pilnīgam testam

---

## 🚀 Soli pa Solim

### 1️⃣ BUILD DEVCONTAINER

Atvērt VS Code devcontainer termināli un build visus mezglus:

```bash
# Build Sensor (ID=1)
cd apps/demo/PD6/sensor
make telosb

# Build Sensor (ID=2) - ja vajag vairākus sensorus
make clean
make telosb CONST=SENSOR_ID=2

# Build Relay
cd ../relay
make telosb

# Build Gateway
cd ../gateway
make telosb
```

---

### 2️⃣ UPLOAD NO WINDOWS

Atvērt **PowerShell** (Windows):

#### A. Atvienot no WSL (ja pievienots)

```powershell
& 'C:\Program Files\usbipd-win\usbipd.exe' detach --busid 2-1
```

#### B. Pārbaudīt COM portu

```powershell
[System.IO.Ports.SerialPort]::GetPortNames()
```
Pieraksti COM numuru (piemēram: **COM9**)

#### C. Upload uz motēm

```powershell
cd C:\Users\livaz\MansOS

# Upload Sensor 1
python mos\make\scripts\ubsl.py --telosb -c COM9 -r -e -I -p apps\demo\PD6\sensor\build\telosb\image.ihex

# Aizvieto mote un upload Relay
python mos\make\scripts\ubsl.py --telosb -c COM9 -r -e -I -p apps\demo\PD6\relay\build\telosb\image.ihex

# Aizvieto mote un upload Gateway
python mos\make\scripts\ubsl.py --telosb -c COM9 -r -e -I -p apps\demo\PD6\gateway\build\telosb\image.ihex
```

**Svarīgi:** Nomainiet `COM9` uz savu COM portu!

---

### 3️⃣ NOVIETOT MOTES

#### Vienkāršs Tests:
```
[Sensor] ──> [Relay] ──> [Gateway + USB]
```

- **Sensor**: Jebkur (raidīs datus)
- **Relay**: Pa vidu
- **Gateway**: Pieslēgts datoram (USB)

---

### 4️⃣ NOLASĪT DATUS

#### Opcija A: Ar Python (ieteicams)

```python
import serial

ser = serial.Serial('COM9', 115200)
print("Gateway output:")

while True:
    line = ser.readline().decode('utf-8').strip()
    print(line)
```

#### Opcija B: Ar PuTTY

1. Atvērt PuTTY
2. Connection type: **Serial**
3. Serial line: **COM9**
4. Speed: **115200**
5. Click **Open**

---

## 📊 Ko Tu Redzēsi

### Gateway CSV Izvads:

```
sourceId,seq,lightValue,hopCount,rssi,lqi,totalReceived
# Gateway started - waiting for packets...
1,1,1024,1,-30,88,1
1,2,1028,1,-30,89,2
1,3,1025,1,-31,87,3
```

### LED Indikatori:

**Sensor:**
- 🔴 Sarkans mirgo - sūta paketi (katras 5s)

**Relay:**
- 🟢 Zaļš mirgo - pārsūtīja paketi
- 🟡 Dzeltens mirgo - ignorēja dublikātu
- 🔵 Zils mirgo - heartbeat (katras 2s)

**Gateway:**
- 🟢 Zaļš mirgo - saņēma jaunu paketi
- 🔴 Sarkans mirgo - ignorēja dublikātu
- 🔵 Zils mirgo - heartbeat (katru 1s)

---

## 🔧 Biežāk Lietotās Komandas

### Build ar Dažādiem ID:

```bash
# Sensor ar ID=2
cd apps/demo/PD6/sensor
make telosb CONST=SENSOR_ID=2

# Sensor ar ID=3
make clean
make telosb CONST=SENSOR_ID=3

# Relay ar ID=101
cd ../relay
make telosb CONST=RELAY_ID=101
```

### Clean Build:

```bash
cd apps/demo/PD6/sensor
make clean
make telosb
```

### Rebuild Visus:

```bash
cd apps/demo/PD6
cd sensor && make clean && make telosb && cd ..
cd relay && make clean && make telosb && cd ..
cd gateway && make clean && make telosb && cd ..
```

---

## 🐛 Problēmu Risināšana

### ❌ "No attached motes found"
→ Atvienot no WSL: `usbipd detach --busid 2-1`

### ❌ Gateway nesaņem datus
→ Pārbaudi, vai:
- Visas motes ir ieslēgtas (USB vai baterija)
- Relay ir starp Sensor un Gateway
- Motes nav pārāk tālu (< 10m istabā)

### ❌ Pārāk daudz dublikātu
→ Normāli! Relay un Gateway tos ignorē automātiski

### ❌ Serial port nestrādā
→ Pārbaudi COM portu: `[System.IO.Ports.SerialPort]::GetPortNames()`

---

## 📚 Pilnā Dokumentācija

Skatīt **README.md** detalizētai informācijai par:
- Paketes struktūru
- Mezglu funkcionalitāti
- Testa scenārijiem
- Datu analīzi
