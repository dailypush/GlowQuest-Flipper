# GlowQuest Flipper

Experimental MagicBand+ effects app using the **Flipper Zero built-in BLE radio**, no ESP32 required.

## Status
Native extra-beacon transmitter is now implemented in source. **FAP build and real MagicBand+ hardware response are not yet verified.** This is a development build, not a finished release.

### Implemented
- Native Flipper UI: Colors, LED Mixer, FX Studio, Fireworks, Range Lab, Band Scout, Settings
- 9-byte E9-05 color/mask/vibration packet builder with bounds checks
- Manufacturer-specific BLE advertising data (AD type 0xFF, company 0x0183)
- Non-connectable 20–25 ms extra beacon via `furi_hal_bt_extra_beacon_*`
- 1.8-second transmission when selecting color/effect; 12-cue simulated fireworks scheduler now transmits cues
- Native protocol tests and GitHub Actions uFBT build

### Not yet implemented
- CC fast-mode wake/prewarm (some bands may not respond promptly)
- Actual scanning/Band Scout and configurable TX power
- Saved custom sequences, advanced animations
- Verified hardware tests and downloadable FAP

### Build
Open `applications_user/glowquest_flipper` and run `ufbt` with the official Flipper SDK. CI is at [Actions](../../actions).

### Reference and credit
Protocol exploration: [GlowQuest BandLab](https://github.com/dailypush/GlowQuest-BandLab). Native beacon technique: [Haw8411/magic-band-plus-lights](https://github.com/Haw8411/magic-band-plus-lights). Official [Flipper extra beacon API](https://developer.flipper.net/flipperzero/doxygen/furi__hal__bt_8h.html).

Use only with bands you own or are authorized to test. The BLE broadcasts are not addressed to a single band.
