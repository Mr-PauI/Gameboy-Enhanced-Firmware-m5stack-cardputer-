// Sound is implemented in SDL2 example
#define ENABLE_SOUND 0
#define ENABLE_LCD 1

#define MAX_FILES 256

#include "M5Cardputer.h"
#include "../../walnut_cgb.h"
#include "SD.h"

#define DEST_W 240
#define DEST_H 135

#define DEBUG_DELAY 0

#define DISPLAY_CENTER(x) x + (DEST_W/2 - LCD_WIDTH/2)

// SD card SPI class.
SPIClass SPI2;

static inline uint16_t rgb888_to_rgb565(uint32_t rgb) {
    return (uint16_t)(
        ((rgb >> 8)  & 0xF800) |   // red
        ((rgb >> 5)  & 0x07E0) |   // green
        ((rgb >> 3)  & 0x001F)     // blue
    );
}

#if WALNUT_GB_12_COLOUR
uint32_t gboriginal_palette[] = { 0x7B8210, 0x5A7942, 0x39594A, 0x294139 , 0x7B8210, 0x5A7942, 0x39594A, 0x294139, 0x7B8210, 0x5A7942, 0x39594A, 0x294139  }; // rgb888 palette x3, must manually be set for each game for 12 colors
uint16_t CURRENT_PALETTE_RGB565[12];  
void update_palette()
{
  for (int i=0;i<12;i++)
    CURRENT_PALETTE_RGB565[i]=rgb888_to_rgb565(gboriginal_palette[i]);
}
#else
uint32_t gboriginal_palette[] = { 0x7B8210, 0x5A7942, 0x39594A, 0x294139 }; // rgb888 palette
uint16_t CURRENT_PALETTE_RGB565[4];
void update_palette()
{
  for (int i=0;i<4;i++)
    CURRENT_PALETTE_RGB565[i]=rgb888_to_rgb565(gboriginal_palette[i]);
}
#endif



// Prints debug info to the display.
void debugPrint(const char* str) {
  M5Cardputer.Display.clearDisplay();
  M5Cardputer.Display.drawString(str, 0, 0);
#if DEBUG_DELAY
  delay(500);
#endif
}

// Walnut-CGB structures and functions.
struct priv_t
{
	/* Pointer to allocated memory holding GB file. */
	uint8_t *rom;
	/* Pointer to allocated memory holding save file. */
	uint8_t *cart_ram;

	/* Frame buffer */
	uint16_t fb[LCD_HEIGHT][LCD_WIDTH];
};

/**
 * Returns a byte from the ROM file at the given address.
 */
uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr)
{
	const struct priv_t * const p = (const struct priv_t *)gb->direct.priv;
	return p->rom[addr];
}

/**
 * Returns a two bytes from the ROM file at the given address.
 */
uint16_t gb_rom_read_16bit(struct gb_s *gb,const uint_fast32_t addr) {
      const uint8_t *src = &((const struct priv_t *)gb->direct.priv)->rom[addr];
      // Alignment check, not required for all platforms. ESP32 series mcu flash memory and psram sources *require* this
      if ((uintptr_t)src & 1) {
          // fallback to safe 8-bit reads when not aligned
          return ((uint16_t)src[0]) | ((uint16_t)src[1] << 8);          
      } 
      return *(uint16_t *)src;
  /* ISO C version below, above may require -fno-strict-aliasing */
  /*
      const uint8_t *src = &((const struct priv_t *)gb->direct.priv)->rom[addr];
      uint16_t val;
      memcpy(&val, src, sizeof(val));
      return val;
  */
}

/**
 * Returns four bytes from the ROM file at the given address.
 */
uint32_t gb_rom_read_32bit(struct gb_s *gb, const uint_fast32_t addr) {
    const uint8_t *src =
        &((const struct priv_t *)gb->direct.priv)->rom[addr];

    // Alignment check: ESP32 flash / PSRAM require 32-bit alignment
    if ((uintptr_t)src & 3) {
        // fallback to safe 8-bit reads when not aligned
        return ((uint32_t)src[0]) |
               ((uint32_t)src[1] << 8) |
               ((uint32_t)src[2] << 16) |
               ((uint32_t)src[3] << 24);
    }

    return *(uint32_t *)src;

    /* ISO C version below, above may require -fno-strict-aliasing */
    /*
    uint32_t val;
    memcpy(&val, src, sizeof(val));
    return val;
    */
}

/**
 * Returns a byte from the cartridge RAM at the given address.
 */
uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr)
{
	const struct priv_t * const p = (const struct priv_t *)gb->direct.priv;
	return p->cart_ram[addr];
}

