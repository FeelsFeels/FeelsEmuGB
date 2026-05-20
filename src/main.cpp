#include "utils/VFS/VFS.h"
#include "utils/Filepaths.h"
#include "GameBoySettings.h"
#include "core/Gameboy.h"
#include "editor/Editor.h"
#include "interface/Renderer.h"
#include <SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#include "interface/WebBridge.h"
#include "interface/WebFileOpener.h"
#include "editor/WebImGuiDefaultLayout.h"
#else
#include <glad/glad.h>
#endif

#include <iostream>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// -----------------------------------------------------------------------------
// All per-frame state lives here so the loop callback can access it.
// On native this is just a convenient bundle; on web it's necessary because
// the loop function is called from outside our stack.
// -----------------------------------------------------------------------------
struct AppState
{
    SDL_Window* window = nullptr;
    SDL_GLContext     gl_context = nullptr;
    SDL_AudioDeviceID audioDeviceID = 0;

    GameBoy* gameboy = nullptr;
    Renderer* renderer = nullptr;
    Editor* editor = nullptr;

    SDLInputProvider inputHandler;

    GLuint gameTexture = 0;
    bool   done = false;
};

static AppState g_app; // single global instance

// -----------------------------------------------------------------------------
// One frame of work.
// Called by emscripten_set_main_loop on web.
// -----------------------------------------------------------------------------
void LoopIteration()
{
    AppState& app = g_app;

#ifdef __EMSCRIPTEN__
    // Save delta time for the web, so we know how many cycles are supposed to be run in that period of time.
    // For desktop, we sync framerate to audio at 1.0x speed
    static uint64_t lastTime = SDL_GetTicks64();
    uint64_t now = SDL_GetTicks64();
    double deltaMs = static_cast<double>(now - lastTime);
    lastTime = now;

    // How many GB cycles should have elapsed in this real time delta?
                           //v milliseconds to seconds conversion
    double targetCycles = (deltaMs / 1000.0) * GBHardWare::MASTER_CLOCK * GBSettings::RUNTIME_SPEED;
    int cycleBudget = static_cast<int>(std::min(targetCycles, (double)GBSettings::CYCLES_PER_FRAME * 2));
#else
    uint64_t startTime = SDL_GetTicks64();
    int cycleBudget = GBSettings::CYCLES_PER_FRAME;
#endif

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            app.done = true;
        if (event.type == SDL_WINDOWEVENT &&
            event.window.event == SDL_WINDOWEVENT_CLOSE &&
            event.window.windowID == SDL_GetWindowID(app.window))
            app.done = true;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    app.editor->Render(*app.gameboy);

    // --- Input ---
    const InputState& keyboard = app.inputHandler.Poll();
    app.gameboy->UpdateInput(app.inputHandler);

    int cyclesThisFrame = 0;
    while (cyclesThisFrame < cycleBudget)
    {
        int cycles = app.gameboy->Update();
        if (cycles <= 0) break;

        const auto& audioBuffer = app.gameboy->GetAudioBuffer();
        if (audioBuffer.size() >= GBSettings::DEVICE_AUDIO_BUFFER_SIZE)
        {
            if (GBSettings::RUNTIME_SPEED != 1.0f)
            {
                // Fast forward: drop audio if queue is full
                if (SDL_GetQueuedAudioSize(app.audioDeviceID) <= 4096 * sizeof(float) * 2)
                    SDL_QueueAudio(app.audioDeviceID, audioBuffer.data(), audioBuffer.size() * sizeof(float));
            }
            else
            {
                // NORMAL MODE
                // Strict Audio Syncing
                // On web we CANNOT busy-wait (SDL_Delay blocks the browser thread).
                // Instead we just skip queuing if the buffer is already full.
                // The browser's audio scheduler will catch up on its own.
#ifndef __EMSCRIPTEN__
                while (SDL_GetQueuedAudioSize(app.audioDeviceID) > 4096 * sizeof(float) * 2)
                {
                    SDL_Delay(1);
                }
                SDL_QueueAudio(app.audioDeviceID, audioBuffer.data(), audioBuffer.size() * sizeof(float));
#else
                if (SDL_GetQueuedAudioSize(app.audioDeviceID) <= 4096 * sizeof(float) * 2)
                    SDL_QueueAudio(app.audioDeviceID, audioBuffer.data(), audioBuffer.size() * sizeof(float));
#endif
            }

            app.gameboy->ClearAudioBuffer();
        }

        cyclesThisFrame += cycles;
    }

    app.renderer->UpdateTexture(app.gameTexture, 160, 144, app.gameboy->GetScreenBuffer().data());

    // --- Savestate shortcuts ---
    if (keyboard[SDL_SCANCODE_LSHIFT].down && keyboard[SDL_SCANCODE_O].pressed)
        app.gameboy->SaveState();
    if (keyboard[SDL_SCANCODE_LSHIFT].down && keyboard[SDL_SCANCODE_P].pressed)
        app.gameboy->LoadState();

    // --- ImGui game viewport ---
    static float aspectRatio = 160.f / 144.f;
    ImGui::Begin("Game Viewport");
    {
        ImVec2 region = ImGui::GetContentRegionAvail();
        float targetH = region.x / aspectRatio;
        ImGui::Image((void*)(intptr_t)app.gameTexture, ImVec2(region.x, targetH));
    }
    ImGui::End();
    ImGui::Render();

    // --- Render ---
    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(app.window);

    // --- Frame limiter ---
#ifndef __EMSCRIPTEN__
    if (GBSettings::RUNTIME_SPEED > 0.0f)
    {
        uint64_t endTime = SDL_GetTicks64();
        uint64_t frameTime = endTime - startTime;
        float targetTime = GBSettings::TARGET_FRAME_TIME / GBSettings::RUNTIME_SPEED;
        if (frameTime < targetTime)
            SDL_Delay(static_cast<uint32_t>(targetTime - frameTime));
    }
#endif
}

