#include "APU.h"
#include "Bus.h"
#include "Timer.h"
#include "../GameBoySettings.h"

/*
static const char read_mask[GB_IO_WAV_END - GB_IO_NR10 + 1] = {
    // NRX0  NRX1  NRX2  NRX3  NRX4
       0x80, 0x3F, 0x00, 0xFF, 0xBF, // NR1X
       0xFF, 0x3F, 0x00, 0xFF, 0xBF, // NR2X
       0x7F, 0xFF, 0x9F, 0xFF, 0xBF, // NR3X
       0xFF, 0xFF, 0x00, 0x00, 0xBF, // NR4X
       0x00, 0x00, 0x70, 0xFF, 0xFF, // NR5X

       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Unused
       // Wave RAM
       0
};
*/


const int SQUARE_DUTY_PATTERNS[4][8] = 
{
    {0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 0}
};
// frequency = 4,194,304 / (262144/(divider * 2^shift))
//           = 4194304/262144 * (divider * 2^shift)
//           = 16 * divider * 2^shift
const int noiseDivisors[8] = { 8, 16, 32, 48, 64, 80, 96, 112 };

uint16_t apuTargetBitMask = 0x1000; //4th bit

// Helper
// Returns the newly calculated frequency, or a value > 2047 if it overflowed.
uint16_t CalculateSweepFrequency(SquareChannel& ch)
{
    uint8_t step = ch.NRx0 & 0x07;
    bool sweepDir = ch.NRx0 & 0x08; // 1 being -, 0 being +
    uint16_t offset = ch.shadowFrequency >> step;
    uint16_t newFrequency = sweepDir ? (ch.shadowFrequency - offset) : (ch.shadowFrequency + offset);

    return newFrequency;
}


APU::APU()
{
    ResetRegisters();
    audioBuffer.reserve(GBSettings::DEVICE_AUDIO_BUFFER_SIZE);
}

void APU::ResetRegisters()
{
    ch1 = {}; ch2 = {}; ch3 = {}; ch4 = {};

    ch1.NRx0 = 0x80;                  ch3.NR30 = 0x7F;
    ch1.NRx1 = 0xBF; ch2.NRx1 = 0x3F; ch3.NR31 = 0xFF; ch4.NR41 = 0xFF;
    ch1.NRx2 = 0xF3; ch2.NRx2 = 0x00; ch3.NR32 = 0x9F; ch4.NR42 = 0x00;
    ch1.NRx3 = 0xFF; ch2.NRx3 = 0xFF; ch3.NR33 = 0xFF; ch4.NR43 = 0x00;
    ch1.NRx4 = 0xBF; ch2.NRx4 = 0xBF; ch3.NR34 = 0xBF; ch4.NR44 = 0xBF;

    NR50 = 0x77;
    NR51 = 0xF3;
    NR52 = 0xF1;

    ResetWaveTable();
    audioBuffer.clear();
}

void APU::ResetWaveTable()
{
    ch3.waveTable.fill(0);
}

void APU::DisableAPU()
{
    ch1 = {}; ch2 = {}; ch3 = {}; ch4 = {};

    ch1.NRx0 = 0x00;                  ch3.NR30 = 0x00;
    ch1.NRx1 = 0x00; ch2.NRx1 = 0x00; ch3.NR31 = 0x00; ch4.NR41 = 0x00;
    ch1.NRx2 = 0x00; ch2.NRx2 = 0x00; ch3.NR32 = 0x00; ch4.NR42 = 0x00;
    ch1.NRx3 = 0x00; ch2.NRx3 = 0x00; ch3.NR33 = 0x00; ch4.NR43 = 0x00;
    ch1.NRx4 = 0x00; ch2.NRx4 = 0x00; ch3.NR34 = 0x00; ch4.NR44 = 0x00;

    ch1.channelEnabled = false;
    ch2.channelEnabled = false;
    ch3.channelEnabled = false;
    ch4.channelEnabled = false;

    //NR50 = 0x00;
    //NR51 = 0x00;
}

