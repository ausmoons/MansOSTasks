# PD6 — Multihop Bezvadu Sensoru Tīkls

Multihop (daudzlēciena) bezvadu sensoru tīkls TmoteSky (TelosB) motēm ar trīs dažādu mezglu tipiem.

## ⚡ Ātrs Sākums

**Pirmo reizi?** Skatīt **[QUICKSTART.md](QUICKSTART.md)** ātrajai pamācībai:
- Kā build visus mezglus
- Kā upload uz motēm
- Kā nolasīt datus no Gateway

## 📋 Projekta Apraksts

Šis projekts realizē daudzlēciena bezvadu sensoru tīklu, kurā:
- **Sensor** mezgli nolasa gaismas sensora vērtības un sūta pa radio
- **Relay** mezgli pārsūta saņemtos datus tālāk (multihop)
- **Gateway** mezgls saņem datus un izvada tos pa USB seriālo portu

### Tīkla Arhitektūra

```
[Sensor 1] ──┐
             ├──> [Relay 100] ──┐
[Sensor 2] ──┘                  ├──> [Gateway] ──> USB/Serial
                                │
[Sensor 3] ─────────────────────┘
```

## 📁 Struktūra

```
PD6/
├── pd6_packet.h        # Kopīgā paketes struktūra visiem mezgliem
├── sensor/
│   ├── main.c          # Sensora mezgla kods
│   └── Makefile
├── relay/
│   ├── main.c          # Releja mezgla kods
│   └── Makefile
├── gateway/
│   ├── main.c          # Vārtejas mezgla kods
│   └── Makefile
├── QUICKSTART.md       # Ātrās pamācības
└── README.md           # Šis fails (detalizēta dokumentācija)
```

## 🔧 Paketes Struktūra

Visi mezgli izmanto vienotu paketes formātu (`pd6_packet.h`):

```c
typedef struct {
    char key[8];           // "PD6MHOP\0" - tīkla identifikators
    uint16_t sourceId;     // Sensora unikālais ID
    uint32_t seq;          // Secības numurs no sensora
    uint16_t lightValue;   // Gaismas sensora vērtība (0-65535)
    uint8_t hopCount;      // Lēcienu skaits no avota
} Pd6Packet_t;
```

## 🚀 Kompilēšana

### 1. Sensor Mezgls

Katram sensoram jānorāda unikāls ID (noklusējuma: 1):

```bash
cd apps/demo/PD6/sensor

# Sensors ar ID=1 (noklusējuma)
make telosb

# Sensors ar ID=2
make telosb CONST=SENSOR_ID=2

# Sensors ar ID=3
make telosb CONST=SENSOR_ID=3
```

### 2. Relay Mezgls

```bash
cd apps/demo/PD6/relay

# Relejs ar ID=100 (noklusējuma)
make telosb

# Relejs ar ID=101
make telosb CONST=RELAY_ID=101
```

### 3. Gateway Mezgls

```bash
cd apps/demo/PD6/gateway
make telosb
```

## 📤 Upload uz Motēm

### DevContainer Vidē (ieteicams)

**1. Build devcontainer:**
```bash
cd apps/demo/PD6/sensor
make telosb
```

**2. Upload no Windows PowerShell:**

```powershell
# Pārbaudi, vai USB nav pievienots WSL
& 'C:\Program Files\usbipd-win\usbipd.exe' list

# Ja pievienots, atvienot
& 'C:\Program Files\usbipd-win\usbipd.exe' detach --busid 2-1

# Upload (nomainīt COM9 uz savu portu)
cd C:\Users\livaz\MansOS

# Sensor 1
python mos\make\scripts\ubsl.py --telosb -c COM9 -r -e -I -p apps\demo\PD6\sensor\build\telosb\image.ihex

# Relay 100
python mos\make\scripts\ubsl.py --telosb -c COM9 -r -e -I -p apps\demo\PD6\relay\build\telosb\image.ihex

# Gateway
python mos\make\scripts\ubsl.py --telosb -c COM9 -r -e -I -p apps\demo\PD6\gateway\build\telosb\image.ihex
```

## 🎯 Mezglu Funkcionalitāte

### Sensor Mezgls (sensor/main.c)

**Funkcionalitāte:**
- Nolasa gaismas sensora vērtību katras 5 sekundes
- Nosūta paketi ar unikālu ID un secības numuru
- Sarkans LED mirgo katru reizi, kad sūta
- Izvada informāciju pa seriālo portu (debug)

