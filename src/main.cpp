/* Created by Gemini */

/*
 * Cardputer-Adv Dual-Screen Game Boy Emulator (Walnut-CGB) - GBC SOUND ENGINE
 *
 * NEW ARCHITECTURE:
 * - AUDIO: Uses "gbc_sound" engine (Ring Buffer + Critical Sections).
 * - Faster than FreeRTOS Queues.
 * - Auto-downmix Stereo -> Mono for M5Cardputer speaker.
 * * CONFIG:
 * - SRAM Safe (Single Buffer).
 * - Frame Skip 1:4.
 */

#define ENABLE_SOUND 1
#define ENABLE_LCD   1

#define MAX_FILES 400
#define INDEX_FILENAME ".roms.idx"

// Frame Skip 4 (Draw 1, Skip 4)
#define FRAME_SKIP_COUNT 4

#include <Arduino.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "M5Cardputer.h"
#include "SD.h"

#include "gbc_sound.h" // NEW AUDIO ENGINE

// Forward declare Walnut hooks (Using Original Context-based APU)
#if ENABLE_SOUND
extern "C" uint8_t audio_read(uint16_t addr);
extern "C" void    audio_write(uint16_t addr, uint8_t val);
#endif

#include "walnut_cgb.h"

#ifndef ILI9341_DRIVER
#define ILI9341_DRIVER
#endif

#include "tft_setup.h"
#include <TFT_eSPI.h>

#if ENABLE_SOUND
  // Original APU includes
  #ifndef MINIGB_APU_AUDIO_FORMAT_S16SYS
    #define MINIGB_APU_AUDIO_FORMAT_S16SYS
  #endif
  extern "C" {
    #include "minigb_apu.h"
  }
  
  // Define Sample count if missing
  #ifndef AUDIO_SAMPLES
    #define AUDIO_SAMPLES 548
  #endif
#endif

// -------------------------
// Video target
// -------------------------
#define USE_NATIVE_GB_HEIGHT 1
#if USE_NATIVE_GB_HEIGHT
  #define DEST_H 144
#else
  #define DEST_H 135
#endif

#define FB_SIZE (LCD_WIDTH * DEST_H * 2)

SPIClass SPI2;
TFT_eSPI tft = TFT_eSPI();

static volatile bool g_do_rendering = true; 

// -------------------------
// Palette
// -------------------------
static inline uint16_t rgb888_to_rgb565(uint32_t rgb) {
  return (uint16_t)(
    ((rgb >> 8)  & 0xF800) |
    ((rgb >> 5)  & 0x07E0) |
    ((rgb >> 3)  & 0x001F)
  );
}

#if WALNUT_GB_12_COLOUR
uint32_t gboriginal_palette[] = { 0x7B8210, 0x5A7942, 0x39594A, 0x294139, 0x7B8210, 0x5A7942, 0x39594A, 0x294139, 0x7B8210, 0x5A7942, 0x39594A, 0x294139 };
uint16_t CURRENT_PALETTE_RGB565[12];
static inline void update_palette() { for (int i = 0; i < 12; i++) CURRENT_PALETTE_RGB565[i] = rgb888_to_rgb565(gboriginal_palette[i]); }
#else
uint32_t gboriginal_palette[] = { 0x7B8210, 0x5A7942, 0x39594A, 0x294139 };
uint16_t CURRENT_PALETTE_RGB565[4];
static inline void update_palette() { for (int i = 0; i < 4; i++) CURRENT_PALETTE_RGB565[i] = rgb888_to_rgb565(gboriginal_palette[i]); }
#endif

// -------------------------
// UI helper
// -------------------------
static inline void uiStatusScreen(const String& l0, const String& l1 = "", const String& l2 = "", const String& l3 = "") {
  M5Cardputer.Display.clearDisplay();
  M5Cardputer.Display.setTextDatum(TL_DATUM);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.drawString(l0, 0, 0);
  if (l1.length()) M5Cardputer.Display.drawString(l1, 0, 14);
  if (l2.length()) M5Cardputer.Display.drawString(l2, 0, 28);
  if (l3.length()) M5Cardputer.Display.drawString(l3, 0, 42);
}