uint8_t APU::Read(Address addr)
{
	switch (addr)
	{
    // Master control registers
    case addrNR50: return NR50;
    case addrNR51: return NR51;
    case addrNR52: 
    {
        uint8_t status = (NR52 & 0x80);
        if (ch1.channelEnabled) status |= 0x01;
        if (ch2.channelEnabled) status |= 0x02;
        if (ch3.channelEnabled) status |= 0x04;
        if (ch4.channelEnabled) status |= 0x08;
        return status;
    }

	// Square Channel 1
	case addrNR10: return ch1.NRx0 | 0x80;		  // Top bit unused
	case addrNR11: return (ch1.NRx1 & 0xC0) | 0x3F; // Only Duty (top 2 bits) readable
	case addrNR12: return ch1.NRx2;				  // All bits readable
	case addrNR13: return 0xFF;			          // Write-only register
	case addrNR14: return (ch1.NRx4 & 0x40) | 0xBF; // Only Length Enable (bit 6) readable

    // Square Channel 2
    case addrNR21: return (ch2.NRx1 & 0xC0) | 0x3F;
    case addrNR22: return ch2.NRx2;
    case addrNR23: return 0xFF;
    case addrNR24: return (ch2.NRx4 & 0x40) | 0xBF;

    // Wave Channel 3
    case addrNR30: return ch3.NR30 | 0x7F;          // Only bit 7 (DAC Power) is readable
    case addrNR31: return 0xFF;                     // Write-only
    case addrNR32: return ch3.NR32 | 0x9F;          // Only bits 5-6 (Volume level) are readable
    case addrNR33: return 0xFF;                     // Write-only
    case addrNR34: return (ch3.NR34 & 0x40) | 0xBF; // Only bit 6 (Length Enable) is readable

    // Noise Channel 4
    case addrNR41: return 0xFF;                     // Write Only
    case addrNR42: return ch4.NR42;                 // All bits readable
    case addrNR43: return ch4.NR43;                 // All bits readable
    case addrNR44: return (ch4.NR44 & 0x40) | 0xBF; // Only bit 6 read-write.

	default: return 0xFF;
	}
}

void APU::Write(Address addr, uint8_t data)
{
    // Wave RAM write
    if (addrWaveTable.Contains(addr))
    {
        ch3.waveTable[addrWaveTable.GetOffset(addr)] = data;
        return;
    }

    if (addr == addrNR52)
    {
        // Turning APU off clears all APU registers
        if ((data & 0x80) == 0)
        {
            DisableAPU();
        }
        NR52 = data & 0x8F;
    }

    // APU off makes all registers read-only except for Wave RAM
    if ((NR52 & 0x80) == 0)
    {
        return;
    }

    switch (addr)
    {
    // Master control registers
    case addrNR50:
        NR50 = data;
        break;
    case addrNR51:
        NR51 = data;
        break;

    // Channel 1
    case addrNR10:
        ch1.NRx0 = data;
        break;
    case addrNR11:
        ch1.NRx1 = data;
        ch1.lengthCounter = 64 - (data & 0x3F); // Writing to NRx1 updates the length counter immediately
        break;
    case addrNR12:
        ch1.NRx2 = data;
        if ((data & 0xF8) == 0) ch1.channelEnabled = false;
        break;
    case addrNR13:
        ch1.NRx3 = data;
        break;
    case addrNR14:
        ch1.NRx4 = data;
        if (data & 0x80) TriggerChannel1();
        break;

    // Channel 2
    case addrNR21:
        ch2.NRx1 = data;
        ch2.lengthCounter = 64 - (data & 0x3F);
        break;
    case addrNR22:
        ch2.NRx2 = data;
        if ((data & 0xF8) == 0) ch2.channelEnabled = false;
        break;
    case addrNR23:
        ch2.NRx3 = data;
        break;
    case addrNR24:
        ch2.NRx4 = data;
        if (data & 0x80) TriggerChannel2();
        break;


    // Channel 3
    case addrNR30:
        ch3.NR30 = data;
        ch3.dacEnabled = (data & 0x80) != 0;
        if (!ch3.dacEnabled) ch3.channelEnabled = false; // Turning off the DAC immediately shuts off the channel
        break;
    case addrNR31:
        ch3.NR31 = data;
        ch3.lengthCounter = 256 - data;
        break;
    case addrNR32:
        ch3.NR32 = data;
        break;
    case addrNR33:
        ch3.NR33 = data;
        break;
    case addrNR34:
        ch3.NR34 = data;
        if (data & 0x80)
        {
            TriggerChannel3();
        }
        break;

    // Channel 4
    case addrNR41:
        ch4.NR41 = data;
        ch4.lengthCounter = 64 - (data & 0x3F);
        break;
    case addrNR42:
        ch4.NR42 = data;
        if ((data & 0xF8) == 0) ch4.channelEnabled = false;
        break;
    case addrNR43:
        ch4.NR43 = data;
        break;
    case addrNR44:
        ch4.NR44 = data;
        if (data & 0x80) TriggerChannel4();
        break;
    }

}

