// The first 32 palettes are the official palettes included on the SGB hardware
// The mappings in () are the built in cartridge maps for games released before
// the SGB hardware was developed. Other mappings and palettes have been obtained
// from a variety of sources. By hand from screenshots, from community generated
// palettes and from game packaging images. Accuracy varies by title for other
// mappings. A real super game boy executes a different path in code on the
// cartridge, so these simulate the primary effect of the SGB but dont provide
// the SGB ram,audio and dynamic palette and border features.
#define default32sgb_pallets_count 32
const uint32_t default32sgb_palettes[] = {
// 0 1-A (Balloon Kid)
0xFFEFCE, 0xDE944A, 0xAD2921, 0x311852,
0xFFEFCE, 0xDE944A, 0xAD2921, 0x311852,
0xFFEFCE, 0xDE944A, 0xAD2921, 0x311852,
// 1 1-B (Wario Land) Kirbys Pinball Land
0xDEDEC6, 0xCEB573, 0xB55210, 0x000000,
0xDEDEC6, 0xCEB573, 0xB55210, 0x000000,
0xDEDEC6, 0xCEB573, 0xB55210, 0x000000,
// 2 1-C (Kirby's Pinball Land)
0xFFC6FF, 0xEF9C52, 0x9C3963, 0x39399C,
0xFFC6FF, 0xEF9C52, 0x9C3963, 0x39399C,
0xFFC6FF, 0xEF9C52, 0x9C3963, 0x39399C,
// 3 1-D (Yoshi's Cookie) (Yoshi no Cooki)
0xFFFFAD, 0xC6844A, 0xFF0000, 0x521800,
0xFFFFAD, 0xC6844A, 0xFF0000, 0x521800,
0xFFFFAD, 0xC6844A, 0xFF0000, 0x521800,
// 4 1-E (Zelda Link's Awakening)
0xFFDEB5, 0x7BC67B, 0x6B8C42, 0x5A3921,
0xFFDEB5, 0x7BC67B, 0x6B8C42, 0x5A3921,
0xFFDEB5, 0x7BC67B, 0x6B8C42, 0x5A3921,
// 5 1-F (Super Mario Land)
0xDEEFFF, 0xE78C52, 0xAD0000, 0x004210,
0xDEEFFF, 0xE78C52, 0xAD0000, 0x004210,
0xDEEFFF, 0xE78C52, 0xAD0000, 0x004210,
// 6 1-G (Solar Striker) Space Invaders
0x000052, 0x00A5EF, 0x7B7B00, 0xFFFF5A,
0x000052, 0x00A5EF, 0x7B7B00, 0xFFFF5A,
0x000052, 0x00A5EF, 0x7B7B00, 0xFFFF5A,
// 7 1-H Pokemon Red
0xFFEFE7, 0xFFBD8C, 0x844200, 0x311800,
0xFFEFE7, 0xFFBD8C, 0x844200, 0x311800,
0xFFEFE7, 0xFFBD8C, 0x844200, 0x311800,
// 8 2-A (Kaeru no Tamei)
0xF7CEA5, 0xC68C4A, 0x297B00, 0x000000,
0xF7CEA5, 0xC68C4A, 0x297B00, 0x000000,
0xF7CEA5, 0xC68C4A, 0x297B00, 0x000000,
// 9 2-B  Dragon Warrior I&II
0xFFFFFF, 0xFFEF52, 0xFF3100, 0x52005A,
0xFFFFFF, 0xFFEF52, 0xFF3100, 0x52005A,
0xFFFFFF, 0xFFEF52, 0xFF3100, 0x52005A,
// 10 2-C (Kirby's Dreal Land) (Hoshino Kirbi Bi)
0xFFC6FF, 0xEF8C8C, 0x7B31EF, 0x29299C,
0xFFC6FF, 0xEF8C8C, 0x7B31EF, 0x29299C,
0xFFC6FF, 0xEF8C8C, 0x7B31EF, 0x29299C,
// 11 2-D (Yoshi, Mario & Yoshi) (Yoshi no Tamago)
0xFFFFA5, 0x00FF00, 0xFF3100, 0x000052,
0xFFFFA5, 0x00FF00, 0xFF3100, 0x000052,
0xFFFFA5, 0x00FF00, 0xFF3100, 0x000052,
// 12 2-E Pokemon Blue / Power Quest
0xFFCE84, 0x94B5E7, 0x291063, 0x100810,
0xFFCE84, 0x94B5E7, 0x291063, 0x100810,
0xFFCE84, 0x94B5E7, 0x291063, 0x100810,
// 13 2-F (Kid Icarus of Myths and Monsters)
0xD6FFFF, 0xFF9452, 0xA50000, 0x180000,
0xD6FFFF, 0xFF9452, 0xA50000, 0x180000,
0xD6FFFF, 0xFF9452, 0xA50000, 0x180000,
// 14 2-G (Baseball)
0x6BBD39, 0xE75242, 0xE7BD84, 0x001800,
0x6BBD39, 0xE75242, 0xE7BD84, 0x001800,
0x6BBD39, 0xE75242, 0xE7BD84, 0x001800,
// 15 2-H
0xFFFFFF, 0xBDBDBD, 0x737373, 0x000000,
0xFFFFFF, 0xBDBDBD, 0x737373, 0x000000,
0xFFFFFF, 0xBDBDBD, 0x737373, 0x000000,
// 16 3-A (Tetris)
0xFFD69C, 0x73C6C6, 0xFF6329, 0x314A63,
0xFFD69C, 0x73C6C6, 0xFF6329, 0x314A63,
0xFFD69C, 0x73C6C6, 0xFF6329, 0x314A63,
// 17 3-B (Dr. Mario)
0xDEDEC6, 0xE78421, 0x005200, 0x001010,
0xDEDEC6, 0xE78421, 0x005200, 0x001010,
0xDEDEC6, 0xE78421, 0x005200, 0x001010,
// 18 3-C (Yakyuuman)
0xE7ADCE, 0xFFFF7B, 0x00BDFF, 0x21215A,
0xE7ADCE, 0xFFFF7B, 0x00BDFF, 0x21215A,
0xE7ADCE, 0xFFFF7B, 0x00BDFF, 0x21215A,
// 19 3-D (Super Mario Land 2)
0xF7FFBD, 0xE7AD7B, 0x08CE00, 0x000000,
0xF7FFBD, 0xE7AD7B, 0x08CE00, 0x000000,
0xF7FFBD, 0xE7AD7B, 0x08CE00, 0x000000,
// 20 3-E (Game Boy Wars)
0xFFFFC6, 0xE7B56B, 0xB57B21, 0x524A73,
0xFFFFC6, 0xE7B56B, 0xB57B21, 0x524A73,
0xFFFFC6, 0xE7B56B, 0xB57B21, 0x524A73,
// 21 3-F (Alleyway)
0x7B7BCE, 0xFF6BFF, 0xFFD600, 0x424242,
0x7B7BCE, 0xFF6BFF, 0xFFD600, 0x424242,
0x7B7BCE, 0xFF6BFF, 0xFFD600, 0x424242,
// 22 3-G (Tennis)
0x63DE52, 0xFFFFFF, 0xCE3139, 0x390000,
0x63DE52, 0xFFFFFF, 0xCE3139, 0x390000,
0x63DE52, 0xFFFFFF, 0xCE3139, 0x390000,
// 23 3-H (Golf)
0xE7FFA5, 0x7BCE39, 0x4A8C18, 0x081800,
0xE7FFA5, 0x7BCE39, 0x4A8C18, 0x081800,
0xE7FFA5, 0x7BCE39, 0x4A8C18, 0x081800,
// 24 4-A (Qix)
0xF7AD6B, 0x7BADFF, 0xD600D6, 0x00007B,
0xF7AD6B, 0x7BADFF, 0xD600D6, 0x00007B,
0xF7AD6B, 0x7BADFF, 0xD600D6, 0x00007B,
// 25 4-B
0xF7EFF7, 0xEFA563, 0x427B39, 0x180808,
0xF7EFF7, 0xEFA563, 0x427B39, 0x180808,
0xF7EFF7, 0xEFA563, 0x427B39, 0x180808,
// 26 4-C
0xFFE7E7, 0xDEA5D6, 0x9CA5E7, 0x080000,
0xFFE7E7, 0xDEA5D6, 0x9CA5E7, 0x080000,
0xFFE7E7, 0xDEA5D6, 0x9CA5E7, 0x080000,
// 27 4-D (X)
0xFFFFBD, 0x94CECE, 0x4A6B7B, 0x08214A,
0xFFFFBD, 0x94CECE, 0x4A6B7B, 0x08214A,
0xFFFFBD, 0x94CECE, 0x4A6B7B, 0x08214A,
// 28 4-E
0xFFDEAD, 0xE7AD7B, 0x7B5A8C, 0x002131,
0xFFDEAD, 0xE7AD7B, 0x7B5A8C, 0x002131,
0xFFDEAD, 0xE7AD7B, 0x7B5A8C, 0x002131,
// 29 4-F (F-1 Race)
0xBDD6D6, 0xDE84DE, 0x8400A5, 0x390000,
0xBDD6D6, 0xDE84DE, 0x8400A5, 0x390000,
0xBDD6D6, 0xDE84DE, 0x8400A5, 0x390000,
// 30 4-G (Metroid II Return of Samus)
0xB5E718, 0xBD215A, 0x291000, 0x008463,
0xB5E718, 0xBD215A, 0x291000, 0x008463,
0xB5E718, 0xBD215A, 0x291000, 0x008463,
// 31 4-H
0xFFFFCE, 0xBDC65A, 0x848C42, 0x425229,
0xFFFFCE, 0xBDC65A, 0x848C42, 0x425229,
0xFFFFCE, 0xBDC65A, 0x848C42, 0x425229,
// Below is above default32sgb_pallets_count and used for titles that aren't mapped to the default profiles
// decimal index value and game title in comments
// 32 Palette for final fantasy legends I/II
0xF8F8F8, 0xB0C0F8, 0x506088, 0x284858,
0xF8F8F8, 0xB0C0F8, 0x506088, 0x284858,
0xF8F8F8, 0xB0C0F8, 0x506088, 0x284858,
// 33 Palette for Pokemon Yellow
0xFFFFFF, 0xFFFF00, 0xFF0000, 0x000000,
0xFFFFFF, 0xFFFF00, 0xFF0000, 0x000000,
0xFFFFFF, 0xFFFF00, 0xFF0000, 0x000000,
// 34 Palette for Arcade Classic No. 1 - Asteroids & Missile Command / Arcade Classic No. 2 Centipede / James bond 007 / Mystic Quest / Game and watch gallery / Ken Griffey Jr. presents Major League Baseball
0xFFFFFF, 0xFF8484, 0x943A3A, 0x000000,
0xFFFFFF, 0xFF8484, 0x943A3A, 0x000000,
0xFFFFFF, 0xFF8484, 0x943A3A, 0x000000,
// 35 Palette for Battlezone Arcade Classic
0x00FF00, 0xFF0000, 0xA5A500, 0x000000,
0x00FF00, 0xFF0000, 0xA5A500, 0x000000,
0x00FF00, 0xFF0000, 0xA5A500, 0x000000,
// 36 Mega Man 1/2/3/Adventures of Lolo
0xFFFFFF, 0x63A5FF, 0x0000FF, 0x000000,
0xFFFFFF, 0x63A5FF, 0x0000FF, 0x000000,
0xFFFFFF, 0x63A5FF, 0x0000FF, 0x000000,
// 37 Star wars: Super return of the jedi
0xFFFFFF, 0x7BFF31, 0x008400, 0x000000,
0xFFFFFF, 0x7BFF31, 0x008400, 0x000000,
0xFFFFFF, 0x7BFF31, 0x008400, 0x000000,
// 38 Gameboy Gallery
0xFFFFFF, 0xFFAD63, 0x843100, 0x000000,
0xFFFFFF, 0xFFAD63, 0x843100, 0x000000,
0xFFFFFF, 0xFFAD63, 0x843100, 0x000000,
// 39 Pocket Bomberman
0xFFC542, 0xFFD600, 0x943A00, 0x4A0000,
0xFFC542, 0xFFD600, 0x943A00, 0x4A0000,
0xFFC542, 0xFFD600, 0x943A00, 0x4A0000,
};
