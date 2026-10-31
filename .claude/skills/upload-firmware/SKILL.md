---
name: upload-firmware
description: Build and upload a firmware of this monorepo to its board over USB with PlatformIO. Use when the user asks to flash, upload, or record the ATmega328P (uno) or ESP32-S3 (esp32s3) firmware.
argument-hint: "[uno|esp32s3] [COMx]"
disable-model-invocation: true
---

# Upload firmware

Arguments: `$ARGUMENTS` — first the environment (`uno` or `esp32s3`, default `uno`), then an optional serial port (`COM3`, ...).

## 1. Resolve the project

| Environment | Project folder | Board |
|-------------|----------------|-------|
| `uno`       | `atmega328p/`  | Arduino Uno (ATmega328P) |
| `esp32s3`   | `esp32s3/`     | ESP32-S3 N16R8 |

Run every PlatformIO command from the project folder. If the folder has no `platformio.ini`, stop and tell the user the project does not exist yet.

## 2. Resolve the PlatformIO executable

Use `pio` if it is on the PATH; otherwise use `~/.platformio/penv/Scripts/platformio.exe`.
If it fails with `No module named 'platformio'`, report it and offer to repair it with `~/.platformio/penv/Scripts/python.exe -m pip install platformio`. Do not repair without the user's approval.

## 3. Find the port

Run `platformio device list`.
- Port given in the arguments: use it.
- Exactly one USB serial device: use it.
- None: stop and ask the user to check the cable and the driver (CH340 on clone boards).
- More than one: list them and ask which one to use.

## 4. Build and upload

```
platformio run -e <env> -t upload --upload-port <port>
```

Report the RAM and Flash usage printed by the build and whether the upload succeeded.

## 5. Troubleshooting

| Symptom | Likely cause | Action |
|---------|--------------|--------|
| `Access is denied` / port busy | Serial monitor or Arduino IDE holding the port | Ask the user to close it, then retry once |
| `stk500_getsync(): not in sync` (uno) | Wrong port, wrong board, or bad cable | Confirm the port with the user |
| `Failed to connect to ESP32` (esp32s3) | Board not in download mode | Ask the user to hold BOOT, tap RESET, release BOOT, then retry |

Never retry an upload more than once without new information from the user.

## 6. Serial monitor

The monitor is interactive and cannot run inside a tool call. After a successful upload, tell the user to run it themselves:

```
! cd <project folder> && ~/.platformio/penv/Scripts/platformio.exe device monitor
```

The baud rate comes from `monitor_speed` in `platformio.ini`.