void APU::Tick(int cycles)
{
    for (int i = 0; i < cycles; ++i)
    {
        // Tick the Frame Sequencer (512 Hz, once every 8192 dots)
        // APU observes the timer class for div-apu ticks

        //// ticks on falling edge. that is, previous state is 1, current state is 0.
        bool currentTargetBitState = timer->GetDiv() & apuTargetBitMask;
        if (previousTargetBitState && !currentTargetBitState)
        {
            StepDivAPUEvent();
        }
        previousTargetBitState = currentTargetBitState;


        // Tick the Channel Oscillators (High Frequency)
        TickChannel1(1);
        TickChannel2(1);
        TickChannel3(1);
        TickChannel4(1);

        downsampleCounter -= 1.0f;
        while (downsampleCounter <= 0.0f)
        {
            Sample();
            downsampleCounter += sampleRate;
        }
    }
}


//////////////
// Triggers //
//////////////
void APU::TriggerChannel1()
{
    // Trigger Behaviour copy pasted at bottom of this file
    // Ch1 is enabled.
    // Bit 3-7 on means DAC is off, channel is off.
    if ((ch1.NRx2 & 0xF8) == 0)
    {
        ch1.channelEnabled = false;
        return;
    }

    ch1.channelEnabled = true;

    // If length timer expired it is reset.
    if (ch1.lengthCounter == 0)
    {
        ch1.lengthCounter = 64;
    }

    // Frequency Timer Reload
    // Combine lower 8 bits (NRx3) with upper 3 bits (NRx4)
    // The period divider is set to the contents of NR13 and NR14.
    uint16_t frequency = ch1.NRx3 | ((ch1.NRx4 & 0x07) << 8);
    ch1.frequencyTimer = (FREQUENCY_MAX_VALUE - frequency) * T_CYCLES_PER_SQUARE_TICK; // Ticks once per 4 dots

    // Envelope Reload
    ch1.envelopeTimer = ch1.NRx2 & 0x07;
    if (ch1.envelopeTimer == 0) ch1.envelopeTimer = 8; // Hardware quirk: The volume envelope and sweep timers treat a period of 0 as 8.
    ch1.currentVolume = ch1.NRx2 >> 4;

    // Sweep Setup
    ch1.shadowFrequency = frequency;
    ch1.sweepTimer = (ch1.NRx0 >> 4) & 0x07;
    if (ch1.sweepTimer == 0) ch1.sweepTimer = 8;  // See above hardware quirk

    // Sweep is enabled if pace OR shift > 0
    ch1.sweepEnabled = (ch1.sweepTimer > 0) || ((ch1.NRx0 & 0x07) > 0);

    // If the individual step is non-zero, frequency calculation and overflow check are performed immediately.
    uint8_t step = ch1.NRx0 & 0x07;
    if (step > 0)
    {
        if (CalculateSweepFrequency(ch1) > 2047)
        {
            ch1.channelEnabled = false;
        }
    }
}

