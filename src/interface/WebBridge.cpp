#ifdef __EMSCRIPTEN__

#include "WebBridge.h"

// -----------------------------------------------------------------------------
// EM_JS definition — compiled exactly once here in WebBridge.cpp.
// -----------------------------------------------------------------------------
//EM_JS(void, js_openRomFilePicker, (), {
//    const input = document.createElement('input');
//    input.type = 'file';
//    input.accept = '.gb,.gbc';
//
//    input.onchange = function(e)
// {

EM_JS(void, js_openRomFilePicker, (), {
    document.getElementById('rom-file-input').click();
    });


EM_JS(void, js_setupRomInput, (), {
    const input = document.createElement('input');
    input.type = 'file';
    input.id = 'rom-file-input';
    input.accept = '.gb,.gbc';
    input.style.display = 'none';
    input.addEventListener('change', function() {
        const file = input.files[0];
        if (!file) return;
        const reader = new FileReader();
        reader.onload = function(ev)
 {
const bytes = new Uint8Array(ev.target.result);
const ptr = _malloc(bytes.length);
HEAPU8.set(bytes, ptr);
_js_onRomLoaded(ptr, bytes.length);
_free(ptr);
input.value = '';
};
reader.readAsArrayBuffer(file);
});
document.body.appendChild(input);
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
    void SetupRomInput()
    {
        js_setupRomInput();
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