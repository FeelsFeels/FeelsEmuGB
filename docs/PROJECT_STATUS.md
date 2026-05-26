# Game Boy Emulator - Project Status

**Date:** May 2026
**Status:** Playable
**Current Focus:** CGB support -> Accuracy

## Current Achievements
The emulator is currently capable of running commercial titles including *Pokémon Red/Blue* and *The Legend of Zelda: Link's Awakening*.

### Core Systems
- [x] **CPU:** Full LR35902 instruction set.
- [x] **PPU:** Scanline-based rendering, Background map, Window layer, Sprites (8x8 & 8x16).
- [x] **Input:** Keyboard mapping to Game Boy Joypad.
- [x] **Audio:** Not fully accurate, but functional.

### Cartridge & Memory
- [x] **MBC1, MBC3, MBC5:** MBC3 does not support RTC yet.
- [x] **Persistence (.sav):** SRAM is automatically saved to disk when closing program (when write to RAM is detected).
- [x] **Save States (.state):** Full binary snapshot of CPU, PPU, RAM, and Registers (APU registers not saved).

### Debugging Tools
- [x] **VRAM Viewer:** View raw tile data in memory.
- [x] **Map Viewer:** 256x256 Render of the current Background/Window with Viewport Overlay.
- [x] **Audio Mixer:** Toggle Specific audio channels on/off.

---

## 🚧 Roadmap & "To-Do" List
*Items to tackle when returning to the project.*

### 1. CGB Support
- So many titles are locked behind CGB support. Donkey Kong country, Zelda...

### 2. MBC
- **MBC3 RTC:** Real Time Clock not done.
- **MBC5:** Implemented, but need to test more.

### 3. Accuracy Improvements
- **OAM DMA:** Currently instantaneous. Should block CPU for ~160 microseconds.
- **Pixel FIFO:** PPU currently renders scanlines instantly. A Pixel FIFO implementation is required for precise mid-scanline effects (like the wobbling in *Prehistorik Man*).
- **Joypad Interrupts:**
- **APU** - DIV-APU ticks based on raw cycles instead of depending on the div register.
- **Failed Tests for Sound: Too many to count. 01, 03, 04, 05, 07, 08, 09, 10, 11, 12** 

### 4. Accuracy Tracking
#### Blargg
- **cgb_sound:** I haven't even implemented cgb.
- **cpu_instrs:** Passed
- **dmg_sound:**
  - 01-registers: NR10-NR51 and wave RAM write/read, Failed #2
  - 03-trigger: Enabling in first half of length period should clock length. Failed #3
  - 04-sweep: If shift=0 and period=0, trigger disables. Failed #10
  - 05-sweep details: Exiting negate mode after calculation disables channel. Failed #4
  - 07-len sweep period sync: Powering up APU MODs next frame time with 8192 Failed #5
  - 08-len ctr during power 40 00 40 40 3CF589B4 Failed
  - 09-wave read while on Failed
  - 10-wave trigger while on
  - 11-regs after power: Powering off shouldn't affect NR41 Failed #4
  - 12-wave write while on
- **instr_timing:** Passed
- **interrupt_time:** 00 00 00 00 08 0D 00 00 00 00 08 0D 7F8F4AAF Failed (Which is actually a pass! On DMG mode.)
- **mem_timing:** Passed
- **oam_bug:** 
  - 01-lcd sync: Turning LCD on starts too late in scanline Failed #2
  - 02-causes: LD DE, $FE00: INC DE. Failed #2
  - 04-scanline timing: INC DE at first corruption Failed #3
  - 05-timing_bug: Should corrupt at beginning of first scanline Failed #2
  - 07-timing effect: Failed
  - 08-instr effect: 00000000 INC/DEC rp pattern is wrong Failed #2
- **halt_bug:** 
#### Mooneye


#### Graphics
- **dmg_acid2:** Passed.

### 5. Editor Improvements
- **Sprite Palette Editor:** Very cool.
- **Pause button:** Especially so we can inspect the state of the gameboy.

---

## 🐞 Known Quirks / Bugs
2.  **CGB Mode:** CGB Flags are detected, but CGB-specific registers (VRAM banking, Palette RAM) are ignored. Games play in DMG (Monochrome) mode.
3.  **ROM Loading:** Hardcodey implementation. All roms have to be in roms/ folder. roms/ folder has to exist.

---

## 💭 Dreams and thoughts and admissions
1. **I wanna make my own GB/GBC game!!!! :** Then I can replace the testroms folder with MY GAME.

2.  **Architecture:** Currently rendered entirely via imgui textures. Not feasible for the future, if I want to "ship it". The dream is to try something like Emscripten to compile it and host it on the web.  
Need to find a better way to display UI along with the actual game itself.  

3.  **Features:** I want to implement the link cable. Local link cable should be possible, but I want to try to make it work over the network as well, somehow.  
I also think it'll be cool if I had something like a split screen play with your friend over the web kinda thing. I feel it'll be really cool for the purposes of speedrunning challenges. That's a challenge for another day though.  
I played with DeSmuME once, and I was so impressed with how many debugging features they had. I want to try that too...  
Also want to make a tracker for the GB audio system. Would be fun.  

---
