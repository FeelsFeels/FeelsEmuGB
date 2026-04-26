# Game Boy Emulator - Project Status

**Date:** April 2026
**Status:** Playable
**Current Focus:** Stability & Game Compatibility

## Current Achievements
The emulator is currently capable of running major commercial titles including *Pokémon Red/Blue*, *Tetris*, *DuckTales*, and *Super Mario Land 2*.

### Core Systems
- [x] **CPU:** Full LR35902 instruction set.
- [x] **PPU:** Scanline-based rendering, Background map, Window layer, Sprites (8x8 & 8x16).
- [x] **Interrupts:** VBlank, STAT (LY=LYC, Mode 0-2), Timer (DIV/TIMA), Joypad.
- [x] **Audio:** Not fully accurate.
- [x] **Input:** Keyboard mapping to Game Boy Joypad.

### Cartridge & Memory
- [x] **MBC1, MBC3, MBC5:** Does not support RTC yet. MBC5 is untested.
- [x] **Persistence (.sav):** SRAM is automatically saved to disk when closing program (when write to RAM is detected).
- [x] **Save States (.state):** Full binary snapshot of CPU, PPU, RAM, and Registers.

### Debugging Tools
- [x] **VRAM Viewer:** View raw tile data in memory.
- [x] **Map Viewer:** 256x256 Render of the current Background/Window with Viewport Overlay.
- [x] **Audio Mixer:** Toggle Specific audio channels on/off.
- [x] **Register Viewer:** Real-time register inspection.

---

## 🚧 Roadmap & "To-Do" List
*Items to tackle when returning to the project.*

### 1. CGB Support
- So many titles are locked behind CGB support. Donkey Kong country, Zelda...

### 2. MBC
- **MBC3 RTC:** Real Time Clock not done.
- **MBC5:** Implemented, but need to implement CGB to test.

### 3. Accuracy Improvements
- **OAM DMA:** Currently instantaneous. Should block CPU for ~160 microseconds.
- **Pixel FIFO:** PPU currently renders scanlines instantly. A Pixel FIFO implementation is required for precise mid-scanline effects (like the wobbling in *Prehistorik Man*).
- **APU** - DIV-APU ticks based on raw cycles instead of depending on the div register.
- **List of failed Blargg tests:** cgb_sound, dg_sound, interrupt_time, mem_timing, oam_bug, halt_bug.
- **Failed Tests for Sound: Too many to count. 01, 03, 04, 05, 07, 08, 09, 10, 11, 12** 

### 4. Editor Improvements
- **Sprite Palette Editor:** Very cool.
- **Pause button:** Especially so we can inspect the state of the gameboy.

---

## 🐞 Known Quirks / Bugs
2.  **Sprite Limits:** The 10-sprites-per-line limit is not implemented (visuals might be *too* perfect compared to real hardware).
3.  **CGB Mode:** CGB Flags are detected, but CGB-specific registers (VRAM banking, Palette RAM) are ignored. Games play in DMG (Monochrome) mode.

---

## 💭 Dreams and thoughts and admissions
1.  **Architecture:** Currently rendered entirely via imgui textures. Not feasible for the future, if I want to "ship it". The dream is to try something like Emscripten to compile it and host it on the web.  
Need to find a better way to display UI along with the actual game itself.  

2.  **Features:** I want to implement the link cable. Local link cable should be possible, but I want to try to make it work over the network as well, somehow.  
I also think it'll be cool if I had something like a split screen play with your friend over the web kinda thing. I feel it'll be really cool for the purposes of speedrunning challenges. That's a challenge for another day though.  
I played with DeSmuME once, and I was so impressed with how many debugging features they had. I want to try that too...  
Also want to make a tracker for the GB audio system. Would be fun.  

---
