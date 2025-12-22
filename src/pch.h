#pragma once

#define NOMINMAX
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <fstream>
#include <filesystem>


#include <SDL.h>
#include <glad/glad.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>


#include "utils/VFS/VFS.h"
#include "utils/Debug.h"

#include "core/Address.h"
#include "core/Interrupts.h"