#ifdef __EMSCRIPTEN__

#include "WebBridge.h"

// -----------------------------------------------------------------------------
// EM_JS definition — compiled exactly once here in WebBridge.cpp.
// -----------------------------------------------------------------------------
EM_JS(void, js_openRomFilePicker, (), {
    const input = document.createElement('input');
    input.type = 'file';
    input.accept = '.gb,.gbc';

    input.onchange = function(e)
 {
const file = e.target.files[0];
if (!file) return;

const reader = new FileReader();
reader.onload = function(ev)
{
const bytes = new Uint8Array(ev.target.result);
const len = bytes.length;

const ptr = Module._malloc(len);
Module.HEAPU8.set(bytes, ptr);
Module._js_onRomLoaded(ptr, len);
Module._free(ptr);
};
reader.readAsArrayBuffer(file);
};

input.click();
    });

// -----------------------------------------------------------------------------
namespace WebBridge
{
    static RomLoadedCallback s_romLoadedCallback;

    void SetRomLoadedCallback(RomLoadedCallback callback)
    {
        s_romLoadedCallback = std::move(callback);
    }

    void OpenRomFilePicker()
    {
        js_openRomFilePicker();
    }
}

// -----------------------------------------------------------------------------
// Called from JS with a pointer into WASM memory and byte length.
// Immediately copies into a vector so JS can free the buffer right after.
// -----------------------------------------------------------------------------
extern "C" {
    EMSCRIPTEN_KEEPALIVE
        void js_onRomLoaded(uint8_t* data, int length)
    {
        if (!data || length <= 0) return;

        std::vector<uint8_t> romData(data, data + length);

        if (WebBridge::s_romLoadedCallback)
            WebBridge::s_romLoadedCallback(std::move(romData));
    }
}

#endif // __EMSCRIPTEN__