# MansOS DevContainer Setup for Windows

This devcontainer provides a complete Linux-based development environment for MansOS with all required toolchains.

## Prerequisites

1. **Docker Desktop for Windows**
   - Download from: https://www.docker.com/products/docker-desktop
   - Make sure WSL 2 backend is enabled

2. **VS Code Extensions**
   - Install "Dev Containers" extension (ms-vscode-remote.remote-containers)

## Getting Started

1. Open this folder in VS Code
2. Press `F1` and select **"Dev Containers: Reopen in Container"**
3. **First time only**: Wait for Docker to build the image (~2-3 minutes)
   - Subsequent opens will be much faster (5-10 seconds)
   - The image is cached, so you only build once

## Included Tools

The devcontainer automatically installs:
- **MSP430 toolchain**: gcc-msp430, binutils-msp430, msp430-libc, msp430mcu
- **AVR toolchain**: gcc-avr, binutils-avr, avr-libc
- **Python tools**: python3, python3-ply, python3-serial, pyserial
- **Build tools**: make, gcc

## Connecting Physical Motes (USB Serial Devices)

### Option 1: USB/IP (Recommended for WSL 2)

Docker Desktop on Windows with WSL 2 requires USB/IP to access USB devices:

1. **Install usbipd-win on Windows** (run in PowerShell as Administrator):
   ```powershell
   winget install --interactive --exact dorssel.usbipd-win
   ```

2. **List connected USB devices** (in PowerShell):
   ```powershell
   usbipd list
   ```

3. **Bind your mote device** (replace `BUSID` with your device's bus ID):
   ```powershell
   usbipd bind --busid BUSID
   ```

4. **Attach the device to WSL** (do this each time you connect the mote):
   ```powershell
   usbipd attach --wsl --busid BUSID
   ```

5. **Verify in container**:
   ```bash
   ls /dev/ttyUSB* /dev/ttyACM*
   ```

### Option 2: Docker with Hyper-V

If using Docker with Hyper-V backend, USB passthrough is more complex. Consider using WSL 2 backend instead.

## Building and Uploading

1. Navigate to a demo app:
   ```bash
   cd apps/demo/Blink
   ```

2. **Build for your platform** (e.g., telosb):
   ```bash
   make telosb
   ```

3. **Upload to mote**:
   ```bash
   make telosb upload
   ```

4. **List connected motes**:
   ```bash
   python3 ../../../tools/lib/motelist.py
   ```

## Supported Platforms

- **telosb** - TelosB (Tmote Sky)
- **z1** - Zolertia Z1
- **launchpad** - TI MSP430 LaunchPad
- **xm1000** - XM1000
- **atmega** - Arduino/ATmega platforms
- **pc** - PC simulation (no hardware needed)

## Testing Without Hardware

Use the PC platform for testing without physical hardware:

```bash
cd apps/demo/Blink
make pc
make pc run
```

## Troubleshooting

### Container opening is slow

If the container is taking too long to open:
1. First build is ~2-3 minutes (normal)
2. If subsequent opens are slow, rebuild: `F1` → **"Dev Containers: Rebuild Container"**
3. Check Docker Desktop is running and not updating

### "Permission denied" on /dev/ttyUSB0

Add your user to dialout group:
```bash
sudo usermod -a -G dialout $USER
```
Then restart the container.

### Mote not detected

1. Verify USB passthrough: `lsusb` should show your device
2. Check serial ports: `ls -l /dev/tty*`
3. Try running motelist: `python3 tools/lib/motelist.py`

### Build errors

Make sure you're in an app directory (e.g., `apps/demo/Blink`) when running make commands.

## Additional Resources

- MansOS Wiki: https://github.com/IECS/MansOS/wiki
- Project website: http://mansos.net
