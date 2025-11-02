For submittting bug reports or for other inquries, I have been using gameboyenhancedfirmware@gmail.com. I am also active in the Cardputer reddit and discord.

*Note: There has been some confusion regarding my releases on Github and source code. I have not uploaded the main projects source code as plans have changed along the way. Originally I was going to polish the implementation and release it to be finished by the community, but just as I begun the project was archived by the original author. This resulted in a very low-effort release of the same firmware without credit to the original author for their implementation landing on m5burner at approximately the same time as my version. As this is my return to coding after a 14 year hiatus, I've decided that rather than have my additions copied with no meaningful contributions or acknowledgement, that I'd just finish the entire emulator myself and release full source code after I have finished my work and had my fun with this project. This seems preferable to having my passion for the project deflated. I've been sharing detailed change logs, and under /rCardputer on reddit and the Cardputer Discord, I am always willing to go into further detail of the specifics of the solutions I have employed in the interest of helping the budding developer. In addition, the cgb.h & sgb.h files shows how I am handling palettes. The CGB palettes aren't great, but they are the accurate. The SGB.h file contains the official 32 included palettes, in addition to palettes that I have gathered from a variety of sources(by hand using screenshots & community created palettes) along with the title mapping information that similary was gathered from a variety of sources. The format is ob0 0 1 2 3 ob1 0 1 2 3 bg 0 1 2 3  for the 12 colour palettes in RGB888. These are converted to RGB565 when a palettte is selected. sgb.h is in 12 color format which may seem confusing, this is for consitency with other colour modes within the emulator and could be reduced to 4 color paletttes. Since there are so few of them, and they are stored in the firmwares ROM, it hasn't been a concern. DMG.h contains a copy of the open source boot rom https://github.com/Hacktix/Bootix for use with Peanut_GB

Finally, in order to share the lessons I've learned during the course of this project, I've created an Unofficial Cardputer Developer Reference Manual. The work I've done on this project directly influenced the guide I've written under the 'Game Development on the CardPuter' heading in the navigation panel. These are topics that have direct application for game projects on the cardputer, but also may be helpful for other projects. 'Arduino Development Specifics' contains some detail on customizing the runtime environment of your sketch by adjusting the stacksize and priority of the main thread and changing the behaviour of the watchdog timers.

http://cardputer.free.nf/

This is a resource I have created to help other developers learn and develop quality software for the CardPuter. If your frustrated at the lack of source code of the main project, oh well, I'm sharing with the community in my own way.

