//#pragma once
//
//#ifdef __EMSCRIPTEN__
//
//#include <emscripten.h>
//#include <vector>
//#include <functional>
//#include <cstdint>
//#include "WebBridge.h"
//
//// -----------------------------------------------------------------------------
//// JS bridge definition — lives here so it's compiled exactly once.
//// -----------------------------------------------------------------------------
//
//// Forward declared so js_onRomLoaded can call back into WebBridge internals.
//// Defined in WebBridge.cpp.
//extern "C" {
//    EMSCRIPTEN_KEEPALIVE
//        void js_onRomLoaded(uint8_t* data, int length);
//}
//
//EM_JS(void, js_openRomFilePicker, (), {
//    const input = document.createElement('input');
//    input.type = 'file';
//    input.accept = '.gb,.gbc';
//
//    input.onchange = function(e)
// {
//const file = e.target.files[0];
//if (!file) return;
//
//const reader = new FileReader();
//reader.onload = function(ev)
//{
//const bytes = new Uint8Array(ev.target.result);
//const len = bytes.length;
//
//// Allocate a buffer in WASM memory, copy bytes in,
//// call C++, then free. C++ copies into a std::vector
//// before we free so there's no dangling pointer.
//const ptr = Module._malloc(len);
//Module.HEAPU8.set(bytes, ptr);
//Module._js_onRomLoaded(ptr, len);
//Module._free(ptr);
//};
//reader.readAsArrayBuffer(file);
//};
//
//input.click();
//    });
//
//
//
//#endif // __EMSCRIPTEN__