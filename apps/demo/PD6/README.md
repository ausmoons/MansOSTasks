# PD6 — Multihop Bezvadu Sensoru Tīkls

Multihop (daudzlēciena) bezvadu sensoru tīkls TmoteSky (TelosB) motēm ar trīs dažādu mezglu tipiem.


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
```

## 🔧 Paketes Struktūra

Visi mezgli izmanto vienotu paketes formātu (`pd6_packet.h`):

```c
#define PD6_KEY_LEN 8
#define PD6_KEY_STR "PD6MHOP\0"

typedef struct __attribute__((packed))
{
    char key[PD6_KEY_LEN]; // "PD6MHOP" - tīkla identifikators
    uint16_t sourceId;     // Sensora unikālais ID
    uint32_t seq;          // Secības numurs no sensora
    uint16_t lightValue;   // Gaismas sensora vērtība (0-65535)
    uint8_t hopCount;      // Lēcienu skaits no avota
} Pd6Packet_t;
```

## 🚀 Kompilēšana

### 1. Sensor Mezgls

```bash
cd apps/demo/PD6/sensor
make telosb
```

### 2. Relay Mezgls

```bash
cd apps/demo/PD6/relay
make telosb
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
Sensor started, ID=0x561f
Sent: ID=0x561f seq=1 light=1024
Sent: ID=0x561f seq=2 light=1030
```
> ID tiek nolasīts automātiski no aparatūras (DS2411 čips) — nav jākonfigurē manuāli.

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
Relay started, ID=0x3a2b
Relay: Forwarded - ID=0x561f seq=1 light=1024 hops=1
Relay: Duplicate ignored - ID=0x561f seq=1
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
# Gateway started, ID=0x7f4c
22047,1,1024,0,-23,90,1
14891,1,980,1,-30,88,2
22047,2,1025,0,-23,91,3
```
> `sourceId` ir decimālais `localAddress` (piemēram, `0x561f` = `22047`).

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
1. **Sensor** — unikāls ID automātiski no aparatūras
2. **Relay** — unikāls ID automātiski no aparatūras
3. **Gateway**

### Rekomendētā Konfigurācija

Vismaz **4-5 motes** pilnīgam testam:
1. **Sensor 1** — piem. ID=0x561f (tavs mote)
2. **Sensor 2** — piem. ID=0x3a2b (cits mote)
3. **Relay 1**
4. **Relay 2** - ievēro multihop
5. **Gateway**

> **Svarīgi:** ID nav jākonfigurē manuāli. Katrs mote nolasa savu unikālo ID no iebūvētās DS2411 aparatūras čipa. Uzlādē vienu un to pašu `.ihex` failu uz visām motēm — katra zina savu ID pati.

### Testa Scenāriji

#### 1. Vienkāršs Tests (1 Sensor → Gateway)

```
[Sensor 1] ─────> [Gateway]
```

- Uzliec sensor uz vienas motes (ID piešķirts automātiski)
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

ser = serial.Serial('COM9', 38400)
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

Katrs mezgls automātiski nolasa savu unikālo ID no aparatūras — TelosB motei ir iebūvēta **DS2411** mikroshēma ar unikālu 8-baitu sērijas numuru. MansOS to pārvērš par 16-bitu `localAddress`, ko var tieši izmantot kodā.

Vienāds `.ihex` fails var tikt uzlādēts uz visām motēm — katra zina savu ID pati.

### Zināmās Motēs

| USB sērijas nr. | localAddress (hex) | localAddress (dec) | Loma |
|-----------------|--------------------|--------------------|------|
| M4AETB0A        | 0x561f             | 22047              | Sensor 1 |
| M4AOQG8Q        | 0x69b0             | 27056              | Sensor 2 |

> ID tika nolasīts automātiski, palaižot sensora firmware un klausoties seriālo portu (`Sensor started, ID=0x561f`).

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