void APU::TriggerChannel2()
{
    if ((ch2.NRx2 & 0xF8) == 0)
    {
        ch2.channelEnabled = false;
        return;
    }

    ch2.channelEnabled = true;

    if (ch2.lengthCounter == 0)
    {
        ch2.lengthCounter = 64;
    }

    uint16_t frequency = ch2.NRx3 | ((ch2.NRx4 & 0x07) << 8);
    ch2.frequencyTimer = (FREQUENCY_MAX_VALUE - frequency) * T_CYCLES_PER_SQUARE_TICK;

    ch2.envelopeTimer = ch2.NRx2 & 0x07;
    if (ch2.envelopeTimer == 0) ch2.envelopeTimer = 8;
    ch2.currentVolume = ch2.NRx2 >> 4;
}

void APU::TriggerChannel3()
{
    // Channel is only allowed to turn on if the DAC is powered
    if (ch3.dacEnabled)
    {
        ch3.channelEnabled = true;
    }

    if (ch3.lengthCounter == 0)
    {
        ch3.lengthCounter = 256;
    }

    // Frequency Timer Reload
    uint16_t frequency = ch3.NR33 | ((ch3.NR34 & 0x07) << 8);
    ch3.frequencyTimer = (FREQUENCY_MAX_VALUE - frequency) * T_CYCLES_PER_WAVE_TICK; // Ticks once per 2 dots

    // Reset Wave Position
    ch3.wavePosition = 0;
}

void APU::TriggerChannel4()
{
    if ((ch4.NR42 & 0xF8) == 0)
    {
        ch4.channelEnabled = false;
        return;
    }

    // Ch4 is enabled
    ch4.channelEnabled = true;

    // Reset Length Timer
    if (ch4.lengthCounter == 0)
    {
        ch4.lengthCounter = 64;
    }

    // Set Volume
    ch4.envelopeTimer = ch4.NR42 & 0x07;
    if (ch4.envelopeTimer == 0) ch4.envelopeTimer = 8;
    ch4.currentVolume = ch4.NR42 >> 4;

    // Reset LFSR
    ch4.lfsr = 0x7FFF;

    // Frequency Timer Reset
    uint8_t divisorCode = ch4.NR43 & 0x07;  // Clock divider
    uint8_t clockShift = ch4.NR43 >> 4;

    // Lookup table for the base divisor

    // Timer = Divisor << Shift
    ch4.frequencyTimer = noiseDivisors[divisorCode] << clockShift;
}


////////////////////////////////
// High Frequency Oscillators //
////////////////////////////////
void APU::TickChannel1(int cycles)
{
    if (!ch1.channelEnabled) return;

    ch1.frequencyTimer -= cycles;
    while (ch1.frequencyTimer <= 0)
    {
        ch1.frequencyTimer += (2048 - ch1.shadowFrequency) * T_CYCLES_PER_SQUARE_TICK;
        ch1.dutyPointer = (ch1.dutyPointer + 1) % 8;
    }
}
void APU::TickChannel2(int cycles)
{
    if (!ch2.channelEnabled) return;

    ch2.frequencyTimer -= cycles;
    while (ch2.frequencyTimer <= 0)
    {
        uint16_t frequency = ch2.NRx3 | ((ch2.NRx4 & 0x07) << 8);
        ch2.frequencyTimer += (2048 - frequency) * T_CYCLES_PER_SQUARE_TICK;
        ch2.dutyPointer = (ch2.dutyPointer + 1) % 8;
    }
}
void APU::TickChannel3(int cycles)
{
    if (!ch3.channelEnabled || !ch3.dacEnabled) return;

    ch3.frequencyTimer -= cycles;
    while (ch3.frequencyTimer <= 0)
    {
        uint16_t frequency = ch3.NR33 | ((ch3.NR34 & 0x07) << 8);
        ch3.frequencyTimer += (2048 - frequency) * T_CYCLES_PER_WAVE_TICK;

        ch3.wavePosition = (ch3.wavePosition + 1) % 32;
    }
}
void APU::TickChannel4(int cycles)
{
    if (!ch4.channelEnabled) return;

    ch4.frequencyTimer -= cycles;
    while (ch4.frequencyTimer <= 0)
    {
        // Reload timer
        uint8_t divisorCode = ch4.NR43 & 0x07;
        uint8_t clockShift = ch4.NR43 >> 4;
        ch4.frequencyTimer += (noiseDivisors[divisorCode] << clockShift);

        // 2. The LFSR bit-shifting math
        uint16_t bit0 = ch4.lfsr & 0x01;
        uint16_t bit1 = (ch4.lfsr >> 1) & 0x01;
        uint16_t xorResult = bit0 ^ bit1;

        // Shift right
        ch4.lfsr >>= 1;

        // Put the XOR result into Bit 14
        // (First clear bit 14, then OR it with the shifted result)
        ch4.lfsr &= ~(1 << 14);
        ch4.lfsr |= (xorResult << 14);

        // 7-Bit Mode (Width Mode)
        // If Bit 3 of NR43 is set, ALSO put the XOR result into Bit 6
        if (ch4.NR43 & 0x08)
        {
            ch4.lfsr &= ~(1 << 6);
            ch4.lfsr |= (xorResult << 6);
        }
    }
}

