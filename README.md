# T5 V2.3.x 2.13" e-paper on pure ESP-IDF (C++)

Target: LILYGO T5 V2.3 / V2.3.1 / V2.3.2, 2.13" 250x122 e-paper, classic ESP32.
Tested on: LILYGO T5 V2.3.x with a DEPG0213BN (SSD1680) panel, ESP-IDF 6.1. Full and partial refresh both work. Other panel variants are untested.

## Build and flash

    . $IDF_PATH/export.sh          # ESP-IDF 5.x
    idf.py set-target esp32
    idf.py build flash monitor

If flashing fails, hold BOOT (GPIO0) while pressing RESET.

## Layout

- `components/epd/` - driver component (`epd213.hpp`, `epd213.cpp`): SPI/GPIO setup, SSD1680 init, full and partial refresh (custom partial LUT), deep sleep, framebuffer drawing, built-in 5x7 font.
- `main/main.cpp` - demo: full refresh, five partial refreshes (counter), final full refresh, deep sleep.

## Pins (see `kT5V23Pins` in `epd213.hpp`)

BUSY 4, RST 16, DC 17, CS 5, SCLK 18, MOSI 23. No power-enable GPIO on this revision (that is V2.4 only).

## Notes

- Partial refresh uploads a custom waveform (`kLutPartial`, from GxEPD2's `GxEPD2_213_BN`) with command 0x32 and updates with 0xCC. The controller's built-in mode-2 waveform is not a usable partial update on this panel: it either runs the full 5 s waveform or changes nothing. A partial update takes about 0.7 s, a full one about 5 s.
- Other panels need their own init sequence and LUT. An older GDEH0213B73 / SSD1675 type most likely needs different update-control values as well.
- Image mirrored in landscape: swap the `nx`/`ny` mapping in `Epd213::drawPixel`.
- Partial updates accumulate ghosting; do a full refresh now and then.
- Always call `sleep()` after the last update, and use a full refresh after waking from long idle periods.
