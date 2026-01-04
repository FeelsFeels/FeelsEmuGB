# Game Boy Emulator - Project Status

**Date:** January 2026
**Status:** Playable / Alpha
**Current Focus:** Stability & Game Compatibility

## 🏆 Current Achievements
The emulator is currently capable of running major commercial titles including *Pokémon Red/Blue*, *Tetris*, *DuckTales*, and *Super Mario Land 2*.

### Core Systems
- [x] **CPU:** Full LR35902 instruction set with accurate cycle timing (M-Cycles).
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

---

## 🐞 Known Quirks / Bugs
2.  **Sprite Limits:** The 10-sprites-per-line limit is not implemented (visuals might be *too* perfect compared to real hardware).
3.  **CGB Mode:** CGB Flags are detected, but CGB-specific registers (VRAM banking, Palette RAM) are ignored. Games play in DMG (Monochrome) mode.

---