**LED indikatori:**
- 🔴 **Sarkans** - pakete nosūtīta

**Seriālais izvads:**
```
Sensor Node ID=1 started
Sent: ID=1 seq=1 light=1024
Sent: ID=1 seq=2 light=1030
```

### Relay Mezgls (relay/main.c)

**Funkcionalitāte:**
- Saņem paketes no sensoriem vai citiem relejiem
- Pārbauda, vai pakete jau ir redzēta (dublikātu noteikšana)
- Ignorē dublikātus (izmantojot `sourceId` + `seq` cache)
- Pārsūta jaunas paketes tālāk, palielinot `hopCount`
- Saglabā pēdējās 20 paketes cache

**LED indikatori:**
- 🟢 **Zaļš** - pakete pārsūtīta
- 🟡 **Dzeltens** - dublikāts ignorēts
- 🔵 **Zils** - heartbeat (relejš dzīvs)

**Seriālais izvads:**
```
Relay Node ID=100 started
Relay: Forwarded - ID=1 seq=1 light=1024 hops=1
Relay: Duplicate ignored - ID=1 seq=1
```

**Dublikātu noteikšana:**
- Saglabā pēdējās 20 paketes (sourceId, seq)
- Ja saņem to pašu (sourceId, seq) atkārtoti → ignorē
- Novērš bezgalīgu pārsūtīšanu tīklā

### Gateway Mezgls (gateway/main.c)

**Funkcionalitāte:**
- Saņem paketes no relejiem vai sensoriem
- Pārbauda, vai pakete jau ir saņemta (dublikātu noteikšana)
- Izvada unikālas paketes pa seriālo portu CSV formātā
- Saglabā pēdējās 30 paketes cache
- Neizraida pa radio (tikai saņem)

**LED indikatori:**
- 🟢 **Zaļš** - jauna pakete saņemta un izvadīta
- 🔴 **Sarkans** - dublikāts ignorēts
- 🔵 **Zils** - heartbeat (gateway dzīvs)

**CSV Izvads:**
```
sourceId,seq,lightValue,hopCount,rssi,lqi,totalReceived
# Gateway started - waiting for packets...
1,1,1024,0,-23,90,1
2,1,980,1,-30,88,2
1,2,1025,0,-23,91,3
```

**Kolonnu nozīme:**
- `sourceId` - sensora ID, kas radīja paketi
- `seq` - secības numurs no sensora
- `lightValue` - gaismas sensora vērtība
- `hopCount` - cik relay mezglus pakete šķērsoja (0 = tieši no sensora)
- `rssi` - signāla stiprums (dBm)
- `lqi` - saites kvalitāte (0-255)
- `totalReceived` - kopējais saņemto pakešu skaits

## 🧪 Testēšana

### Minimālā Konfigurācija

Nepieciešamas **vismaz 3 motes:**
1. **Sensor** (ID=1)
2. **Relay** (ID=100)
3. **Gateway**

### Rekomendētā Konfigurācija

Vismaz **4-5 motes** pilnīgam testam:
1. **Sensor 1** (ID=1)
2. **Sensor 2** (ID=2)
3. **Relay 1** (ID=100)
4. **Relay 2** (ID=101) - ievēro multihop
5. **Gateway**

### Testa Scenāriji

#### 1. Vienkāršs Tests (1 Sensor → Gateway)

```
[Sensor 1] ─────> [Gateway]
```

- Uzliec sensor uz vienas motes (ID=1)
- Uzliec gateway uz otras motes
- Novieto tuvu (tajā pašā istabā)
- Gateway saņems paketes ar `hopCount=0`

#### 2. Ar Vienu Relay

```
[Sensor 1] ──> [Relay 100] ──> [Gateway]
```

- Novieto Sensor un Gateway tālu (dažādās istabās)
- Relay starp tiem
- Gateway saņems paketes ar `hopCount=1`

#### 3. Ar Diviem Relay (Multihop)

```
[Sensor 1] ──> [Relay 100] ──> [Relay 101] ──> [Gateway]
```

- Izveidojiet garāku ķēdi
- Gateway saņems paketes ar `hopCount=2` vai lielāku

#### 4. Vairāki Sensori

