# LineyBoi

Line-following firmware for the [Elecrow CrowBot Bolt](https://www.elecrow.com/wiki/crowbot-bolt-programmable-smart-robot-car-steam-robot-kit.html), built with PlatformIO and the Arduino framework on ESP32.

The robot reads the two underside IR reflectance sensors and steers to stay on a dark line.

## Hardware

| Function | GPIO |
| --- | --- |
| Left motor − | 12 |
| Left motor + | 13 |
| Right motor − | 14 |
| Right motor + | 15 |
| Left line sensor | 36 |
| Right line sensor | 39 |

Motors are N20 gear motors driven by the onboard L9110S. Line sensors are the CrowBot RPR220 pair.

The ESP32 runs from USB, but the motors run from the 4× AAA pack. Turn the robot's power switch on or the wheels will not move even if serial output looks healthy.

## Line following

Each sensor is analog-read and treated as on-line when the value is **≤ 1000** (dark) and off-line when **> 1000** (light).

| Left | Right | Action |
| --- | --- | --- |
| 1 (light) | 1 (light) | Forward |
| 1 (light) | 0 (dark) | Left |
| 0 (dark) | 1 (light) | Right |
| 0 (dark) | 0 (dark) | Stop |

Motor PWM duty is `HALF_SPEED` (currently 60 of 255). CrowBot motors often need more than that to start from a stall; raise it if the robot is sluggish.

`loop()` includes a `delay(50)`. ESP32 `analogWrite()` reconfigures LEDC on every call, so writing the motor pins with no delay can starve PWM and leave the motors dead.

Serial prints `left right` at 115200 baud.

## Build and upload

Needs [PlatformIO](https://platformio.org/).

```bash
pio run --target upload
pio device monitor --baud 115200
```

`platformio.ini` targets `esp32dev` with 8 MB flash, matching the CrowBot Bolt's ESP32-WROVER-B.

Declared libraries (FastLED, U8g2, IRremote) are for later RGB, OLED, and IR remote work. They are not used by the line follower yet.