// -------------------------
// PERF DEBUG
// -------------------------
static inline uint64_t now_us() { return (uint64_t)esp_timer_get_time(); }
static volatile uint32_t dbg_frames = 0;
static volatile uint32_t dbg_draws = 0;
static uint32_t dbg_last_report_ms = 0;

#if ENABLE_SOUND
static minigb_apu_ctx g_apu;
// Buffer to hold raw stereo samples from APU
static int16_t g_apuStereoBuffer[AUDIO_SAMPLES * 2];
// Buffer to hold mono samples for gbc_sound
static int16_t g_apuMonoBuffer[AUDIO_SAMPLES];

extern "C" uint8_t audio_read(uint16_t addr) {
  if (addr >= 0xFF10 && addr <= 0xFF3F) return minigb_apu_audio_read(&g_apu, addr);
  return 0xFF;
}
extern "C" void audio_write(uint16_t addr, uint8_t val) {
  if (addr >= 0xFF10 && addr <= 0xFF3F) minigb_apu_audio_write(&g_apu, addr, val);
}
#endif

// ============================================================================
// ROM FileManager
// ============================================================================
#define ROMS_ROOT  "/roms"
#define POS_FILE   "/roms/last_pos.txt"

struct FileItem { String name; bool isDir; bool isUpdateCmd; };

class RomFileManager {
private:
  FileItem items[MAX_FILES];
  int count = 0;
  String currentPath = ROMS_ROOT;

  bool isRomFile(const String& name) {
    String s = name; s.toLowerCase();
    return s.endsWith(".gb") || s.endsWith(".gbc");
  }

  void ensureRomsRoot() { if (!SD.exists(ROMS_ROOT)) SD.mkdir(ROMS_ROOT); }

  bool isValidRomsDir(const String& p) {
    if (!p.startsWith(ROMS_ROOT)) return false;
    if (!SD.exists(p)) return false;
    File f = SD.open(p);
    bool ok = (f && f.isDirectory());
    if (f) f.close();
    return ok;
  }

  void saveLastPath() {
    ensureRomsRoot();
    File f = SD.open(POS_FILE, FILE_WRITE);
    if (f) { f.print(currentPath); f.close(); }
  }

  String loadLastPathOrDefault() {
    ensureRomsRoot();
    if (SD.exists(POS_FILE)) {
      File f = SD.open(POS_FILE, FILE_READ);
      if (f) {
        String s = f.readString(); s.trim(); f.close();
        if (isValidRomsDir(s)) return s;
      }
    }
    return String(ROMS_ROOT);
  }

  static bool isJunkName(const String& fn) {
    if (fn.equalsIgnoreCase("System Volume Information")) return true;
    if (fn.startsWith("._")) return true;
    if (fn == INDEX_FILENAME) return true;
    return false;
  }

  static int ciCompare(const String& a, const String& b) {
    String aa = a, bb = b;
    aa.toLowerCase(); bb.toLowerCase();
    if (aa < bb) return -1;
    if (aa > bb) return 1;
    return 0;
  }

  void sortTemp(FileItem* tmp, int n) {
    for (int i = 1; i < n; i++) {
      FileItem key = tmp[i];
      int j = i - 1;
      while (j >= 0) {
        bool keyDir = key.isDir;
        bool jDir   = tmp[j].isDir;
        bool swap = false;
        if (keyDir != jDir) swap = (keyDir && !jDir);
        else swap = (ciCompare(key.name, tmp[j].name) < 0);
        if (!swap) break;
        tmp[j + 1] = tmp[j];
        j--;
      }
      tmp[j + 1] = key;
    }
  }

