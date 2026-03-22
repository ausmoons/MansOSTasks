# PD5 — TmoteSky radio raidīšanas attālums

Šeit ir divas MansOS programmas TmoteSky (telosb) motēm:

- `tx/` — raidītājs: sūta secīgi numurētas pakas ar atslēgu `PD5RANGE`.
- `rx/` — uztvērējs: katrai saņemtai pakai izvada CSV rindu ar `seq,rssi,lqi,rxCount`.

## Build

No katras mapes atsevišķi:

- `make MOSROOT=../../MansOS telosb`

(ja MansOS ir instalēts citur, nomaini `MOSROOT` vai atstāj pēc noklusējuma.)

## Mērījumu ideja

1. Uzliec `tx` uz vienas motes, `rx` uz otras.
2. Pārvieto motes dažādos attālumos un salīdzini, cik CSV rindas sekundē/minūtē tiek saņemtas (`rxCount`).
3. Pagriez motes (antenas/plates orientāciju) un salīdzini `rssi`/`lqi` sadalījumu un pakas zudumus (pēc `seq` iztrūkumiem).

Ja vajag, vari pievienot kanāla/jaudas iestatīšanu `PD5/tx/main.c` un `PD5/rx/main.c` (MansOS telosb atbalsta `radioSetChannel()` un `radioSetTxPower()`).