////////////
// Events //
////////////
void APU::StepDivAPUEvent()
{
    switch (divAPU)
    {
    case 0: TriggerSoundLengthEvent(); break;
    case 1: break;
    case 2: TriggerSoundLengthEvent(); TriggerCh1SweepEvent(); break;
    case 3: break;
    case 4: TriggerSoundLengthEvent(); break;
    case 5: break;
    case 6: TriggerSoundLengthEvent(); TriggerCh1SweepEvent(); break;
    case 7: TriggerEnvelopeEvent(); break;
    }

    divAPU++;
    if (divAPU > 7) divAPU = 0;
}

void APU::TriggerEnvelopeEvent()
{
    //DRY be damned...
    
    // Channel 1
    uint8_t pace = ch1.NRx2 & 0x07;

    // If pace is 0, the envelope is disabled
    if (pace != 0)
    {
        if (ch1.envelopeTimer > 0) ch1.envelopeTimer--;

        if (ch1.envelopeTimer == 0)
        {
            ch1.envelopeTimer = pace; // Reload the timer

            // Bit 3 determines direction (1 = increase, 0 = decrease)
            uint8_t direction = (ch1.NRx2 & 0x08) ? 1 : 0;

            if (direction == 1 && ch1.currentVolume < 15)
            {
                ch1.currentVolume++;
            }
            else if (direction == 0 && ch1.currentVolume > 0)
            {
                ch1.currentVolume--;
            }
        }
    }

    // Channel 2
    pace = ch2.NRx2 & 0x07;
    if (pace != 0)
    {
        if (ch2.envelopeTimer > 0) ch2.envelopeTimer--;

        if (ch2.envelopeTimer == 0)
        {
            ch2.envelopeTimer = pace;

            uint8_t direction = (ch2.NRx2 & 0x08) ? 1 : 0;

            if (direction == 1 && ch2.currentVolume < 15)
            {
                ch2.currentVolume++;
            }
            else if (direction == 0 && ch2.currentVolume > 0)
            {
                ch2.currentVolume--;
            }
        }
    }

    // Channel 3 does not have volume envelope
    // Channel 4
    pace = ch4.NR42 & 0x07;
    if (pace != 0)
    {
        if (ch4.envelopeTimer > 0) ch4.envelopeTimer--;

        if (ch4.envelopeTimer == 0)
        {
            ch4.envelopeTimer = pace;

            uint8_t direction = (ch4.NR42 & 0x08) ? 1 : 0;

            if (direction == 1 && ch4.currentVolume < 15)
            {
                ch4.currentVolume++;
            }
            else if (direction == 0 && ch4.currentVolume > 0)
            {
                ch4.currentVolume--;
            }
        }
    }

}