  void drawScanRange(int startN, int endN) {
    M5Cardputer.Display.setTextDatum(MC_DATUM);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.fillRect(0, (M5Cardputer.Display.height()/2) - 10,
                                 M5Cardputer.Display.width(), 20, TFT_BLACK);
    String msg = "Scanning " + String(startN) + "-" + String(endN) + "...";
    M5Cardputer.Display.drawString(msg, M5Cardputer.Display.width()/2, M5Cardputer.Display.height()/2);
    M5Cardputer.Display.setTextDatum(TL_DATUM);
  }

  void generateIndex(const String& path) {
    M5Cardputer.Display.clearDisplay();
    M5Cardputer.Display.setTextDatum(MC_DATUM);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.drawString("Initializing...", M5Cardputer.Display.width()/2, M5Cardputer.Display.height()/2);

    String idxPath = path + "/" + INDEX_FILENAME;
    if (SD.exists(idxPath)) SD.remove(idxPath);

    File idxFile = SD.open(idxPath, FILE_WRITE);
    if (!idxFile) return;

    File root = SD.open(path);
    if (!root || !root.isDirectory()) {
      if (root) root.close();
      idxFile.close();
      return;
    }

    FileItem tmp[MAX_FILES];
    int tmpCount = 0;
    int scanned = 0;

    M5Cardputer.Display.clearDisplay();
    M5Cardputer.Display.drawString("Scanning 1-50...", M5Cardputer.Display.width()/2, M5Cardputer.Display.height()/2);

    for (File f = root.openNextFile(); f && tmpCount < MAX_FILES; ) {
      scanned++;
      bool isdir = f.isDirectory();
      String fn = String(f.name());
      int lastSlash = fn.lastIndexOf('/');
      if (lastSlash != -1) fn = fn.substring(lastSlash + 1);
      f.close();
      delay(2);

      if (isJunkName(fn)) {
        if (scanned % 50 == 1 && scanned > 1) drawScanRange(scanned, scanned + 49);
        f = root.openNextFile();
        continue;
      }

      if (isdir) tmp[tmpCount++] = { fn, true, false };
      else if (isRomFile(fn)) tmp[tmpCount++] = { fn, false, false };

      if (scanned % 50 == 1 && scanned > 1) drawScanRange(scanned, scanned + 49);
      f = root.openNextFile();
    }
    root.close();

    if (tmpCount > 1) sortTemp(tmp, tmpCount);

    for (int i = 0; i < tmpCount; i++) {
      if (tmp[i].isDir) { idxFile.print("D:"); idxFile.println(tmp[i].name); }
      else              { idxFile.print("F:"); idxFile.println(tmp[i].name); }
    }
    idxFile.close();
    M5Cardputer.Display.setTextDatum(TL_DATUM);
  }

public:
  void begin() { ensureRomsRoot(); loadDirectory(loadLastPathOrDefault()); }
  void updateCurrentIndex() { generateIndex(currentPath); loadDirectory(currentPath); }

  void loadDirectory(String path) {
    if (!path.startsWith(ROMS_ROOT)) path = ROMS_ROOT;
    if (!SD.exists(path)) path = ROMS_ROOT;

    currentPath = path;
    count = 0;

    if (currentPath != String(ROMS_ROOT) && count < MAX_FILES) items[count++] = { "..", true, false };
    if (count < MAX_FILES) items[count++] = { "[ UPDATE ROM LIST ]", false, true };

    String idxPath = path + "/" + INDEX_FILENAME;
    if (SD.exists(idxPath)) {
      File idxFile = SD.open(idxPath, FILE_READ);
      if (idxFile) {
        while (idxFile.available() && count < MAX_FILES) {
          String line = idxFile.readStringUntil('\n');
          line.trim();
          if (line.length() > 2) {
            char type = line.charAt(0);
            String name = line.substring(2);
            if (!name.length() || isJunkName(name)) continue;
            if (type == 'D') items[count++] = { name, true, false };
            else if (type == 'F') items[count++] = { name, false, false };
          }
        }
        idxFile.close();
      }
    }
    saveLastPath();
  }

