#define ENABLE_SOUND 0
#define ENABLE_LCD 1
#define MAX_FILES 256

#include "M5Cardputer.h"
#include "walnut_cgb.h"
#include "SD.h"

#include <TFT_eSPI.h>
#include <sys/time.h>
#include <unistd.h>

// -------------------------
// Video target
// -------------------------
// With external TFT we can keep native 144 lines.
// If you want the old 135-line compression, set to 0.
#define USE_NATIVE_GB_HEIGHT 1

#if USE_NATIVE_GB_HEIGHT
  #define DEST_H 144
#else
  #define DEST_H 135
#endif

// SD card SPI class (Cardputer SD pins)
SPIClass SPI2;

// External TFT (configured by TFT_eSPI + your injected tft_setup.h)
TFT_eSPI tft = TFT_eSPI();

static inline uint16_t rgb888_to_rgb565(uint32_t rgb) {
  return (uint16_t)(
    ((rgb >> 8)  & 0xF800) |
    ((rgb >> 5)  & 0x07E0) |
    ((rgb >> 3)  & 0x001F)
  );
}

#if WALNUT_GB_12_COLOUR
uint32_t gboriginal_palette[] = {
  0x7B8210, 0x5A7942, 0x39594A, 0x294139,
  0x7B8210, 0x5A7942, 0x39594A, 0x294139,
  0x7B8210, 0x5A7942, 0x39594A, 0x294139
};
uint16_t CURRENT_PALETTE_RGB565[12];
void update_palette() {
  for (int i = 0; i < 12; i++) CURRENT_PALETTE_RGB565[i] = rgb888_to_rgb565(gboriginal_palette[i]);
}
#else
uint32_t gboriginal_palette[] = { 0x7B8210, 0x5A7942, 0x39594A, 0x294139 };
uint16_t CURRENT_PALETTE_RGB565[4];
void update_palette() {
  for (int i = 0; i < 4; i++) CURRENT_PALETTE_RGB565[i] = rgb888_to_rgb565(gboriginal_palette[i]);
}
#endif

// -------------------------
// INTERNAL display only (UI/debug/menu)
static inline void debugPrint(const char* str) {
  M5Cardputer.Display.clearDisplay();
  M5Cardputer.Display.drawString(str, 0, 0);
}

static inline void set_font_size(int size) {
  int textsize = M5Cardputer.Display.height() / size;
  if (textsize == 0) textsize = 1;
  M5Cardputer.Display.setTextSize(textsize);
}

// -------------------------
// Walnut/Peanut-GB structures and callbacks
struct priv_t {
  uint8_t *rom;
  uint8_t *cart_ram;

  // Presentation framebuffer: DEST_H rows x 160 cols (RGB565)
  uint16_t fb[DEST_H][LCD_WIDTH];
};

uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr) {
  const struct priv_t * const p = (const struct priv_t *)gb->direct.priv;
  return p->rom[addr];
}

uint16_t gb_rom_read_16bit(struct gb_s *gb, const uint_fast32_t addr) {
  const uint8_t *src = &((const struct priv_t *)gb->direct.priv)->rom[addr];
  if ((uintptr_t)src & 1) return ((uint16_t)src[0]) | ((uint16_t)src[1] << 8);
  return *(uint16_t *)src;
}

uint32_t gb_rom_read_32bit(struct gb_s *gb, const uint_fast32_t addr) {
  const uint8_t *src = &((const struct priv_t *)gb->direct.priv)->rom[addr];
  if ((uintptr_t)src & 3) {
    return ((uint32_t)src[0]) |
           ((uint32_t)src[1] << 8) |
           ((uint32_t)src[2] << 16) |
           ((uint32_t)src[3] << 24);
  }
  return *(uint32_t *)src;
}

uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr) {
  const struct priv_t * const p = (const struct priv_t *)gb->direct.priv;
  return p->cart_ram[addr];
}

void gb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr, const uint8_t val) {
  const struct priv_t * const p = (const struct priv_t *)gb->direct.priv;
  p->cart_ram[addr] = val;
}

uint8_t *read_rom_to_ram(const char *file_name) {
  File rom_file = SD.open(file_name);
  if (!rom_file) return NULL;

  size_t rom_size = rom_file.size();
  if (rom_size == 0) { rom_file.close(); return NULL; }

  char *readRom = (char*)malloc(rom_size);
  if (!readRom) { rom_file.close(); return NULL; }

  if (rom_file.readBytes(readRom, rom_size) != (int)rom_size) {
    free(readRom);
    rom_file.close();
    return NULL;
  }

  uint8_t *rom = (uint8_t*)readRom;
  char buf[96];
  sprintf(buf, "ROM f:%02x l:%02x sz:%u", rom[0], rom[rom_size-1], (unsigned)rom_size);
  debugPrint(buf);

  rom_file.close();
  return rom;
}

