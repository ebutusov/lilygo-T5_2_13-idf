# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Pure ESP-IDF (C++, no Arduino) firmware for the LILYGO T5 V2.3.x board: classic ESP32, 4 MB flash, no PSRAM, 2.13" 250x122 SSD1680-family e-paper. Per the README, the code was written from the LILYGO pin map and the SSD1680 command set and has **not been run on hardware yet**. Treat driver details (init sequence, update-control values, mirroring) as unverified. The README's "Things to check on first run" lists the known suspects.

## Commands

Needs ESP-IDF 5.x with the environment loaded (`$IDF_PATH`). The existing `build/` was generated with CMake 4.0.3.

    idf.py set-target esp32      # once; sdkconfig.defaults is applied on first configure
    idf.py build
    idf.py flash monitor         # if flashing fails, hold BOOT (GPIO0) while pressing RESET

There are no tests and no lint config beyond `.clangd`. `build/`, `sdkconfig` and `sdkconfig.old` are generated; edit `sdkconfig.defaults` for config changes. It only takes effect on a fresh configure, so delete `sdkconfig` or run `idf.py fullclean` after changing it.

## Architecture

- `components/epd/` is the whole driver (`include/epd213.hpp`, `epd213.cpp`) in namespace `epd`. It is a local IDF component, picked up automatically by the top-level `CMakeLists.txt`. It requires `esp_driver_gpio`, `esp_driver_spi` and `esp_timer`. The build tree (`esp_hal_*` components) indicates ESP-IDF 6.x, where the monolithic `driver` component no longer provides `driver/gpio.h` or `driver/spi_master.h`, so depend on the split `esp_driver_*` components.
- `main/main.cpp` is a demo only: full refresh, 5 partial refreshes, final full refresh, then `sleep()`.

Key design points of `Epd213`:
- **Draw/refresh split.** All drawing (`drawPixel`, lines, rects, bitmap, built-in 5x7 font) only writes the in-RAM framebuffer `fb_`. Nothing reaches the panel until `refreshFull()` or `refreshPartial()`.
- **Framebuffer is in native portrait layout** (122x250, 16 bytes per row, bit 1 = white). Rotation is applied in `drawPixel` (`nx`/`ny` mapping), so mirroring fixes belong there. `Color::Black` is enum value 1, which is the opposite of the buffer's bit polarity.
- **Partial refresh depends on a baseline and a custom LUT.** `baseline_valid_` is set after a full refresh. `refreshPartial()` falls back to `refreshFull()` when it is false. The "previous" image goes in RAM 0x26 and the "new" image in 0x24 (`prev_` keeps a copy that is re-sent before each partial update). The DEPG0213BN's built-in mode-2 waveform is not a usable partial update, so `refreshPartial()` uploads `kLutPartial` (from GxEPD2's `GxEPD2_213_BN`) with 0x32 and updates with 0xCC. Reference for init/update sequences: GxEPD2 `GxEPD2_213_BN.cpp`.
- **Power.** `sleep()` puts the panel in deep sleep with RAM retained. The panel should not sit powered with a static image, so call `sleep()` after the last update.
- Pins are the `kT5V23Pins` default (BUSY 4, RST 16, DC 17, CS 5, SCLK 18, MOSI 23). There is no power-enable GPIO on this revision. SPI uses SPI2_HOST.
- `sdkconfig.defaults` sets `CONFIG_FREERTOS_HZ=1000`, which the driver's short `vTaskDelay` BUSY polling relies on. It also disables C++ exceptions, so don't use `throw`/`try`. Report errors through `esp_err_t`.