![image](https://github.com/Mr-PauI/gb_cardputer_mod/assets/169319235/5a9bf85b-0a44-4f37-931b-c06ac70d62a3)

Updates are more frequent on m5burner than github, but I'm trying to drop a binary here now and then.

Gameboy/Gameboy Color Emulator; complete with audio, configurable controls, display and performance options, savegames, save states, filesystem navigation and no .gb/.gbc rom file size limits imposed by memory. Various other enhancements. Accurate palettes. Partial Super Gameboy Enhancement support, including borders. Extended 12 colour mode, along with all official/original GBC palettes. 44 Analogue Pocket 12 colour community palettes included with automatic mapping of titles to AP palettes partially implemented.

All *.gb and *.gbc files are listed by the file explorer in the current directory. 

Read the instructions at the bottom for controls or refer to the graphic guide below. Forked from original gb_cardputer implementation. No bootloader has been merged with this firmware (at this time), so if you are having issues try using m5launcher to install.

![image](https://github.com/Mr-PauI/Gameboy-Enhanced-Firmware-m5stack-cardputer-/assets/169319235/31c67758-ca57-4217-a3ef-25c20e5cc8f2)


![image](https://github.com/Mr-PauI/Gameboy-Enhanced-Firmware-m5stack-cardputer-/assets/169319235/5c78d59e-ae08-46ed-9522-f11b659504ed)

Press \ to cycle through available display aspect ratios (note super gameboy border hidden/off-screen when in 16:9 or above)

Press Fn+\ to cycle through available vertical scaling and alignment options

List of SGB and Analogue Pocket enhanced titles:
* Adventures of Lolo
* Alleyway
* Arcade Classics No 1: Missle Command & Asteroids
* Arcade Classics No 2: Centipede and Millipede
* Balloon kid
* Baseball
* Battlezone/Super Breakout
* Donkey Kong
* Donkey Kong Land
* Dr. Mario
* Dragon Warrior I&II
* F1 race
* Final Fantasy Legend
* Final Fantasy Legend II
* Game & Watch Gallery
* Game Boy Gallery
* Game Boy Wars
* Golf
* James Bond 007
* Kaeru no Tame
* Kirbys Dream Land
* Kirbys pinball land
* Zelda Links Awakening
* Mario and Yoshi
* Mario Land
* Mario Land 2
* Mega Man I
* Mega Man II
* Mega Man III
* Metroid II
* Mystic Quest
* Pocket Bomberman
* Pokemon Red
* Pokemon Blue
* Pokemon Yellow
* Pokemon Gold(limited)
* Pokemon Silver(limited)
* Qix
* Solar Striker
* Space Invaders
* Star Wars: Super Return of the Jedi
* Tennis
* Tetris
* The Rescue of Princess Blobette Starring A Boy and His Blob
* Wario Land
* X
* Yakyuman
* Yoshi

1.11.2024:v0.79.30
* interlacing now provides a further performance boost
* full screen(16:9) is now the default scaling mode, sgb borders on by default if available for a game but hidden unless scaling mode changed
* changed default palette for new users from black&white to the gameboy pocket palette
* game specific borders on by default, must change aspect ratio to see them

31.10.2025:v0.79.27
* fixed rendering bug with centered vertical alignment
* changed default aspect ratio to 12:10 for improved visibility

30.10.2025:v0.79.26
* Initial implementation of vertical scaling and alignment control, rotate through the various modes using Fn+\

29.10.2025:v0.79.25
* Initial implementation of full screen(and other) horizontal scaling modes, rotate through various aspect ratios(including over scanned resolutions) using the \ key

28.10.2025:v0.79.22
* fixed crash on palette changed introduced with v0.79.12
* quick save/quick load feature now works correctly with cgb games

27:.10.2025:v0.79.20
* fixed save games not working or being corrupted when switching between games.
* significantly increased the speed of Grand Theft Auto

26.10.2025:v0.79.14
* interlacing no longer causes screen distortion

25.10.2025:v0.79.12
* further performance improvements (all games,+2-4fps)

24.10.2025:v0.79.08
* overall performance improvement (all games)

23.10.2025:v0.92.06
* performance improvements 

22.10.2025:v0.92.04
* extended cpu fast mode compatibility to include cgb
* fixed interlacing not synchronized with cgb games issue

20.10.2025:v0.79.02
* performance improvement for cgb games

18.10.2025:v0.79.00
* added gameboy colour support to main project
* added save message on menu entry if saving
* increased audio volume effective range
* added message when trying to change cgb to dmg/sgb colours
* automatic cgb enhance used for pokemon yellow
* dmg/sgb compatibility enabled
* fixed crash on error

17.10.2025:v0.78.40
* improved audio quality

15.10.2025:v0.78.38
* some small improvement to entry of settings menu speed
* added fixes for donkey kong land 2/3

14.10.2025:v0.78.36
* added fixes for donkey kong land 2/3
* fixed sgb border behaviour when exiting an sgb enhanced game
* fixed palette not resetting after game exit
* removed savegame indicator when exiting a game
* autosave no longer triggered on setting menu return
* increased swap speed
* removed audio clipping

13.10.2025:v0.78.35
* increased performance in all interlaced and non interlaced modes(will be an option in future but its a world of difference, no drops in pokemon menus, etc)
* removed old settings from menu
* more efficient use of 2nd core(bass boost temporarily disabled)
* audio sampling rate increased

12.10.2025:v0.78.31
* fixed bug with cpu mode setting(increased responsiveness)
* increased interlacing/interleaving speed
* added sparse interlacing mode
  
11.10.2025:v0.78.21
* maximized file list generation speed.
* improved exit game function, immediate now.

10.10.2025:v0.78.20
* added basic file/directory sorting functions(oversight from long ago finally corrected)
* fixed file browsing bug after exiting a game
* improved exit game function, smoother exit from menu

09.10.2025:v0.78.10
* Removed last usage of malloc in preparation for modularity
* Updated cpu core, Final Fantasy Legends III among others now work as expected. 
* Updated to latest m5stack libraries, cardputer adv officially supported. End of adv testing phase.

24.07.2024:v0.78

* Reduced overall volume; it still draws enough power to cause the backlight to flicker with the boost on full effect and volume maxed but it was flickering badly with the new amplification code.
* Added revision number to boot sequence
* Added configurable BASS BOOST to settings menu (0 -> +42db gain)
* Removed dynamically allocated memory component for cartridge ram, 32k is largest any compatible game requires anyways so this is now a fixed buffer preventing memory fragmentation which allows the new....
* Added ability to quit the current game and return to the file selection menu; there may be a slight
sound as the next game starts. This is just left over audio buffer info from the previous emulation.
* Spent time this month porting the emulator to T-Display S3 Touch, gamepad support working in that version, this was research to hopefully discover the incompatibility the cardputer seems to be having, power and library conflict are leading contenteders. Code is the same.

07.07.2024:

* Added bass boost function to audio engine
* Added SGB support for pocket bomberman
* Improved audio quality; less clipping at high volume levels.
* Fix bug with startup audio being forced to Mute (part of BLE debugging code I forgot to remove) 
  
06.07.2024:v0.77
* Added adventures of lolo, james bond 007, Megaman 3, The Rescue of Princess Blobette Starring A Boy and His Blob, Game & Watch Gallery, Arcade Classics No2 Centipede and Millipede SGB & AP support
* Increased speed of entering large directories in some circumstances
* Increased file list limit to 1024 entries maximum, provided there is sufficient memory. Most files probably use less than half the max file name size, so 1024x128 directory entry listings are easily accommodated. File names may still be a full 256 bytes in length.

29.06.2024:v0.76
* Compatibiliyy mode settings now take effect immediately if changed mid-game; will display message if mode is locked on due to override requirement (ie: battlezone which requires it on for any functionality)
* Button mapped to controller B and del/back button will return to a parent directory (if not at root)
* Added CRT Television border available for all games
* Added Star Wars: Super Return of the Jedi SGB & Ap support w/Border
* Increased file/directory listing limit to 512 entries per directory
* No longer crashes when there are too many files, will list first 512 files and display warning.
* Added warning message when too many files/directories exist to list.

25.06.2024:v0.75
* Firmware now contains bootloader so it can be loaded without a launcher/direct from m5burner
* Added SGP & AP support for Mega Man I/II and Mystic Quest
* Added USB serial debug output to the entire function chain leading up to the main emulator loop. This should hopefully aid in helping debug games, and provide more comprehensive information for trouble shooting.
* Added CPU mode to settings menu (CPU MODE: FAST/COMPATIBLE), defaults to fast. Enabled automatically for titles known to need this: Battlezone so far, will add any reported titles. For now it takes effect only upon restart of the emulator (will take effect immediately in future update). This disables all speed hacks,and forces rendering of every line of every frame.
* BLE gamepad support only stable in file picker, causes restart during emulation so still disabled.
* Fixed Minor bug with auto SGB mode flag setup
  
24.06.2024:v0.74
* Added compatibility mode that skips all speed hacks (skipped frames and unrenderd lines)
* Added automatic compatibility mode override for titles that are identified to require this, as a result Battlezone is now compatible.
* Added SGB support for Battlezone
* Resolved BLE server and SD card conflict; this took about a week of research and debugging.
  
20.05.2024:v0.73
* BLE startup behaviour implemented, BLE sync from main menu implemented
* Gamepad buttons a,b,x,y and d-pad directional control integrated
  
19.05.2024:v0.72
* Implemented Default Palette selection, SGB manual/auto selection and SGB border options
* Added Default Palette, SGB palette manual/auto, SGB border manual/auto/gboy to options menu
* Added BLE connect manual/auto, BLE button swap on/off (swaps a+b button mapping), BLE deadzone small/med/large for left-stick deadzone configuration
* All new options are now saved in a new iteration of the config file, as usual, any previous config file will automatically migrate to the new version
* BLE connection settings will be saved/loaded regardless of BLE compilation support for consistent configuration file layout
  
18.05.2024:v0.71
* Added SGB profiles for Alleyway, Baseball, Dr. Mario, Game Boy Wars, Kaeru no Tame, Kirbys Dream Land, Mario and Yoshi,Qix,Tennis,X,Yakyuman,Yoshi
(note: not all of these are compatible, but it almost completes the built in SGB maps that were included in the SGB hardware; future core updates may improve compatibilty)
* Added AP profiles for Alleyway, Baseball, Dr. Mario
* Added bluetooth library to the emulator, this has a fairly substantial memory cost and will be disabled until ready for deployment; made inclusion of the BLE code optional when compiling.
* Added initial hooks for gamepad support.
  
12.06.2024:v0.70
* Added per-game savestates (1 state/game in .qsv file), backspace to save, minus/underscore button to load
* All borders are now drawn as RGB565 sprites; almost no impact for FPS display now
* Borders now have a cooldown before changing, allowing the emulator to run smoothly when cycling options
* Increased scroll speed in file selection menu
* Changed number of file/directories displayed at once from 3 to 5
* Added Dragon Warrior I & II SGB & AP support w/ Border
* Added SGB border support for Pokemon Silver/Gold, SGB/AP colours temporarily set to existing pallets until better is found

11.06.2024(pm):v0.68
* Control A and Control Start will now also navigate into directories
* Message Box constrast increased (darker background)
* First item in subdirectories is selected (if anything is present) instead of return to parent directory option
* Added Generic Fantasy Border (from a jp only SGB game) available to all games
* Final Fantasy Legend II SGB * AP support (same as FFL1)
* Added Metroid II SGB & AP support
* Added Golf SGB & AP support
* Added Solar Striker SGB & AP support
* Added Missle Command/Asteroids SGB and AP support w/ border
* Added Space Invaders SGB & SP support w/ Border

11.06.2024(am):v0.67
* Added ability to return to parent directory if it exists
* Added ability to launch ROMs from other directories
* Hides System Volume Information if in root directory; this is probably not what the user wants.
  
10.06.2024:v0.66.5
* Added memsub_malloc() and memsub_freeall() functions to allow paging memory to be used for other purposes:namely, file browsing.
* Changed file_picker() function to use memsub_malloc() / memsub_freeall(), limits are as follows:
256 files/folders per folder, maximum path+file length+3 letter extension is 256 bytes
There is no limit to the depth of the folders, except that the path name+file cannot exceed 256 bytes
No issue with long file names, the memsub_malloc() has 65k roughly allocated per directory listing, just be mindful of the overall path+filename length
* Added *.gbc in addition to *.gb files to selection menu
* Added directories to selection menu, can now explore subdirectories
* Adjusted default SGB palettes for Pokemon Blue/Red; previous fix required adjustment of these two titles.
  
09.06.2024:v0.66
* Added Max and Min audio quality settings, 6 and 72 sample intervals respectively
* Added 12 colour Analogue Pocket palette for Kirbys Pinball Land
* Added 12 colour Analogue Pocket palette for Donkey Kong Land
* Added SGB border and palette for Donkey Kong Land
* Added Cardputer Arcade cabinet border, available for all games
  
08.06.2024:
* Silver has been tested, must rename to .gb from .gbc
  
06.06.2024:v0.65
* increased paged memory limit to 2megs to accommodate Pokemon Gold/Silver
* Gold has been tested, must rename to .gb from .gbc
* increased specificity of error messages regarding paging
  
04.05.2024:v0.64
* Fixed bug with setting initial super gameboy palettes
* Added Pokemon Yellow SGB Border and AP Community Palette
* Added Supermario land 2 SGB border
* Added Tetris SGB border
* Added Final Fantasy Legends SGB Palette and Border, added 12 colour Analogue Pocket palette
* Pokemon Blue border is now scaled using simple pixel scaling method (same as all other borders) to preserve pixelized look
* Gameboy border left border has been adjusted
  
03.05.2024:v0.63
* Added audio quality setting (high,med,low, update intervals of 12,24 and 36 audio samples)
* Added startup audio volume setting (default is mute)
* Added interlacing setting
* Added new settings to configuration files; old configuration files are always supported, and will be upgraded to the newest version when the settings are saved for the first time.
* Squished another bug; timed events weren't taking into account time for border drawing/clearing
* Fixed crash from settings menu when accessed from main menu, settings are saved on game launch or if changed mid game. Crash was related to saving within the file picker.
  
02.05.2024:v0.622
* Added mute and volume half/max contols to Fn+Left/Right keys
  
01.05.2024:v0.621
* Experimented with an inline version of the ganeboy CPU its various functions but they yeilded no performance gain
* CPU step loop moved inside of peanut_gb to reduce function calls, essentially the same as inline but with no memory penalty
  
30.05.2024.v0.62
* Audio rendering has been made faster updating every 24 by default samples, was twelve; 24 will be the default (a.k.a. medium quality) sligt reduction in quality when frame rate dips low can be noticed at 36; may make option for audio quality. Updating every 36 samples seems
to be the minimum(clicking becomes more likely beyond this update interval), so 12,24,36 seem to be appropriate settings; every 12 sameples was used in 6.0 may added higher and lower quality settings in future
* Adjusted TTL logic to run seperately from the page retrieval so that retrieving data is as fast as possible when in memory.
  
29.05.2024:v0.61
* Memory solution devised was based on two ideas: Assume stastically random access patterns, and place no limits on addressable space.
The reality is that the gameboy games never exceed 8megabits (1megabyte); Taking advantage of the limitations of the gameboy, I have removed the generalized nature of the	previous solution in favor of one that provides a deterministic approach to page seeking, with the tradeoff that no games larger than a real game boy game can be loaded any longer. This should not pose an issue for any (valid) title. This has increased frame rates slightly all around and made them much steadier; page location has no bearing on access times. Page seek times are fixed, and a minor amount of page maintenance is done at these intervals, distributed across successive calls still. This would be deterministic page seeking mixed with the previous progressive pruning.
* Added Error messages with regards to writing save game files, attempts to do so when there is no cart ram
* Added Error messages specific to the case of being unable to open a file vs. file size 0 (was ambiguous before)
* Removed pause associated with automatic savegames; bars operate on a timer now and will disappear 1second after pending write is completed

28.05.2024:v0.601
* No longer will create save files or allow save files to be created for titles without cart ram(0b files no more)


27.05.2024:v0.6
* Audio is fully implemented. Inaccuratley, built for speed. Hardly any impact of rendering.
Note: While the tones will not change pitch as a title slows down, music will speed up and slow down along with frame rates.
* Implemented volume up/down controls attached to Fn+up/down arrows. If these are mapped to controls, at this time, it will respond to both.
Further Note: I put no limits on control configuration. Make everything a single button if you want.
* Added cessation of audio rendering when volume is set to 0.

26.05.2024:v0.57
* alterered APU to allow variable amounts of time between calls
* Tuned APU performance to get first recognizable gameplay sounds out of the emulator.

25.05.2024:v0.56
* Added 12color AP palette mapping for Links Awakening
* Added SGB border for Pokemon Red
* Added SGB border for Links Awakening
* Added notification for titles with Analogue Pocket profile, but no SGB profile
* Work on audio playback engine has begun, core 0 continusoly feeds audio from a buffer to the speaker; a test 1000hz sine wave shows little to no stuttering while emulator runs, needs tuning. Disabled until apu work complete.

23.05.2024:v0.53
* Required memory for pokemon +5k just incase has been made available
* Implemented 12-color Analogue Pocket palettes for Pokemon Red/Blue
* Implemented preset SGB and GBC palettes for Pokemon Red/Blue
* Implemented SGB borders for Pokemon Blue
* disabled audio for this release; exported to wav file to examine and while its the same code
it is not generating the same output as the PC test version. Will take further research.

22.05.2025:v0.52
* Implemented audio with peanut_gb hooks on PC (version for debugging); verified APU core and waveforms
* Integrated Audio with peanut_gb hooks on ESP32; audio waveforms are now being generated, needs balancing with CPU and speaker playback still

21.05.2024:v0.51
* implemented main emulator function as a task on core 1 and a second persistent task on core 0 via RTOS; this is in preparation to acommodate the APU engine
core 0 thread must call vTaskDelay as there are vital threads on that core, which if not attented to, will cause a restart. I dont have full use of this core.
discovered pokemon starts working if I have more free memory so I suspect I run out of stack space. 180k paging it works 200 it doesn't will calculate requied amount

20.05.2024:v0.5
* added bottom menu bar with instructions to the main rom selection menu, pres ESC or ' key (same thing) to enter the settings menu either from the main menu or while playing a game
* added options menu, can be entered from the menu or from within a game; displays the 8 main controls with their current setting, saves settings when closed
* added restore defaults for config menu
settings are saved to gbconfig.dat; delete this if you are having any issues

19.05.2024:v0.492
* Reimplemented memory subsystem to use progressive partial page seeking/pruning; the original memory management code was the first I had typed in 14 years; after some thought I devised something much more suitable for a real time environment. This resulted in the average page seek time being much lower, and distributes maintenance of the paging system across successive calls. The results are the largest improvement to speed to date. Over double the frame rates from before; less stutter, smoother page transitions in memory. Donkey Kong Land averages around 45fps now; Super Mario land 2 gets an average 53fps on the overworld.

18.05.2024:v0.491
* Reduced rendering workload 6.25% by modifying peanut_gb to inherently skip lines that aren't visible due to scaling.
This prevents the engine from having to process the layer and sprite data for these lines all together. Why 6.25%? because 9 lines are skipped which is 6.25% of the lines that were rendered previously.

17.05.2024:v0.49
* Refactored graphics code; ~120,000 less operations a frame
* Scrolling behaviour and screen content differences no longer effect rendering performance Titles such as final fantasy or even super mario land 2 show a huge improvement in overworld movement speed.
17.05.2024: v0.483 Pushing this now as a BUG FIX RELEASE
* Fixed bug in main menu causing selection to only move upward; you can now navigate properly again.
* added FPS display while Fn button is held, causes slow down which is subtracted from the FPS display. This is so I can evaluate performance improvements more than anything else, but it doesn't hurt to have so I'm leaving it in.
* new borders are on hold while I make decisions about the internal format (leaning towards argb1555,presently 565)
16.05.2024: v0.482
* Added Analogue Pocket 12color palette category with 44 palettes
* Automatic 12color(AP) palettes mapped as per Analogue Pocket suggested mappings for:
Mario 1/2/Wario Land/Balloon Kid/F1 Race/Tetris
* added Cottage Daytime SGB border
* Fixed 12colours not being assigned until palette select bug
* regularly mapped controller up/down/a now functions in addition to arrow/enter keys in main menu to allow a single hand posture for the entire interface if desired.
15.05.2024: v0.481
* proper startup sequence, may help compatibility with some games.
* Message boxes now will display any emulation errors reported by peanut_gb
* Attempts to access ROM address outside of the available cartridge ROM will display an appropriate message
14.05.2024 v0.48
* Added 12 colour mode, with the 12 palettes the game boy colour could apply to old game boy games as the first of this mode, but more to come in this area.
* Super gameboy support added for screen borders with gameboy skin set as default, followed by 1 (for now) of the official borders from the hardware itself. Activate by holding Fn and pressing '[' and cycle borders by holding Fn and pressing ']'. Because these are the same keys used for other visual modifications, I hope using the combination of Fn and these keys is intuitive.
* Super gameboy palette support added for balloon kid, the legend of zelda links awakening and kirbys pinball land
* Various other small tweaks you probably wont notice.
12.06.2025 V0.47
* Added first iteration of Super Gameboy Mode with all 32 official palettes that were included with the original hardware. Nintendo included a table on the device to map certain games to certain palettes, and that functionality is partially implemented. Mario 1/2/Wario Land, F1 Racer and Tetris all autodetect and assign their colour scheme. This mode can be toggled on and off at any time during play with the '[' button, and the current cycle palette button will cycle through the 32 included palettes. Games with defined profiles with start with that palette selected automatically when Super Gameboy Mode is engaged for the first time each session.
*Lots of user interface changes; message boxes will appear to describe your palette selections, among other things - no console style debug remains in normal operation.
*Another 4k of memory allocated to ROM storage, may smooth out some edge cases of stuttering.
*Improved readability in the main menu; and made highlighted selection more apparent.
*Added smooth transitions to splash screen.
*Disabled unused (for the moment) configuration file

11,05.2024:: 0.44 
* squished a bug, added palette control, press ']' to cycle between presets.
 b/w, gameboy(original), gameboy pocket and gameboy light (in that order).
* Huge performance improvements for larger ROMs with over 110k more ram available to the memory sub-system. Palette values are from https://en.wikipedia.org/wiki/List_of_video_game_console_palettes for accuracy.

10.05.2024:: 0.41 
*added savegame support. if a game uses it's onboard ram constantly though, use the manual backup button (=). The save feature will only automatically engage after the cartridge ram has been left untouched for a second.

Yellow bars on either side of the display momentarily indicate that the cartridge ram has been backed up. The savegame format will not be changing, its a simple binary dump.

Controls(in game):
left/right use 'a' and 'd'
up/down use 'e' and 's'
A/B use 'l' and 'k'
start/select use '1' and '0'
cycle palette in current mode use the ] square bracket
cycle between classic gameboy,super gameboy & 12 color modes [
quicksave/savestate press del/backspace key
quickload/loadstate press minus/underscore key
force cart ram backup press '=' (if uncertain of gamesave use this)
turn on super gameboy border at any time by pressing Fn+'['
cycle through border options by pressing Fn+']'
Display current FPS by holding Fn; it will appear in 1 second and update every second there after, can cause some slowdown but that is accounted for in the FPS count.
Press `/esc in the main menu or during gameplay for settings menu
Mute audio using Fn+left arrow
Raise audio to 50% then 100% using Fn+right arrow
Change volume using Fn+up/down arrow keys for granular control
Press \ to cycle through available display aspect ratios (note super gameboy border hidden/off-screen when in 16:9 or above)
Press Fn+\ to cycle through available vertical scaling and alignment options

Have fun!

## Original Project Readme ##

# GB Cardputer
Run GameBoy games on your M5Stack Cardputer!*

Uses [Peanut-GB](https://github.com/deltabeard/Peanut-GB), a super cool project that's basically an entire GameBoy emulator in a single C header!

# Warning
This "port" is not polished at all, the code needs major commenting, refactoring and other general cleanup.

Audio also doesn't work, since I just didn't feel like making it work for now lol, though I might addreess that later on

I just cobbled this together between yesterday (17/1/2024) and today just to mess around with the Cardputer a bit.
