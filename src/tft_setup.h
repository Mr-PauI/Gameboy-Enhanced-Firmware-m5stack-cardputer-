#pragma once

// ==========================================
// SCREEN DRIVER SELECTION
// ==========================================

#if defined(USE_ILI9488)
    // --- External driver 480x320 (Native 320x480) ---
    #define ILI9488_DRIVER
 
    // Frequencies (ILI9488 handles high freq well, but starting safe)
    #define SPI_FREQUENCY        10000000 
    #define SPI_READ_FREQUENCY    6000000

#else
    // --- External driver 320x240 (Default) ---
    #define ILI9341_DRIVER
    // Conservative frequencies (can be increased later)
    #define SPI_FREQUENCY        10000000
    #define SPI_READ_FREQUENCY  6000000
#endif

// --- Use HSPI/SPI3 to keep separate from internal display ---
#define USE_HSPI_PORT

// --- External Pins Cardputer-Adv ---
#define TFT_CS     5
#define TFT_DC     6
#define TFT_RST    3

// Moved to "alternative" GPIOs to avoid SD conflicts
#define TFT_SCLK 15
#define TFT_MOSI 13
#define TFT_MISO -1 

// No touch
#define TOUCH_CS -1

// Color configuration
#define TFT_RGB_ORDER TFT_BGR
#define TFT_INVERSION_OFF

// --- Fonts ---
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_GFXFF
#define SMOOTH_FONT