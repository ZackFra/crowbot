# LineyBoi

Line-following firmware for the [Elecrow CrowBot Bolt](https://www.elecrow.com/wiki/crowbot-bolt-programmable-smart-robot-car-steam-robot-kit.html), built with PlatformIO and the Arduino framework on ESP32.

The two underside IR sensors measure line offset. A PID loop turns that into **steer**. A separate throttle cuts **forward** PWM on curves. Those mix per wheel so the robot stays on a dark line.

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

## Controller

`loop()` runs `pid()`. Each pass:

1. `analogRead` both sensors.
2. Clamp to per-sensor calibration min/max and `map` onto a shared **0–1000** scale (white → 0, black → 1000). Raw ADCs are not comparable (left ~98–963, right ~77–2395).
3. `error = leftNormalized - rightNormalized` (about −1000…+1000). If both are on white (`< 100`), reuse `lastError` so the last turn direction is held.
4. **Steer** (PID): `u = KP*error + KI*integral + KD*(error - lastError)/dt`
5. **Throttle** (not PID): `speed = BASE_SPEED - KS*|error|`, floored at 0. Larger offset → less forward PWM so a turn can finish before both sensors leave the tape.
6. Mix: `left = speed + u`, `right = speed - u`. `setWheel` sends that to one forward pin and one reverse pin (L9110S). Negative command means that wheel runs backward.

`delay(10)` is required: ESP32 `analogWrite()` reconfigures LEDC on every call, so a tight loop starves PWM.

### Gains (as in `src/main.cpp`)

| Symbol | Role |
| --- | --- |
| `P` / `KP` | Present offset: how hard to turn right now |
| `I` / `KI` | Stuck offset: slow correction for sensor/motor bias on a straight |
| `D` / `KD` | Trend: ease off if error is already shrinking (stops pivot-overshoot on curves) |
| `KS` | Slow down as `\|error\|` grows (corners) |
| `BASE_SPEED` | Forward PWM when centered (0–255) |

PID does not control measured speed (there are no encoders). It controls **line error**. The actuator is wheel PWM. `speed` is shared forward PWM; `u` is the difference between wheels.

### Tuning

1. `KI = 0`, `KD = 0`. Raise `KP` until it follows, back off if it shakes.
2. Raise `KS` if it still drives off on corners; lower `KS` if it spins in place on gentle bends.
3. Add a small `KD` if curves rock back and forth.
4. Add a small `KI` only if a long straight always sits a hair off-center. Freeze/reset `integral` if you expand lost-line behavior, or I will wind up.

If it turns the wrong way, swap `speed + u` and `speed - u`.

`calibrate()` in `src/main.cpp` prints running min/max and normalized values. Call it from `loop()` instead of `pid()` to retune `MIN_*` / `MAX_*`. `bangBang()` is the old 0/1 on-off follower, unused.

## Build and upload

Needs [PlatformIO](https://platformio.org/).

```bash
pio run --target upload
pio device monitor --baud 115200
```

`platformio.ini` targets `esp32dev` with 8 MB flash, matching the CrowBot Bolt's ESP32-WROVER-B.

Declared libraries (FastLED, U8g2, IRremote) are for later RGB, OLED, and IR remote work. They are not used by the line follower yet.