  bool handleSelection(int index, String &outFilePath) {
    if (index < 0 || index >= count) return false;
    FileItem sel = items[index];

    if (sel.isUpdateCmd) { updateCurrentIndex(); return false; }

    if (sel.isDir) {
      if (sel.name == "..") {
        int lastSlash = currentPath.lastIndexOf('/');
        String parent = (lastSlash <= 0) ? String(ROMS_ROOT) : currentPath.substring(0, lastSlash);
        if (!parent.startsWith(ROMS_ROOT)) parent = ROMS_ROOT;
        currentPath = parent;
      } else {
        String next = (currentPath == String(ROMS_ROOT)) ? (String(ROMS_ROOT) + "/" + sel.name) : (currentPath + "/" + sel.name);
        if (!next.startsWith(ROMS_ROOT)) next = ROMS_ROOT;
        currentPath = next;
      }
      loadDirectory(currentPath);
      return false;
    }

    outFilePath = (currentPath == String(ROMS_ROOT)) ? (String(ROMS_ROOT) + "/" + sel.name) : (currentPath + "/" + sel.name);
    return true;
  }

  int getCount() const { return count; }
  String getCurrentPath() const { return currentPath; }
  FileItem getItem(int index) const {
    if (index >= 0 && index < count) return items[index];
    return {"", false, false};
  }
};

static RomFileManager RFM;

static bool rom_picker(String &outPath) {
  RFM.begin();

  int sel = 0;
  int top = 0;

  M5Cardputer.Display.clearDisplay();
  M5Cardputer.Display.setTextWrap(false);
  M5Cardputer.Display.setTextDatum(TL_DATUM);
  M5Cardputer.Display.setTextSize(1);

  const int lineH = 14;
  const int headerH = 28;
  const int screenH = M5Cardputer.Display.height();
  const int visibleLines = (screenH - headerH) / lineH;

  auto redraw = [&]() {
    M5Cardputer.Display.clearDisplay();
    M5Cardputer.Display.setTextDatum(TC_DATUM);
    M5Cardputer.Display.setTextColor(TFT_ORANGE, TFT_BLACK);
    M5Cardputer.Display.drawString("GB Emu Dual v0.1 - Select rom", M5Cardputer.Display.width()/2, 0);

    M5Cardputer.Display.setTextDatum(TL_DATUM);
    M5Cardputer.Display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    M5Cardputer.Display.drawString(RFM.getCurrentPath(), 0, 14);

    int n = RFM.getCount();
    if (n <= 0) return;

    if (sel < top) top = sel;
    if (sel >= top + visibleLines) top = sel - visibleLines + 1;
    if (top < 0) top = 0;

    for (int row = 0; row < visibleLines; row++) {
      int idx = top + row;
      if (idx >= n) break;
      FileItem it = RFM.getItem(idx);
      String label = (it.isUpdateCmd) ? it.name : (it.isDir ? "[ " + it.name + " ]" : it.name);
      int y = headerH + row * lineH;
      if (idx == sel) {
        M5Cardputer.Display.setTextColor(TFT_BLACK, TFT_WHITE);
        M5Cardputer.Display.drawString("> " + label, 0, y);
      } else {
        M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
        M5Cardputer.Display.drawString("  " + label, 0, y);
      }
    }
    M5Cardputer.Display.setTextColor(TFT_DARKGREY, TFT_BLACK);
    M5Cardputer.Display.drawString(";/.: move  ENTER: sel  BKSP: up", 0, screenH - lineH);
  };

  redraw();

  while (true) {
    M5Cardputer.update();
    bool changed = false;
    if (M5Cardputer.Keyboard.isPressed()) {
      auto st = M5Cardputer.Keyboard.keysState();
      for (auto c : st.word) {
        if (c == ';') { sel--; changed = true; delay(140); }
        else if (c == '.') { sel++; changed = true; delay(140); }
      }
      if (st.del) {
        if (RFM.getCount() > 0) {
          FileItem it0 = RFM.getItem(0);
          if (it0.isDir && it0.name == "..") {
            sel = 0; String dummy; RFM.handleSelection(sel, dummy); sel = 0; top = 0; changed = true; delay(160);
          }
        }
      }
      if (st.enter) {
        int n = RFM.getCount();
        if (n > 0) {
          if (sel < 0) sel = n - 1; 
          else if (sel >= n) sel = 0;
          String picked;
          if (RFM.handleSelection(sel, picked)) { outPath = picked; return true; }
          else { sel = 0; top = 0; changed = true; delay(160); }
        }
      }
    }
    if (changed) {
      int n = RFM.getCount();
      if (n > 0) { if (sel < 0) sel = n - 1; if (sel >= n) sel = 0; } else sel = 0;
      redraw();
    }
  }
}