/**
 * Writes a given byte to the cartridge RAM at the given address.
 */
void gb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr,
		       const uint8_t val)
{
	const struct priv_t * const p = (const struct priv_t *)gb->direct.priv;
	p->cart_ram[addr] = val;
}

/**
 * Returns a pointer to the allocated space containing the ROM. Must be freed.
 */
uint8_t *read_rom_to_ram(const char *file_name) {
  // Open file from the SD card.
  File rom_file = SD.open(file_name);
  size_t rom_size;
  char *readRom = NULL;

  rom_size = rom_file.size();

  if(rom_size == NULL)
    return NULL;

  readRom = (char*)malloc(rom_size);

  if(rom_file.readBytes(readRom, rom_size) != rom_size) {
    free(readRom);
    rom_file.close();
    return NULL;
  }

  uint8_t *rom = (uint8_t*)readRom;
  char debugRomStr[100];
  // Print first and last byte of the ROM for debugging purposes.
  sprintf(debugRomStr, "f: %02x | l: %02x", rom[0], rom[rom_size-1]);
  debugPrint(debugRomStr);
  rom_file.close();
  return rom;
}

/**
 * Ignore all errors.
 */
void gb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t val) {
  const char* gb_err_str[GB_INVALID_MAX] = {
    "UNKNOWN",
    "INVALID OPCODE",
    "INVALID READ",
    "INVALID WRITE",
    "HATL FOREVER"
  };

	struct priv_t * priv = (struct priv_t *)gb->direct.priv;

  free(priv->cart_ram);
  free(priv->rom);
}



#if ENABLE_LCD
/**
 * Draws scanline into framebuffer.
 */
void lcd_draw_line(struct gb_s *gb, const uint8_t pixels[160],  
      const uint_fast8_t line) {  
      int yplot=line * DEST_H / LCD_HEIGHT; // (optional) this is our scaling calculation, done once per horizontal line.  
                                            // Skipping non-visible lines provides a small boost. LCD_HEIGHT is defined in walnut-cgb.h
      uint16_t (*fb565)[LCD_WIDTH] = ((priv_t *)gb->direct.priv)->fb;
 	if (gb->cgb.cgbMode)    
 	{ // Gameboy Color RGB565 rendering   
 		for (unsigned int x = 0; x < LCD_WIDTH; x++)    
 		{    
                    fb565[yplot][x] = gb->cgb.fixPalette[pixels[x]]; // map colours from current palette   
 		}    
 	}
#if WALNUT_GB_12_COLOUR  
  else  // some titles have 12-color mappings. Some common mappings can be found in the sgb.h file
  { // Using 12-colour palette  
        for (unsigned int x = 0; x < LCD_WIDTH; x++)        
              fb565[yplot][x]=CURRENT_PALETTE_RGB565[ ((pixels[x]&18)>>1) | (pixels[x]&3) ]; // A 12-element uint16_t array  
  }  
#else
 	else
  { // Using a 4 colour palette  
        for (unsigned int x = 0; x < LCD_WIDTH; x++)  
          fb565[yplot][x]=CURRENT_PALETTE_RGB565[ (pixels[x])&3] ; // A 4-element uint16_t array  
  }  
#endif
}  

// Draw a frame to the display while scaling it to fit.
// This is needed as the Cardputer's display has a height of 135px,
// while the GameBoy's has a height of 144px.
void fit_frame(uint16_t fb[144][160]) {
  M5Cardputer.Display.drawBitmap(40,0,160,135,fb[0]); // Push the entire image to the screen
  // if using interlacing this can be done line by line in sync with the rendering for increased speeed
  // additionally using the second core to handle video(and audio) on ESP32/ESP32-s3 can offer another
  // noticeable improvement to performance
}
#endif


void set_font_size(int size) {
  int textsize = M5Cardputer.Display.height() / size;
  if(textsize == 0) {
    textsize = 1;
  }
  M5Cardputer.Display.setTextSize(textsize);
}