```
[Sensor 1] ──┐
             ├──> [Relay 100] ──> [Gateway]
[Sensor 2] ──┘
```

- Gateway saņems paketes no abiem sensoriem
- CSV izvadā redzami dažādi `sourceId`

#### 5. Dublikātu Tests

```
      ┌──> [Relay 100] ──┐
[Sensor 1]              ├──> [Gateway]
      └──> [Relay 101] ──┘
```

- Gateway saņems to pašu paketi no 2 relay
- Tikai viena tiks izvadīta (otra ignorēta kā dublikāts)
- Sarkans LED mirgo, kad dublikāts ignorēts

## 📊 Datu Analīze

### Nolasīt Datus no Gateway

**Windows PowerShell:**
```powershell
# Atrast COM portu
[System.IO.Ports.SerialPort]::GetPortNames()

# Nolasīt datus (izmanto PuTTY, TeraTerm vai Python)
# COM9 - tavs ports, 115200 - baud rate
```

**Ar Python (ieteicams):**
```python
import serial

ser = serial.Serial('COM9', 115200)
print("Listening for packets...")

while True:
    line = ser.readline().decode('utf-8').strip()
    print(line)
```

### CSV Datu Analīze

Saglabā CSV datus un analizē:
- **Pakešu zudums** - iztrūkstošie `seq` numuri no katra `sourceId`
- **Signāla kvalitāte** - RSSI un LQI vērtības
- **Hop count** - cik lēcienus pakete veica
- **Multihop efektivitāte** - vai vairāki relay uzlabo pārklājumu

## 🔍 Unikālo ID Realizācija

### Sensors
- ID tiek definēts kompilācijas laikā ar `SENSOR_ID`
- Noklusējuma vērtība: 1
- Mainīt: `make telosb CONST=SENSOR_ID=X`

### Relay
- ID tiek definēts kompilācijas laikā ar `RELAY_ID`
- Noklusējuma vērtība: 100
- Mainīt: `make telosb CONST=RELAY_ID=X`

### Gateway
- Nav ID (tikai viens gateway tīklā)

## 🐛 Troubleshooting

### Relay nepārsūta paketes
- Pārbaudi, vai radio ir ieslēgts (`radioOn()`)
- Pārbaudi LED indikatorus (zaļš = pārsūta, dzeltens = dublikāts)
- Pārbaudi serial output, vai saņem paketes

### Gateway nesaņem datus
- Pārbaudi, vai motes ir pietiekami tuvu
- Pārbaudi RSSI vērtības (< -70 dBm = pārāk vājš)
- Pieliec relay starpā

### Pārāk daudz dublikātu
- Normāli multihop tīklā!
- Gateway un Relay tos ignorē automātiski
- Palielini `PACKET_CACHE_SIZE`, ja vajag

### Paketes zuducas
- Pārbaudi RSSI/LQI vērtības
- Samazini attālumu starp motēm
- Pievieno vairāk relay mezglus

## 📝 Tehniskā Informācija

### Radio Parametri
- **Frekvence**: 2.4 GHz (IEEE 802.15.4)
- **Radio chips**: CC2420
- **Max TX power**: 0 dBm
- **Packet size**: 17 baiti (bez radio header)

### Laika Intervāli
- **Sensor**: Sūta katras 5 sekundes
- **Relay**: Heartbeat katras 2 sekundes
- **Gateway**: Heartbeat katru sekundi

### Cache Izmēri
- **Relay**: 20 paketes
- **Gateway**: 30 paketes

## 📚 Koda Faili

1. **pd6_packet.h** - Paketes struktūras definīcija (koplietots)
2. **sensor/main.c** - Sensora mezgla implementācija (154 rindas)
3. **relay/main.c** - Releja mezgla implementācija (118 rindas)
4. **gateway/main.c** - Gateway mezgla implementācija (130 rindas)

## 🎓 Mācību Mērķi

Šis projekts demonstrē:
- ✅ Multihop (daudzlēciena) maršrutēšanu
- ✅ Dublikātu noteikšanu un filtrēšanu
- ✅ Unikālu mezglu ID izmantošanu
- ✅ Radio signāla kvalitātes mērīšanu (RSSI, LQI)
- ✅ Sensoru datu vākšanu un pārsūtīšanu
- ✅ Bezvadu sensoru tīkla arhitektūru

## 📄 Licenza

MansOS projekta daļa - izmantot studiju nolūkos.