// ============================================================================
// Emulator core
// ============================================================================
struct priv_t {
  uint8_t *rom;
  uint8_t *cart_ram;
  uint16_t* fb;
};

static uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr) {
  const struct priv_t * const p = (const struct priv_t *)gb->direct.priv;
  return p->rom[addr];
}
static uint16_t gb_rom_read_16bit(struct gb_s *gb, const uint_fast32_t addr) {
  const uint8_t *src = &((const struct priv_t *)gb->direct.priv)->rom[addr];
  if ((uintptr_t)src & 1) return ((uint16_t)src[0]) | ((uint16_t)src[1] << 8);
  return *(uint16_t *)src;
}
static uint32_t gb_rom_read_32bit(struct gb_s *gb, const uint_fast32_t addr) {
  const uint8_t *src = &((const struct priv_t *)gb->direct.priv)->rom[addr];
  if ((uintptr_t)src & 3) return ((uint32_t)src[0]) | ((uint32_t)src[1] << 8) | ((uint32_t)src[2] << 16) | ((uint32_t)src[3] << 24);
  return *(uint32_t *)src;
}
static uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr) {
  const struct priv_t * const p = (const struct priv_t *)gb->direct.priv;
  return p->cart_ram ? p->cart_ram[addr] : 0xFF;
}
static void gb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr, const uint8_t val) {
  const struct priv_t * const p = (const struct priv_t *)gb->direct.priv;
  if (p->cart_ram) p->cart_ram[addr] = val;
}
static void gb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t val) {
  struct priv_t *priv = (struct priv_t *)gb->direct.priv;
  if (priv) { if (priv->cart_ram) free(priv->cart_ram); if (priv->rom) free(priv->rom); priv->cart_ram = NULL; priv->rom = NULL; }
}

static uint8_t *read_rom_to_ram(const char *file_name) {
  uiStatusScreen("Loading ROM to RAM...", file_name);
  Serial.printf("[Gemini] Opening ROM: %s\n", file_name);

  File rom_file = SD.open(file_name);
  if (!rom_file) {
    Serial.println("[Gemini] Failed to open file!");
    return NULL;
  }

  size_t rom_size = rom_file.size();
  Serial.printf("[Gemini] ROM Size: %u bytes\n", rom_size);
  if (rom_size == 0) { rom_file.close(); return NULL; }

  uint8_t* readRom = (uint8_t*)malloc(rom_size);
  if (!readRom) {
    Serial.println("[Gemini] Malloc failed! Not enough RAM.");
    uiStatusScreen("Error", "RAM Full");
    rom_file.close(); 
    return NULL; 
  }

  size_t bytesRead = 0;
  const size_t chunkSize = 1024;
  while (bytesRead < rom_size) {
    size_t toRead = rom_size - bytesRead;
    if (toRead > chunkSize) toRead = chunkSize;
    if (rom_file.read(readRom + bytesRead, toRead) != toRead) {
      Serial.printf("[Gemini] READ ERROR at offset %u.\n", bytesRead);
      free(readRom);
      rom_file.close();
      return NULL;
    }
    bytesRead += toRead;
  }

  rom_file.close();
  Serial.println("[Gemini] ROM loaded successfully.");
  return readRom;
}

