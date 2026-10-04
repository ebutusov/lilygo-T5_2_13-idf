# T5 V2.3.x 2.13" e-paper on pure ESP-IDF (C++)

Target: LILYGO T5 V2.3 / V2.3.1 / V2.3.2, 2.13" 250x122 e-paper, classic ESP32.
Tested against: nothing yet. This was written from the LILYGO pin map and the SSD1680 command set and has not been run on hardware.

## Build and flash

    . $IDF_PATH/export.sh          # ESP-IDF 5.x
    idf.py set-target esp32
    idf.py build flash monitor

If flashing fails, hold BOOT (GPIO0) while pressing RESET.

## Layout

- `components/epd/` - driver component (`epd213.hpp`, `epd213.cpp`): SPI/GPIO setup, SSD1680 init, full and partial refresh, deep sleep, framebuffer drawing, built-in 5x7 font.
- `main/main.cpp` - demo: full refresh, five partial refreshes (counter), final full refresh, deep sleep.

## Pins (see `kT5V23Pins` in `epd213.hpp`)

BUSY 4, RST 16, DC 17, CS 5, SCLK 18, MOSI 23. No power-enable GPIO on this revision (that is V2.4 only).

## Things to check on first run

- Blank or garbage screen: confirm the panel variant. If your panel is an older GDEH0213B73 / SSD1675 type, the init sequence needs a LUT upload (0x32) and different update-control values.
- Image mirrored in landscape: swap the `nx`/`ny` mapping in `Epd213::drawPixel`.
- Partial refresh looks washed out: raise the border waveform or do a full refresh more often.
- Always call `sleep()` after the last update, and use a full refresh after waking from long idle periods.