void APU::TriggerSoundLengthEvent()
{
    // Channel 1
    if (ch1.NRx4 & 0x40) // If Length Enable bit is set
    {
        if (ch1.lengthCounter > 0) ch1.lengthCounter--;
        if (ch1.lengthCounter == 0) ch1.channelEnabled = false;
    }

    // Channel 2
    if (ch2.NRx4 & 0x40)
    {
        if (ch2.lengthCounter > 0) ch2.lengthCounter--;
        if (ch2.lengthCounter == 0) ch2.channelEnabled = false;
    }

    // Channel 3
    if (ch3.NR34 & 0x40)
    {
        if (ch3.lengthCounter > 0) ch3.lengthCounter--;
        if (ch3.lengthCounter == 0) ch3.channelEnabled = false;
    }

    // Channel 4
    if (ch4.NR44 & 0x40)
    {
        if (ch4.lengthCounter > 0) ch4.lengthCounter--;
        if (ch4.lengthCounter == 0) ch4.channelEnabled = false;
    }
}

void APU::TriggerCh1SweepEvent()
{
    if (ch1.sweepTimer > 0) ch1.sweepTimer--;

    if (ch1.sweepTimer == 0)
    {
        uint8_t pace = (ch1.NRx0 >> 4) & 0x07;
        ch1.sweepTimer = (pace > 0) ? pace : 8; // Reload timer
        uint8_t step = ch1.NRx0 & 0x07; // Individual step

        if (ch1.sweepEnabled && pace > 0)
        {
            bool sweepDir = ch1.NRx0 & 0x08; // 1 being -, 0 being +
            uint16_t offset = ch1.shadowFrequency >> step;
            uint16_t newFrequency = sweepDir ? (ch1.shadowFrequency - offset) : (ch1.shadowFrequency + offset);

            // The Overflow Check
            if (newFrequency > 2047)
            {
                ch1.channelEnabled = false;
                return;
            }
            
            if (step > 0)
            {
                ch1.shadowFrequency = newFrequency;
                ch1.NRx3 = newFrequency & 0xFF;
                ch1.NRx4 = (ch1.NRx4 & 0xF8) | ((newFrequency >> 8) & 0x07);
                ch1.frequencyTimer = (2048 - ch1.shadowFrequency) * T_CYCLES_PER_SQUARE_TICK;

                // Second overflow check
                if (CalculateSweepFrequency(ch1) > 2047)
                {
                    ch1.channelEnabled = false;
                }
            }
        }
    }

}



///////////////////////////
// Audio Mixer Interface //
///////////////////////////
void APU::Sample()
{
    float a1 = GetChannel1Amplitude();
    float a2 = GetChannel2Amplitude();
    float a3 = GetChannel3Amplitude();
    float a4 = GetChannel4Amplitude();


    // Map from [0, 15] to [-1.0, 1.0]
    a1 = ch1.channelEnabled ? (a1 / 7.5f) - 1.0f : 0.0f;
    a2 = ch2.channelEnabled ? (a2 / 7.5f) - 1.0f : 0.0f;
    a3 = (ch3.channelEnabled && ch3.dacEnabled) ? (a3 / 7.5f) - 1.0f : 0.0f;
    a4 = ch4.channelEnabled ? (a4 / 7.5f) - 1.0f : 0.0f;


    // Extra debugging/for fun mix
    a1 = GBSettings::ENABLE_AUDIO_CHANNEL_1 ? a1 : 0.0f;
    a2 = GBSettings::ENABLE_AUDIO_CHANNEL_2 ? a2 : 0.0f;
    a3 = GBSettings::ENABLE_AUDIO_CHANNEL_3 ? a3 : 0.0f;
    a4 = GBSettings::ENABLE_AUDIO_CHANNEL_4 ? a4 : 0.0f;

    // Panning (NR51)
    // Bits 0-3 map to Right output. Bits 4-7 map to Left output.
    float leftMix = 0.0f;
    float rightMix = 0.0f;

    if (NR51 & 0x10) leftMix += a1;
    if (NR51 & 0x01) rightMix += a1;

    if (NR51 & 0x20) leftMix += a2;
    if (NR51 & 0x02) rightMix += a2;

    if (NR51 & 0x40) leftMix += a3;
    if (NR51 & 0x04) rightMix += a3;

    if (NR51 & 0x80) leftMix += a4;
    if (NR51 & 0x08) rightMix += a4;

    // Master Volume (NR50)
    // Volumes are 0-7, but formula adds 1, so multiplier is 1 to 8.
    int leftVolume = ((NR50 >> 4) & 0x07) + 1;
    int rightVolume = (NR50 & 0x07) + 1;

    // Multiply mix by volume (Max 8), then divide by 32 to normalize back to [-1.0, 1.0]
    // (4 channels * max volume 8 = max potential sum of 32)
    leftMix = (leftMix * leftVolume) / 32.0f;
    rightMix = (rightMix * rightVolume) / 32.0f;

    // Push to buffer
    audioBuffer.push_back(leftMix);
    audioBuffer.push_back(rightMix);
}