void gb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t val) {
  (void)gb_err; (void)val;
  struct priv_t *priv = (struct priv_t *)gb->direct.priv;
  if (!priv) return;
  if (priv->cart_ram) free(priv->cart_ram);
  if (priv->rom) free(priv->rom);
  priv->cart_ram = NULL;
  priv->rom = NULL;
}

#if ENABLE_LCD
void lcd_draw_line(struct gb_s *gb, const uint8_t pixels[160], const uint_fast8_t line) {
  uint16_t (*fb565)[LCD_WIDTH] = ((priv_t *)gb->direct.priv)->fb;

#if USE_NATIVE_GB_HEIGHT
  const int yplot = (int)line; // 0..143
  if (yplot < 0 || yplot >= DEST_H) return;
#else
  const int yplot = (int)line * DEST_H / LCD_HEIGHT; // 144 -> 135
  if (yplot < 0 || yplot >= DEST_H) return;
#endif

  if (gb->cgb.cgbMode) {
    for (unsigned int x = 0; x < LCD_WIDTH; x++)
      fb565[yplot][x] = gb->cgb.fixPalette[pixels[x]];
  }
#if WALNUT_GB_12_COLOUR
  else {
    for (unsigned int x = 0; x < LCD_WIDTH; x++)
      fb565[yplot][x] = CURRENT_PALETTE_RGB565[ ((pixels[x] & 18) >> 1) | (pixels[x] & 3) ];
  }
#else
  else {
    for (unsigned int x = 0; x < LCD_WIDTH; x++)
      fb565[yplot][x] = CURRENT_PALETTE_RGB565[(pixels[x]) & 3];
  }
#endif
}

static inline void present_frame_external(uint16_t fb[DEST_H][LCD_WIDTH]) {
  // Screen size depends on env (ILI9341 vs ILI9488) and rotation.
  const int screenW = tft.width();
  const int screenH = tft.height();

  // Center 160xDEST_H inside external screen
  const int x0 = (screenW - LCD_WIDTH) / 2;
  const int y0 = (screenH - DEST_H) / 2;

  tft.startWrite();
  tft.setAddrWindow(x0, y0, LCD_WIDTH, DEST_H);
  tft.pushPixels((uint16_t*)fb, (uint32_t)LCD_WIDTH * (uint32_t)DEST_H);
  tft.endWrite();
}
#endif

// -------------------------
// File picker (INTERNAL display only)
char* file_picker() {
  File root_dir = SD.open("/");
  if (!root_dir) { debugPrint("SD open / failed"); return NULL; }

  String file_list[MAX_FILES];
  int file_list_size = 0;

  while (1) {
    File file_entry = root_dir.openNextFile();
    if (!file_entry) break;

    if (!file_entry.isDirectory()) {
      String file_name = file_entry.name();
      String file_extension = file_name.substring(file_name.lastIndexOf(".") + 1);
      file_extension.toLowerCase();
      if (file_extension.equals("gb")) {
        if (file_list_size < MAX_FILES) file_list[file_list_size++] = file_name;
      }
    }
    file_entry.close();
  }
  root_dir.close();

  if (file_list_size == 0) { debugPrint("No .gb in /"); return NULL; }

  bool file_picked = false;
  int select_index = 0;

  M5Cardputer.Display.clearDisplay();

  char* file_list_cstr[MAX_FILES];
  for (int i = 0; i < file_list_size; i++) {
    file_list_cstr[i] = (char*)malloc(sizeof(char) * MAX_FILES);
    if (file_list_cstr[i]) file_list[i].toCharArray(file_list_cstr[i], MAX_FILES);
  }

  while (!file_picked) {
    M5Cardputer.update();

    if (M5Cardputer.Keyboard.isPressed()) {
      Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
      for (auto i : status.word) {
        switch (i) {
          case ';': select_index--; delay(200); break;
          case '.': select_index++; delay(200); break;
          default: break;
        }
      }
      if (status.enter) file_picked = true;
    }

    if (select_index < 0) select_index = file_list_size - 1;
    else if (select_index > file_list_size - 1) select_index = 0;

    M5Cardputer.Display.clearDisplay();

    const int dispW = M5Cardputer.Display.width();
    const int dispH = M5Cardputer.Display.height();

    for (int i = 0; i < file_list_size; i++) {
      if (!file_list_cstr[i]) continue;

      if (select_index == i) {
        set_font_size(64);
        int textW = M5Cardputer.Display.textWidth(file_list_cstr[i]);
        int textH = M5Cardputer.Display.fontHeight();
        M5Cardputer.Display.drawString(" > ", 0, (dispH/2) - (textH/2));
        M5Cardputer.Display.drawString(file_list_cstr[i], (dispW/2) - (textW/2), (dispH/2) - (textH/2));
      } else if (i == select_index - 1) {
        set_font_size(128);
        int textW = M5Cardputer.Display.textWidth(file_list_cstr[i]);
        int textH = M5Cardputer.Display.fontHeight();
        M5Cardputer.Display.drawString(file_list_cstr[i], (dispW/2) - (textW/2),
                                       (dispH/2) - (textH/2) - textH*2);
      } else if (i == select_index + 1) {
        set_font_size(128);
        int textW = M5Cardputer.Display.textWidth(file_list_cstr[i]);
        int textH = M5Cardputer.Display.fontHeight();
        M5Cardputer.Display.drawString(file_list_cstr[i], (dispW/2) - (textW/2),
                                       (dispH/2) - (textH/2) + textH*2);
      }
    }
  }

  char* selected_path = (char*)malloc(sizeof(char) * (MAX_FILES + 2));
  if (!selected_path) return NULL;
  sprintf(selected_path, "/%s", file_list_cstr[select_index]);

  for (int i = 0; i < file_list_size; i++) {
    if (file_list_cstr[i]) free(file_list_cstr[i]);
  }

  return selected_path;
}

