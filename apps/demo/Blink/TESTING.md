# Testing Blink in DevContainer

## Quick Start (No Hardware)

1. **Open terminal** in VS Code (`Ctrl+``)
2. **Navigate and build**:
   ```bash
   cd apps/demo/Blink
   make pc
   make pc run
   ```

## Build for TelosB

```bash
cd apps/demo/Blink
make telosb
```

Built files will be in `build/` directory.

## Upload to Physical Mote

### Docker Desktop Limitation

USB devices attached to `docker-desktop` WSL don't appear inside DevContainers. Use this workaround:

#### 1. Build in DevContainer

```bash
cd apps/demo/Blink
make telosb
```

#### 2. Detach USB from WSL (PowerShell as Admin)

```powershell
usbipd detach --busid X-X      # Replace X-X with your BUSID from 'usbipd list'
```

#### 3. Find COM port in Windows

```powershell
mode                           # Look for "COM3", "COM9", etc.
```

#### 4. Install pyserial (first time only)

```powershell
python -m pip install pyserial
```

#### 5. Upload from Windows

```powershell
python mos\make\scripts\ubsl.py --telosb -c COMX -r -e -I -p apps\demo\Blink\build\telosb\image.ihex
```
Replace `COMX` with your actual COM port (e.g., `COM9`).

### Alternative: Native WSL (Full USB Support)

For proper USB support in DevContainers, use native Ubuntu WSL instead of Docker Desktop:

1. Install Ubuntu WSL: `wsl --install Ubuntu`
2. Install Docker in Ubuntu (not Docker Desktop)
3. Attach USB: `usbipd attach --wsl --busid X-X --distribution Ubuntu`
4. Then `make telosb upload` works directly in DevContainer

## Troubleshooting

**'usbipd' is not recognized?**

If you get this error in PowerShell:
```
usbipd : The term 'usbipd' is not recognized as the name of a cmdlet, function, script file, or operable program.
```

You need to install usbipd-win first:

1. Download and install from: https://github.com/dorssel/usbipd-win/releases
2. Or install via winget:
   ```powershell
   winget install --interactive --exact dorssel.usbipd-win
   ```
3. Restart PowerShell and try again

**Device not found?**
```bash
ls /dev/tty*                    # Check if device is visible
python3 ../../tools/lib/motelist.py  # List motes
```

**Permission denied?**
```bash
sudo chmod 666 /dev/ttyUSB0
```

## Other Platforms

```bash
make z1           # Zolertia Z1
make launchpad    # MSP430 LaunchPad
make clean        # Clean build files
```