// Opens a ROM file picker menu
// and returns a string containing
// the path of the picked ROM.
char* file_picker() {
  // Open SD card root dir.
  File root_dir = SD.open("/");
  String file_list[MAX_FILES];
  int file_list_size = 0;

  // Look for .gb files in the root dir.
  while(1) {
    File file_entry = root_dir.openNextFile();
    // If we checked all files, stop searching
    if(!file_entry) {
      break;
    }

    // Chec if the entry is a file
    if(!file_entry.isDirectory()) {
      // Get the file extension
      String file_name = file_entry.name();
      String file_extension = file_name.substring(file_name.lastIndexOf(".") + 1);

      // Convert the file extension to lowercase
      //
      // WARNING: major yapping ahead
      //
      // Note: SD cards might have to be formatted as FAT32 
      // to work with this library; if that's the case then
      // doing this doesn't make a difference because FAT32
      // is case insensitive. However, the author of
      // https://github.com/shikarunochi/CardputerSimpleLaucher
      // (which much of the code in this function is edited from)
      // added this line so I guess I can't be too sure.
      // It's not like saving CPU cycles is important here
      // Since this is only called when the menu is first shown so
      //
      // (not like any of this code is particularly efficient)
      //
      // yapping sesh over
      file_extension.toLowerCase();

      if(!file_extension.equals("gb")) {
        continue;
      }

      // Add the ROM's filename to the array
      file_list[file_list_size] = file_name;
      file_list_size++;
    }

    file_entry.close();
  }

  root_dir.close();

  // Boolean to check if a file has been picked.
  // If so we should start the game (ofc lol)  
  bool file_picked = false;
  int select_index = 0;

  M5Cardputer.Display.clearDisplay();

  // This might be kinda stupid but
  // File.name() returns an Arduino-style
  // String object, when Peanut-GB being
  // written in C expects a plain old
  // char array in its `read_rom_to_ram`
  // callback, so we'll need to "convert" these strings
  char* file_list_cstr[MAX_FILES];
  for(int i = 0; i < file_list_size; i++) {
    file_list_cstr[i] = (char*)malloc(sizeof(char)*MAX_FILES);
    file_list[i].toCharArray(file_list_cstr[i], MAX_FILES);
  }

  // Menu loop
  while(!file_picked) {
    // Read Keyboard matrix
    M5Cardputer.update();
    if(M5Cardputer.Keyboard.isPressed()) {
      Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
      M5Cardputer.Display.clearDisplay();
      for(auto i : status.word) {
        // Controls:
        // Up: e
        // Down: s
        // Play: l
        switch(i) {
          case ';':
            select_index--;
            delay(300);
            break;
          case '.':
            select_index++;
            delay(300);
            break;
          default:
            break;
        }
      }

      if(status.enter) {
        file_picked = true;
      }
    }

    // Loop over list if we went over its bounds
    // e.g. user presses up on the first element
    // or down on the last one
    if(select_index < 0) {
      select_index = file_list_size-1;
    } else if(select_index > file_list_size-1) {
      select_index = 0;
    }

    // Render controls
    // M5Cardputer.Display.drawString("Up/Down: E/S; Select: L", 0, 0);

    //M5Cardputer.Display.setTextDatum(MC_DATUM);

    const int dispW = M5Cardputer.Display.width();
    const int dispH = M5Cardputer.Display.height();

    // Render list
    for(int i = 0; i < file_list_size; i++) {
      // Add an arrow to point to 
      // the currently selected file

      if(select_index == i) {
        set_font_size(64);
        int textW = M5Cardputer.Display.textWidth(file_list_cstr[i]);
        int textH = M5Cardputer.Display.fontHeight();

        M5Cardputer.Display.drawString(" > ", 0, (dispH/2)-(textH/2));
        M5Cardputer.Display.drawString(file_list_cstr[i], (dispW/2)-(textW/2), (dispH/2)-(textH/2));
      } else if(i == select_index-1) {
        set_font_size(128);
        int textW = M5Cardputer.Display.textWidth(file_list_cstr[i]);
        int textH = M5Cardputer.Display.fontHeight();

        M5Cardputer.Display.drawString(file_list_cstr[i], (dispW/2)-(textW/2), (dispH/2)-(textH/2)-textH*2);
      } else if(i == select_index+1) {
        set_font_size(128);
        int textW = M5Cardputer.Display.textWidth(file_list_cstr[i]);
        int textH = M5Cardputer.Display.fontHeight();

        M5Cardputer.Display.drawString(file_list_cstr[i], (dispW/2)-(textW/2), (dispH/2)-(textH/2)+textH*2);
      }
    }
  }

  // Return '/' + selected file path
  char* selected_path = (char*)malloc(sizeof(char)*MAX_FILES+sizeof(char));
  sprintf(selected_path, "/%s", file_list_cstr[select_index]);
  return selected_path;
}