#if ENABLE_LCD
static void lcd_draw_line(struct gb_s *gb, const uint8_t pixels[160], const uint_fast8_t line) {
  if (!g_do_rendering) return;

  uint16_t* fb_ptr = ((priv_t *)gb->direct.priv)->fb;
  if (!fb_ptr) return;

  #if USE_NATIVE_GB_HEIGHT
  const int yplot = (int)line;
  #else
  const int yplot = (int)line * DEST_H / LCD_HEIGHT;
  #endif
  if (yplot < 0 || yplot >= DEST_H) return;

  uint16_t* line_ptr = &fb_ptr[yplot * LCD_WIDTH];

  if (gb->cgb.cgbMode) {
    for (unsigned int x = 0; x < LCD_WIDTH; x++) line_ptr[x] = gb->cgb.fixPalette[pixels[x]];
  }
  #if WALNUT_GB_12_COLOUR
  else {
    for (unsigned int x = 0; x < LCD_WIDTH; x++) line_ptr[x] = CURRENT_PALETTE_RGB565[ ((pixels[x] & 18) >> 1) | (pixels[x] & 3) ];
  }
  #else
  else {
    for (unsigned int x = 0; x < LCD_WIDTH; x++) line_ptr[x] = CURRENT_PALETTE_RGB565[(pixels[x]) & 3];
  }
  #endif
}

static inline void present_frame_external(uint16_t* fb) {
  if (!fb) return;
  const int screenW = tft.width();
  const int screenH = tft.height();
  const int x0 = (screenW - LCD_WIDTH) / 2;
  const int y0 = (screenH - DEST_H) / 2;

  tft.setSwapBytes(true);
  tft.pushImage(x0, y0, LCD_WIDTH, DEST_H, fb);
  tft.setSwapBytes(false);
}
#endif

// Report debug stats every second
static void dbg_report_1hz() {
  uint32_t now = millis();
  if (now - dbg_last_report_ms < 1000) return;
  dbg_last_report_ms = now;

  Serial.printf("\n[Gemini] ===== 1s PERF =====\n");
  Serial.printf("[Gemini] LOGIC FPS: %lu  DRAW FPS: %lu\n", (unsigned long)dbg_frames, (unsigned long)dbg_draws);
  dbg_frames = 0;
  dbg_draws = 0;
}

