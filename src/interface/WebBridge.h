#pragma once

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <vector>
#include <functional>
#include <cstdint>

// -----------------------------------------------------------------------------
// WebBridge
// Handles all JS <-> WASM interop for the web build.
// -----------------------------------------------------------------------------
namespace WebBridge
{
    // Callback signature — fires when the user picks a ROM file.
    using RomLoadedCallback = std::function<void(std::vector<uint8_t>)>;

    void SetRomLoadedCallback(RomLoadedCallback callback);
    void OpenRomFilePicker();
    void SetupRomInput();
}

// -----------------------------------------------------------------------------
// Declarations only — all EM_JS and EMSCRIPTEN_KEEPALIVE definitions
// live in WebBridge.cpp to avoid ODR violations when this header is
// included in multiple translation units.
// -----------------------------------------------------------------------------
extern "C" {
    void js_openRomFilePicker();
    void js_onRomLoaded(uint8_t* data, int length);
    void js_setupRomInput();
}

#endif // __EMSCRIPTEN__