void setup() {
  update_palette();

  // 1) External TFT first: fully configured by injected tft_setup.h
  tft.init();
  tft.setRotation(3);      // your established landscape orientation
  tft.fillScreen(TFT_BLACK);

  // 2) Internal Cardputer init (UI + keyboard)
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);

#if ENABLE_SOUND
  M5Cardputer.Speaker.begin();
#endif

  M5Cardputer.Display.setRotation(1);
  set_font_size(64);

  // 3) SD init on Cardputer pins
  debugPrint("Waiting for SD...");
  SPI2.begin(
    M5.getPin(m5::pin_name_t::sd_spi_sclk),
    M5.getPin(m5::pin_name_t::sd_spi_miso),
    M5.getPin(m5::pin_name_t::sd_spi_mosi),
    M5.getPin(m5::pin_name_t::sd_spi_ss)
  );
  while (false == SD.begin(M5.getPin(m5::pin_name_t::sd_spi_ss), SPI2)) {
    delay(1);
  }

  // 4) Pick ROM on internal display
  debugPrint("Pick ROM (.gb) ...");
  char* selected_file = file_picker();
  if (!selected_file) { debugPrint("No ROM selected"); while (1) delay(1000); }
  debugPrint(selected_file);

  // 5) Init emulation core
  static struct gb_s gb;
  static struct priv_t priv;

  priv.rom = read_rom_to_ram(selected_file);
  if (!priv.rom) { debugPrint("ROM read failed"); while (1) delay(1000); }

  enum gb_init_error_e ret = gb_init(&gb,
                                     &gb_rom_read,
                                     &gb_rom_read_16bit,
                                     &gb_rom_read_32bit,
                                     &gb_cart_ram_read,
                                     &gb_cart_ram_write,
                                     &gb_error,
                                     &priv);
  if (ret != GB_INIT_NO_ERROR) { debugPrint("gb_init failed"); while (1) delay(1000); }

  priv.cart_ram = (uint8_t*)malloc(gb_get_save_size(&gb));
  if (!priv.cart_ram) { debugPrint("save alloc failed"); while (1) delay(1000); }

#if ENABLE_LCD
  gb_init_lcd(&gb, &lcd_draw_line);
  gb.direct.interlace = 0;
#endif

  M5Cardputer.Display.clearDisplay();

  // 6) Emulator loop
  const double target_speed_us = 1000000.0 / VERTICAL_SYNC;

  while (1) {
    unsigned long start_us, end_us;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    start_us = (long)tv.tv_sec * 1000000 + (long)tv.tv_usec;

    gb.direct.joypad = 0xff;

    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isPressed()) {
      Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
      for (auto i : status.word) {
        switch (i) {
          case 'e': gb.direct.joypad_bits.up     = 0; break;
          case 'a': gb.direct.joypad_bits.left   = 0; break;
          case 's': gb.direct.joypad_bits.down   = 0; break;
          case 'd': gb.direct.joypad_bits.right  = 0; break;
          case 'k': gb.direct.joypad_bits.b      = 0; break;
          case 'l': gb.direct.joypad_bits.a      = 0; break;
          case '1': gb.direct.joypad_bits.start  = 0; break;
          case '2': gb.direct.joypad_bits.select = 0; break;
          default: break;
        }
      }
    }

    gb_run_frame_dualfetch(&gb);

#if ENABLE_LCD
    present_frame_external(priv.fb);
#endif

    gettimeofday(&tv, NULL);
    end_us = (long)tv.tv_sec * 1000000 + (long)tv.tv_usec;

    int_fast16_t delay_us = (int_fast16_t)(target_speed_us - (double)(end_us - start_us));
    if (delay_us > 0) usleep(delay_us);
  }
}

void loop() {}