uint8_t APU::GetChannel1Amplitude()
{
    if (!ch1.channelEnabled) return 0;

    uint8_t dutyIndex = ch1.NRx1 >> 6;

    return (SQUARE_DUTY_PATTERNS[dutyIndex][ch1.dutyPointer] == 1) ? ch1.currentVolume : 0;
}

uint8_t APU::GetChannel2Amplitude()
{
    if (!ch2.channelEnabled) return 0;
    uint8_t dutyIndex = ch2.NRx1 >> 6;
    return (SQUARE_DUTY_PATTERNS[dutyIndex][ch2.dutyPointer] == 1) ? ch2.currentVolume : 0;
}

uint8_t APU::GetChannel3Amplitude()
{
    if (!ch3.channelEnabled || !ch3.dacEnabled) return 0;

    // Wave RAM holds 32 4-bit samples.
    // wavePosition is 0-31.
    uint8_t sampleByte = ch3.waveTable[ch3.wavePosition / 2];
    uint8_t sample = (ch3.wavePosition % 2 == 0) ? (sampleByte >> 4) : (sampleByte & 0x0F);

    // Apply the Volume Shift (NR32)
    uint8_t volumeShift = (ch3.NR32 >> 5) & 0x03;
    switch (volumeShift)
    {
    case 0: return 0;          // Mute
    case 1: return sample;     // 100%
    case 2: return sample >> 1; // 50%
    case 3: return sample >> 2; // 25%
    default: return 0;
    }
}

uint8_t APU::GetChannel4Amplitude()
{
    if (!ch4.channelEnabled) return 0;

    // In the LFSR, a 0 bit means "HIGH" output, a 1 bit means "LOW" output (silence)
    if ((ch4.lfsr & 0x01) == 0)
        return ch4.currentVolume;
    else
        return 0;
}


// Editor stuff
void APU::GetChannelVolumes(float& a, float& b, float& c, float& d)
{
    a = ch1.channelEnabled ? (ch1.currentVolume / 15.0f) : 0.0f;
    b = ch2.channelEnabled ? (ch2.currentVolume / 15.0f) : 0.0f;
    d = ch4.channelEnabled ? (ch4.currentVolume / 15.0f) : 0.0f;

    if (!ch3.channelEnabled || !ch3.dacEnabled)
    {
        c = 0.0f;
    }
    else
    {
        c = GetChannel3Amplitude();
        c = (ch3.channelEnabled && ch3.dacEnabled) ? (c / 7.5f) - 1.0f : 0.0f;
    }
}

/*
Trigger (Write-only): Writing any value to NR14 with this bit set triggers the channel, causing the following to occur:

Ch1 is enabled.
If length timer expired it is reset.
The period divider is set to the contents of NR13 and NR14.
Envelope timer is reset.
Volume is set to contents of NR12 initial volume.
Sweep does several things.
    CH1 period value is copied to the “shadow register”.
    The “sweep timer” is reset.
    The “enabled flag” is set if either the sweep pace or individual step are non-zero, cleared otherwise.
    If the individual step is non-zero, frequency calculation and overflow check are performed immediately.

Channel 3:
Ch3 is enabled.
If the length timer expired it is reset.
The period divider is set to the contents of NR33 and NR34.
Volume is set to contents of NR32 initial volume.
Wave RAM index is reset, but its not refilled.

Channel 4:
Ch4 is enabled.
If the length timer expired it is reset.
Envelope timer is reset.
Volume is set to contents of NR42 initial volume.
LFSR bits are reset.
*/




