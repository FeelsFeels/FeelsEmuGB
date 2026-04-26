# Game Boy Emulator - Project Status

**Date:** January 2026
**Status:** Playable
**Current Focus:** Stability & Game Compatibility

## Current Achievements
The emulator is currently capable of running major commercial titles including *Pokémon Red/Blue*, *Tetris*, *DuckTales*, and *Super Mario Land 2*.

### Core Systems
- [x] **CPU:** Full LR35902 instruction set.
- [x] **PPU:** Scanline-based rendering, Background map, Window layer, Sprites (8x8 & 8x16).
- [x] **Interrupts:** VBlank, STAT (LY=LYC, Mode 0-2), Timer (DIV/TIMA), Joypad.
- [x] **Timer:** Cycle-accurate DIV and TIMA implementation.
- [x] **Input:** Keyboard mapping to Game Boy Joypad.

### Cartridge & Memory
- [x] **MBC1:** Supports Banking Mode 0/1, RAM Banking, ROM Banking.
- [x] **Persistence (.sav):** SRAM is automatically saved to disk (with "Dirty Flag" optimization).
- [x] **Save States (.state):** Full binary snapshot of CPU, PPU, RAM, and Registers.

### Debugging Tools
- [x] **VRAM Viewer:** View raw tile data in memory.
- [x] **Map Viewer:** 256x256 Render of the current Background/Window with Viewport Overlay.
- [x] **Register Viewer:** Real-time register inspection.

---

## 🚧 Roadmap & "To-Do" List
*Items to tackle when returning to the project.*

### 1. Audio (APU) - **High Priority**
*Currently, the emulator is silent.*
- **Channel 1 (Pulse):** Sweep & Envelope logic.
- **Channel 2 (Pulse):** Basic Envelope logic.
- **Channel 3 (Wave):** Arbitrary waveform playback (32-byte buffer).
- **Channel 4 (Noise):** LFSR (Linear Feedback Shift Register) for percussion.
- **Mixer:** Downsampling 512Hz signals to 44.1kHz for SDL/OpenAL.

### 2. MBC Edge Cases
- **MBC3 RTC:** The Real-Time Clock allows *Pokémon* to track day/night cycles. Currently stubbed. Needs to tick seconds/minutes and save/load from the `.sav` file.
- **MBC5:** Required for later games like *Pokémon Yellow* (sometimes) or *Zelda DX*.

### 3. Accuracy Improvements
- **OAM DMA:** Currently instantaneous. Should block CPU for ~160 microseconds.
- **Pixel FIFO:** PPU currently renders scanlines instantly. A Pixel FIFO implementation is required for precise mid-scanline effects (like the wobbling in *Prehistorik Man*).
- **APU** - DIV-APU ticks based on raw cycles instead of depending on the div register.
- **List of failed Blargg tests:** cgb_sound, dg_sound, interrupt_time, mem_timing, oam_bug, halt_bug.
- **Failed Tests for Sound:** 

### 4. Editor Improvements
- **Window viewport box visualizer:** Maybe in red.
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
