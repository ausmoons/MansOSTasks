# MansOS DevContainer Setup for Windows

This devcontainer provides a complete Linux-based development environment for MansOS with all required toolchains.

## Prerequisites

1. **Docker Desktop for Windows**
   - Download from: https://www.docker.com/products/docker-desktop
   - Make sure WSL 2 backend is enabled

2. **VS Code Extensions**
   - Install "Dev Containers" extension (ms-vscode-remote.remote-containers)

3. **usbipd-win** (for USB mote access) — run once in PowerShell as Administrator:
   ```powershell
   winget install --interactive --exact dorssel.usbipd-win
   ```

## Getting Started

1. Open this folder in VS Code
2. Press `F1` and select **"Dev Containers: Reopen in Container"**
3. **First time only**: Wait for Docker to build the image (~2-3 minutes)
   - Subsequent opens will be much faster (5-10 seconds)

## Included Tools

The devcontainer automatically installs:
- **MSP430 toolchain**: gcc-msp430, binutils-msp430, msp430-libc, msp430mcu
- **AVR toolchain**: gcc-avr, binutils-avr, avr-libc
- **Python tools**: python3, python3-ply, python3-serial, pyserial
- **Build tools**: make, gcc

## Connecting a Mote (every time you plug one in)

Do these steps in **Windows PowerShell as Administrator** each time you connect a mote:

**Step 1** — Find the mote's BUSID:
```powershell
usbipd list
```
Look for "USB Serial Converter" — note its BUSID (e.g. `2-1`).

**Step 2** — Bind and attach it to WSL (replace `2-1` with your BUSID):
```powershell
usbipd bind --busid 2-1
usbipd attach --wsl --busid 2-1
wsl -d docker-desktop -- modprobe ftdi_sio
```
> Note: `bind` is only needed the first time for each mote. After that, just `attach` and `modprobe`.

**Step 3** — Back in the container terminal, fix permissions:
```bash
sudo chmod 666 /dev/ttyUSB0
```

**Step 4** — Verify the mote is detected:
```bash
python3 /workspaces/MansOS/tools/lib/motelist.py
```

## Building and Uploading

Navigate to your app directory and run:

```bash
cd /workspaces/MansOS/apps/demo/PD5/rx

# Clean, build and upload in one command:
make clean && make telosb upload
```

To build only (without uploading):
```bash
make telosb
```

To upload previously built firmware:
```bash
make telosb upload
```

## Supported Platforms

- **telosb** - TelosB (Tmote Sky)
- **z1** - Zolertia Z1
- **launchpad** - TI MSP430 LaunchPad
- **xm1000** - XM1000
- **atmega** - Arduino/ATmega platforms
- **pc** - PC simulation (no hardware needed)

## Testing Without Hardware

```bash
cd apps/demo/Blink
make pc run
```

## Troubleshooting

### Mote not showing up (`/dev/ttyUSB0` missing)

1. Re-run the PowerShell steps above (bind → attach → modprobe)
2. Check the mote is detected in the kernel log inside the container:
   ```bash
   dmesg | tail -20
   ```
   You should see lines mentioning `tmote sky` or `FT232`.

### "Permission denied" on /dev/ttyUSB0

Run inside the container:
```bash
sudo chmod 666 /dev/ttyUSB0
```

### Container opening is slow

First build is ~2-3 minutes (normal). If subsequent opens are slow:
`F1` → **"Dev Containers: Rebuild Container"**

### Build errors

Make sure you are inside an app directory (e.g. `apps/demo/PD5/rx`) when running make commands.

## Additional Resources

- MansOS Wiki: https://github.com/IECS/MansOS/wiki
- Project website: http://mansos.net