void setup() {
  // put your setup code here, to run once:
  update_palette();
  // Init M5Stack and M5Cardputer libs.
  auto cfg = M5.config();
  // Use keyboard.
  M5Cardputer.begin(cfg, true);

#if ENABLE_SOUND
  M5Cardputer.Speaker.begin();
#endif

  // Set display rotation to horizontal.
  M5Cardputer.Display.setRotation(1);
  set_font_size(64);

  // Initialize SD card.
  // Some of this code is taken from
  // https://github.com/shikarunochi/CardputerSimpleLaucher
  debugPrint("Waiting for SD Card to Init...");
  SPI2.begin(
      M5.getPin(m5::pin_name_t::sd_spi_sclk),
      M5.getPin(m5::pin_name_t::sd_spi_miso),
      M5.getPin(m5::pin_name_t::sd_spi_mosi),
      M5.getPin(m5::pin_name_t::sd_spi_ss));
  while (false == SD.begin(M5.getPin(m5::pin_name_t::sd_spi_ss), SPI2)) {
    delay(1);
  }

  // Initialize GameBoy emulation context.
  static struct gb_s gb;
  static struct priv_t priv;
  enum gb_init_error_e ret;
  debugPrint("postInit");

  
  debugPrint("Before filepick");
  char* selected_file = file_picker();
  debugPrint(selected_file);
  debugPrint("After filepick");

  // Check for errors in reading the ROM file.
  if((priv.rom = read_rom_to_ram(selected_file)) == NULL) {
    // error reporting
    debugPrint("Error at read_rom_to_ram!!");
  }

  ret = gb_init(&gb, &gb_rom_read, &gb_rom_read_16bit, &gb_rom_read_32bit, &gb_cart_ram_read, &gb_cart_ram_write, &gb_error, &priv);

  if(ret != GB_INIT_NO_ERROR) {
    // error reporting
    debugPrint("GB_INIT error!!!");
  }

  priv.cart_ram = (uint8_t*)malloc(gb_get_save_size(&gb));

#if ENABLE_LCD
  gb_init_lcd(&gb, &lcd_draw_line);
  // Disable interlacing (this is default behaviour but ¯\_(ツ)_/¯)
  gb.direct.interlace = 0;
#endif

  debugPrint("Before loop");

  // Clear the display of any printed text before starting emulation.
  M5Cardputer.Display.clearDisplay();
  
  // Target game speed.
  const double target_speed_us = 1000000.0 / VERTICAL_SYNC;
  while(1) {
    // Variables needed to get steady frametimes.
    int_fast16_t delay;
    unsigned long start, end;
    struct timeval timecheck;

    // Get current timer value
    gettimeofday(&timecheck, NULL);
    start = (long)timecheck.tv_sec * 1000000 +
      (long)timecheck.tv_usec;

    // Reset Joypad
    // This works because button status
    // is stored as a single 8-bit value,
    // with 1 being the non-pressed state.
    // This sets all bits to 1
    gb.direct.joypad = 0xff;

    // Read Keyboard matrix
    M5Cardputer.update();
    if(M5Cardputer.Keyboard.isPressed()) {
      Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
      // The Cardputer can detect up to 3 key updates at one time
      for(auto i : status.word) {
        // Controls:
        //      e
        //     |=|                     [A]
        // a |=====| d             [B]  l
        //     |=|       //  //     k
        //      s         2  1
        //
        // Might implement a config file to set these.
        switch(i) {
          case 'e':
            gb.direct.joypad_bits.up = 0;
            break;
          case 'a':
            gb.direct.joypad_bits.left = 0;
            break;
          case 's':
            gb.direct.joypad_bits.down = 0;
            break;
          case 'd':
            gb.direct.joypad_bits.right = 0;
            break;
          case 'k':
            gb.direct.joypad_bits.b = 0;
            break;
          case 'l':
            gb.direct.joypad_bits.a = 0;
            break;
          case '1':
            gb.direct.joypad_bits.start = 0;
            break;
          case '2':
            gb.direct.joypad_bits.select = 0;
            break;
          default:
            break;
        }
      }
    }

    /* Execute CPU cycles until the screen has to be redrawn. */
    gb_run_frame_dualfetch(&gb); // Walnut-CGB's default execution method (16-bit dual-fetch opcode chained dispatch)
    //gb_run_frame(&gb); can be used if the original Peanut-GB core is desired for comparison or compatibility (with CGB branch merged) 

    // Draw the current frame to the screen.
    fit_frame(priv.fb);

    // Get the time after running the CPU and rendering a frame.
    gettimeofday(&timecheck, NULL);
    end = (long)timecheck.tv_sec * 1000000 +
            (long)timecheck.tv_usec;

    // Subtract time taken to render a frame to the target speed.
    delay = target_speed_us - (end - start);

    /* If it took more than the maximum allowed time to draw frame,
    * do not delay.
    * This is a naive frame pacing loop for example purposes
    * a more robust solution can significantly improve performance.
    * Suggested improvements: timing debt, auto-frame skip, or advanced pacing logic.
    */
    if(delay < 0)
      continue;

    usleep(delay);
  }
}

void loop() {

}
