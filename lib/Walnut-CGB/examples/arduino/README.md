This is a basic Walnut-CGB Arduino implementation for the M5Stack Cardputer(ESP32-S3FN8). It demonstrates:

* Running small Game Boy / Game Boy Color ROMs (from available RAM, limited on this platform)
* Basic DMG and CGB palette support (4, 12, or full colors depending on the game)
* Simple file browser for selecting ROMs
* Input handling and screen output on the M5Stack display

Note: This example does not include paging; it only uses available RAM. ROMs larger than what fits in RAM cannot be loaded. Full implementations with paging, internal flash storage or PSRAM support are required for larger games.