// -----------------------------------------------------------------------------
// Emscripten requires a plain function pointer (no captures) for the loop.
// This trampoline satisfies that while letting LoopIteration() be a normal fn.
// -----------------------------------------------------------------------------
#ifdef __EMSCRIPTEN__
static void EmscriptenLoopCallback()
{
    LoopIteration();

    // Emscripten doesn't have a "stop the loop" signal — we cancel it by
    // throwing a JS exception via emscripten_cancel_main_loop() when done.
    if (g_app.done)
        emscripten_cancel_main_loop();
}
#endif

// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

#ifndef __EMSCRIPTEN__
    VFS::MountDirectory("", Filepaths::roms);
#endif

#ifdef GAMEBOY_DOCTOR
#ifdef _MSC_VER
    FILE* fp;
    freopen_s(&fp, "cpu_log.txt", "w", stdout);
#else
    freopen("cpu_log.txt", "w", stdout);
#endif
#endif

    // -------------------------------------------------------------------------
    // SDL + GL setup
    // -------------------------------------------------------------------------
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Desktop uses OpenGL 4.6 core. Web uses OpenGL ES 3.0 (WebGL2).
#ifdef __EMSCRIPTEN__
    const char* glsl_version = "#version 300 es";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    const char* glsl_version = "#version 460";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
#endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    g_app.window = SDL_CreateWindow("Gameboy Emulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);

    g_app.gl_context = SDL_GL_CreateContext(g_app.window);
    SDL_GL_MakeCurrent(g_app.window, g_app.gl_context);
    SDL_GL_SetSwapInterval(1);

    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec desiredSpec = {};
    desiredSpec.freq = 44100;
    desiredSpec.format = AUDIO_F32;
    desiredSpec.channels = 2;
    desiredSpec.samples = 2048;
    desiredSpec.callback = nullptr;
    g_app.audioDeviceID = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, nullptr, 0);
    SDL_PauseAudioDevice(g_app.audioDeviceID, 0);

#ifndef __EMSCRIPTEN__
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(g_app.window, g_app.gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // -------------------------------------------------------------------------
    // App objects
    // -------------------------------------------------------------------------
    g_app.gameboy = new GameBoy();
    g_app.gameboy->SetAudioSampleRate(static_cast<float>(GBHardWare::MASTER_CLOCK) /static_cast<float>(GBSettings::DEVICE_AUDIO_OUTPUT_RATE));

    g_app.renderer = new Renderer();
    g_app.renderer->Init();
    g_app.gameTexture = g_app.renderer->CreateTexture(160, 144);

    g_app.editor = new Editor();
    g_app.editor->Init(g_app.renderer);

    // -------------------------------------------------------------------------
    // Main loop
    // -------------------------------------------------------------------------
#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
    LoadDefaultWebLayout();
    // 0 = use requestAnimationFrame (~60fps), 1 = simulate infinite loop
    WebBridge::SetupRomInput();
    WebBridge::SetRomLoadedCallback([](std::vector<uint8_t> romData) {
        g_app.gameboy->InsertCartridge(std::move(romData));
    });
    emscripten_set_main_loop(EmscriptenLoopCallback, 0, 1);
#else
    
    //g_app.gameboy->GetCPU().StartTracing("./mylog.log");

    while (!g_app.done)
        LoopIteration();
#endif

    // -------------------------------------------------------------------------
    // Cleanup
    // -------------------------------------------------------------------------
    g_app.gameboy->GetCPU().StopTracing();

    delete g_app.gameboy;
    delete g_app.renderer;
    delete g_app.editor;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(g_app.gl_context);
    SDL_DestroyWindow(g_app.window);
    SDL_Quit();

    return 0;
}