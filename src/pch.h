#pragma once

#define NOMINMAX
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <unordered_map>


#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>        // WebGL2 / GLES3 — provided by Emscripten
#else
#include <glad/glad.h>        // Desktop GL loader
#endif
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>


#include "utils/VFS/VFS.h"
#include "utils/Debug.h"
#include "utils/Savestates.h"

#include "core/Address.h"
#include "core/Interrupts.h"