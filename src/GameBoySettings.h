
struct GBSettings
{
    static const int CYCLES_PER_FRAME;
    static float TARGET_FRAME_TIME; // 1000.0f / 59.7f = ~16.74ms
    static float RUNTIME_SPEED;

    static constexpr float DEVICE_AUDIO_OUTPUT_RATE = 44100.0f;
    static constexpr int DEVICE_AUDIO_BUFFER_SIZE = 4096; // 2 channels(left and right), so 2048 samples for SDL to queue.
                                                          // 2048 / 44100 = 0.0464399s = 4.64ms worth of sound queued when buffer is full.
    static bool ENABLE_AUDIO_CHANNEL_1;
    static bool ENABLE_AUDIO_CHANNEL_2;
    static bool ENABLE_AUDIO_CHANNEL_3;
    static bool ENABLE_AUDIO_CHANNEL_4;
};

struct GBHardWare
{
    // 1 Machine cycle = 4 T-Cycle. CPU instructions run on M-cycles.
    static constexpr int MASTER_CLOCK         = 4194304;

    static constexpr int TIMER_DIV            = 16384;

    static constexpr int DIV_APU_PERIOD       = 8192;       // T Cycles per audio tick

    static constexpr int APU_CH1_PERIOD_CLOCK = 1048576;    // Once per 4 dots (THIS ACTUALLY DESCRIBES FREQUENCY THOUGH, PERIOD CLOCK IS THE NAME OF THE REGISTER)
    static constexpr int APU_CH3_PERIOD_CLOCK = 2097152;    // Once per 2 dots
    static constexpr int APU_CH4_PERIOD_CLOCK = 262144;     // Once per 16 dots
};