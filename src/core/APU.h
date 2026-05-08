#pragma once
#include <array>

#include "Address.h"
#include "Register.h"

constexpr int FREQUENCY_MAX_VALUE = 2 << 10;
constexpr int T_CYCLES_PER_SQUARE_TICK = 4;
constexpr int T_CYCLES_PER_WAVE_TICK = 2;

constexpr int MAX_LENGTH_TIMER_CH_1_2_4 = 64;
constexpr int MAX_LENGTH_TIMER_CH3      = 256;

class Timer;

struct SquareChannel
{
	uint8_t NRx4;	// TL-- -FFF: Trigger, Length Enable, Frequency MSB
	uint8_t NRx3;	// FFFF FFFF: Frequency LSB
	uint8_t NRx2;	// VVVV APPP: Volume initial, Envelope direction, Sweep pace
	uint8_t NRx1;	// DDLL LLLL: Duty, Length Timer
	uint8_t NRx0;	// -PPP NSSS: Sweep pace, Direction, Step

	// --- Internal State ---
	bool channelEnabled = false;

	// The wave
	uint16_t frequencyTimer = 0;	// How many T-Cycles / dots until the next step?
	uint8_t  dutyPointer = 0;		// Pointer to the waveform table

	// Length
	uint16_t lengthCounter = 0;		// Mutes channel after counter finishes.

	// Envelope (volume change over time)
	uint8_t currentVolume = 0;
	uint8_t envelopeTimer = 0;

	// Sweep (Ch 1 Only)
	uint16_t shadowFrequency = 0;	// When channel triggered, period value copied here.
	uint8_t  sweepTimer = 0;	
	bool     sweepEnabled = false;
};

struct WaveChannel
{
	uint8_t NR30; // E--- ----: DAC power
	uint8_t NR31; // LLLL LLLL: Length
	uint8_t NR32; // -VV- ----: Volume code(00 = 0 %, 01 = 100 %, 10 = 50 %, 11 = 25 %)
	uint8_t NR33; // FFFF FFFF: Frequency LSB
	uint8_t NR34; // TL-- -FFF: Trigger, Length enable, Frequency MSB

	// --- Internal State ---
	std::array<uint8_t, 16> waveTable;

	uint16_t frequencyTimer = 0;
	uint8_t wavePosition = 0;	// Index into Wave Table's nibbles

	uint16_t lengthCounter = 0;

	bool channelEnabled = false;
	bool dacEnabled = false;

};

struct NoiseChannel
{
	uint8_t NR41; // --LL LLLL: Length
	uint8_t NR42; // VVVV APPP: Starting volume, Envelope add mode, period
	uint8_t NR43; // SSSS WDDD: Clock shift, Width mode of LFSR, Divisor code
	uint8_t NR44; // TL-- ----: Trigger, Length enable

	// --- Internal State ---
	bool channelEnabled = false;

	uint16_t lengthCounter = 0;

	uint8_t currentVolume = 0;
	uint8_t envelopeTimer = 0;

	uint16_t lfsr = 0x7FFF;
	int frequencyTimer = 0; // Shortcut because I don't want to deal with deal with a prescaler
};



class APU
{
public:
	APU();

	void AttachTimer(Timer* p) { timer = p; }

	void ResetRegisters();
	void ResetWaveTable();
	void DisableAPU();

	uint8_t Read(Address addr);
	void Write(Address addr, uint8_t data);

	void Tick(int cycles);
	
	void SetSampleRate(float rate) { sampleRate = rate; downsampleCounter = rate; }
	void Sample();

	std::vector<float>& GetAudioBuffer() { return audioBuffer; }
	void ClearAudioBuffer() { audioBuffer.clear(); }

	void GetChannelVolumes(float& ch1, float& ch2, float& ch3, float& ch4);

private:
	uint8_t GetChannel1Amplitude();
	uint8_t GetChannel2Amplitude();
	uint8_t GetChannel3Amplitude();
	uint8_t GetChannel4Amplitude();

	void TriggerChannel1();
	void TriggerChannel2();
	void TriggerChannel3();
	void TriggerChannel4();

	void StepDivAPUEvent();
	void TriggerEnvelopeEvent();
	void TriggerSoundLengthEvent();
	void TriggerCh1SweepEvent();

	void TickChannel1(int cycles);
	void TickChannel2(int cycles);
	void TickChannel3(int cycles);
	void TickChannel4(int cycles);

	SquareChannel ch1;
	SquareChannel ch2;
	WaveChannel ch3;
	NoiseChannel ch4;

	ByteRegister NR52; // Audio Master control
	ByteRegister NR51; // Sound panning
	ByteRegister NR50; // Master Volume

	// APU Timer shortcut. Instead of ticking div-apu based on the system timer, I count 8192 cycles instead.
	// TODO: Accuracy. 
	Timer* timer;
	bool previousTargetBitState = false;

	uint8_t divAPU = 0;

	std::vector<float> audioBuffer;
	float downsampleCounter = 0.0f;
	float sampleRate = 0.0f;
};