void setup() {
  Serial.begin(115200);
  delay(100);

  setCpuFrequencyMhz(240);
  Serial.printf("[Gemini] CPU Freq: %d MHz\n", getCpuFrequencyMhz());
  Serial.printf("[Gemini] Free Internal Heap: %u\n", ESP.getFreeHeap());

  update_palette();

  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextDatum(TL_DATUM);
  M5Cardputer.Display.setTextSize(1);

  // Audio setup with GBC SOUND ENGINE
#if ENABLE_SOUND
  uiStatusScreen("Booting...", "Init Audio (RingBuffer)...");
  // Init GBC sound engine
  gbc_sound_init(32768);
  minigb_apu_audio_init(&g_apu);
#endif

  uiStatusScreen("Booting...", "Init external TFT...");
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  
  uiStatusScreen("Booting...", "Init SD...");
  SPI2.begin(M5.getPin(m5::pin_name_t::sd_spi_sclk), M5.getPin(m5::pin_name_t::sd_spi_miso), M5.getPin(m5::pin_name_t::sd_spi_mosi), M5.getPin(m5::pin_name_t::sd_spi_ss));
  while (!SD.begin(M5.getPin(m5::pin_name_t::sd_spi_ss), SPI2, 10000000)) delay(200);

  String romPath;
  if (!rom_picker(romPath) || romPath.length() == 0) { uiStatusScreen("Error", "No ROM"); while (1) delay(1000); }
  uiStatusScreen("Loading ROM...", romPath);

  static struct gb_s gb;
  static struct priv_t priv;
  
  Serial.println("[Gemini] Allocating Single Framebuffer...");
  priv.fb = (uint16_t*)malloc(FB_SIZE);
  if (!priv.fb) {
      Serial.println("[Gemini] CRITICAL: FB malloc failed!");
      uiStatusScreen("Error", "FB Alloc Fail");
      while(1) delay(1000);
  }

  priv.rom = read_rom_to_ram(romPath.c_str());
  if (!priv.rom) { uiStatusScreen("Error", "ROM read failed"); while (1) delay(1000); }

  gb_init(&gb, &gb_rom_read, &gb_rom_read_16bit, &gb_rom_read_32bit, &gb_cart_ram_read, &gb_cart_ram_write, &gb_error, &priv);
  
  gb.direct.interlace = 1;

  uint_fast32_t save_size = 0;
  if (gb_get_save_size_s(&gb, &save_size) == 0 && save_size > 0) {
    priv.cart_ram = (uint8_t*)malloc((size_t)save_size);
    memset(priv.cart_ram, 0, (size_t)save_size);
  } else priv.cart_ram = NULL;

#if ENABLE_LCD
  gb_init_lcd(&gb, &lcd_draw_line);
#endif

  M5Cardputer.Display.clearDisplay();

  const uint32_t frame_budget_us = 16666;
  int skip_counter = 0;
  int input_throttle = 0;
  
  while (1) {
    uint32_t now = micros();
    
    // 1. Input Logic
    if (input_throttle++ >= 3) {
        input_throttle = 0;
        gb.direct.joypad = 0xff;
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isPressed()) {
          Keyboard_Class::KeysState st = M5Cardputer.Keyboard.keysState();
          for (auto i : st.word) {
            if (i == 'e') gb.direct.joypad_bits.up = 0;
            else if (i == 'a') gb.direct.joypad_bits.left = 0;
            else if (i == 's') gb.direct.joypad_bits.down = 0;
            else if (i == 'd') gb.direct.joypad_bits.right = 0;
            else if (i == 'k') gb.direct.joypad_bits.b = 0;
            else if (i == 'l') gb.direct.joypad_bits.a = 0;
            else if (i == '1') gb.direct.joypad_bits.start = 0;
            else if (i == '2') gb.direct.joypad_bits.select = 0;
          }
        }
    }

    // 2. Decide if we render
    g_do_rendering = (skip_counter == 0);

    // 3. Run Emulator
    gb_run_frame_dualfetch(&gb);
    dbg_frames++;

    // 4. Audio - GBC SOUND ENGINE INTEGRATION
#if ENABLE_SOUND
    // 1. Generate stereo samples from APU to g_apuStereoBuffer
    minigb_apu_audio_callback(&g_apu, (audio_sample_t*)g_apuStereoBuffer);
    
    // 2. Downmix Stereo (Interleaved) -> Mono for gbc_sound ring buffer
    // Loop through the stereo buffer (L, R, L, R...)
    for (int i = 0; i < AUDIO_SAMPLES; i++) {
        int32_t L = g_apuStereoBuffer[i * 2];
        int32_t R = g_apuStereoBuffer[i * 2 + 1];
        // Average
        g_apuMonoBuffer[i] = (int16_t)((L + R) / 2);
    }

    // 3. Submit mono samples to the ring buffer
    gbc_sound_submit(g_apuMonoBuffer, AUDIO_SAMPLES);
#endif

    // 5. Draw
    if (g_do_rendering) {
#if ENABLE_LCD
      present_frame_external(priv.fb);
      dbg_draws++;
#endif
    }

    // Advance skip counter
    skip_counter++;
    if (skip_counter > FRAME_SKIP_COUNT) skip_counter = 0;

    uint32_t end_frame = micros();
    uint32_t elapsed = end_frame - now;
    if (elapsed < frame_budget_us) {
       delayMicroseconds(frame_budget_us - elapsed);
    }
    
    dbg_report_1hz();
  }
}

void loop() {}