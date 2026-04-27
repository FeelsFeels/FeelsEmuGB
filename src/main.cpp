#include "utils/VFS/VFS.h"
#include "utils/Filepaths.h"
#include "GameBoySettings.h"
#include "core/Gameboy.h"
#include "editor/Editor.h"
#include "interface/Renderer.h"

#include <glad/glad.h>
#include <SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

// Screen dimensions
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;



int main(int argc, char* argv[])
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    VFS::MountDirectory("", Filepaths::roms);

#ifdef GAMEBOY_DOCTOR
#ifdef _MSC_VER
    FILE* fp;
    freopen_s(&fp, "cpu_log.txt", "w", stdout);
#else
    freopen("cpu_log.txt", "w", stdout);
#endif
#endif

#pragma region Setup
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    const char* glsl_version = "#version 460";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Create Window with OpenGL flag
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Gameboy Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);

    // Create OpenGL Context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable V-Sync

    SDL_GetTicks64(); // Initialize timer

    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec desiredSpec = {};
    desiredSpec.freq = 44100;
    desiredSpec.format = AUDIO_F32; // 32-bit floats
    desiredSpec.channels = 2;       // Stereo
    desiredSpec.samples = 2048;     // Buffer size
    desiredSpec.callback = nullptr; // Manually push data
    SDL_AudioDeviceID audioDeviceID = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, nullptr, 0);
    // Unpause the device to let it start playing
    SDL_PauseAudioDevice(audioDeviceID, 0);


    // Load OpenGL pointers
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup ImGui Context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking (Optional)

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
#pragma endregion

    GameBoy* gameboy = new GameBoy();
    gameboy->SetAudioSampleRate(static_cast<float>(GBHardWare::MASTER_CLOCK) / static_cast<float>(GBSettings::DEVICE_AUDIO_OUTPUT_RATE));
    //gameboy->InsertCartridge("blargg_test_roms/cpu_instrs/individual/01-special.gb");

    Renderer renderer;
    renderer.Init();
    GLuint gameTexture = renderer.CreateTexture(160, 144);

    Editor editor;
    editor.Init(&renderer);

    // Keyboard input
    SDLInputProvider inputHandler;

    bool done = false;
    while (!done)
    {
        uint64_t startTime = SDL_GetTicks64(); // Start of frame

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
     
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        editor.Render(*gameboy);

        // GAMEBOY LOOP HERE
        // Run CPU for 1 frame
        // Update Texture with PPU pixels
        const InputState& keyboard = inputHandler.Poll();
        gameboy->UpdateInput(inputHandler);

        int cyclesThisFrame = 0;
        while (cyclesThisFrame < GBSettings::CYCLES_PER_FRAME)
        {
            int cycles = gameboy->Update();
            if (cycles <= 0) break;

            const auto& audioBuffer = gameboy->GetAudioBuffer();
            if (audioBuffer.size() >= GBSettings::DEVICE_AUDIO_BUFFER_SIZE)
            {
                if (GBSettings::RUNTIME_SPEED != 1.0f)
                {
                    // FAST FORWARD MODE
                    // If the audio queue has space, queue it.
                    // If queue is full, don't do anything. Drop audio.
                    if (SDL_GetQueuedAudioSize(audioDeviceID) <= 4096 * sizeof(float) * 2)
                    {
                        SDL_QueueAudio(audioDeviceID, audioBuffer.data(), audioBuffer.size() * sizeof(float));
                    }
                }
                else
                {
                    // NORMAL MODE
                    // Strict Audio Syncing
                    while (SDL_GetQueuedAudioSize(audioDeviceID) > 4096 * sizeof(float) * 2)
                    {
                        SDL_Delay(1);
                    }
                    SDL_QueueAudio(audioDeviceID, audioBuffer.data(), audioBuffer.size() * sizeof(float));
                }

                gameboy->ClearAudioBuffer();
            }

            cyclesThisFrame += cycles;
        }
        renderer.UpdateTexture(gameTexture, 160, 144, gameboy->GetScreenBuffer().data());


        // Savestates shortcuts
        if (keyboard[SDL_SCANCODE_LSHIFT].down&& keyboard[SDL_SCANCODE_O].pressed)
        {
            gameboy->SaveState();
        }
        if (keyboard[SDL_SCANCODE_LSHIFT].down && keyboard[SDL_SCANCODE_P].pressed)
        {
            gameboy->LoadState();
        }

        // Show game in ImGui Window
        static float aspectRatio = 160.f / 144.f;
        ImGui::Begin("Game Viewport");
        {
            ImVec2 region = ImGui::GetContentRegionAvail();
            float targetH = region.x / aspectRatio;
            
            ImGui::Image((void*)(intptr_t)gameTexture, ImVec2(region.x, targetH));
        }
        ImGui::End();
        ImGui::Render();



        // Rendering
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f); // Clear background
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw ImGui Data
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        SDL_GL_SwapWindow(window);

        // Limit Speed 
        if (GBSettings::RUNTIME_SPEED > 0.0f)
        {
            uint64_t endTime = SDL_GetTicks64();
            uint64_t frameTime = endTime - startTime;
            float targetTime = GBSettings::TARGET_FRAME_TIME / GBSettings::RUNTIME_SPEED;
            if (frameTime < targetTime)
            {
                SDL_Delay(static_cast<uint32_t>(targetTime - frameTime));
            }
        }
    }

    delete gameboy;

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}