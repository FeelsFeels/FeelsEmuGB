cmake_minimum_required(VERSION 3.24)

# Enable Solution Folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set Project Name
project(FeelsEmuGB VERSION 0.0.1 LANGUAGES C CXX)

# Set Startup Project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Output directories for binaries and libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- DEPENDENCIES ---

# SDL2
include(FetchContent)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.0
)
FetchContent_MakeAvailable(SDL2)
set_target_properties(SDL2 SDL2main PROPERTIES FOLDER "External/SDL")

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.90.4-docking
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/imgui_demo.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)
target_link_libraries(imgui PRIVATE SDL2::SDL2)
target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR}" "${imgui_SOURCE_DIR}/backends")
set_target_properties(imgui PROPERTIES FOLDER "External/ImGui")

# GLAD
add_library(glad STATIC "src/glad/src/glad.c")
target_include_directories(glad PUBLIC "src/glad/include")
set_target_properties(glad PROPERTIES FOLDER "External/GLAD")

# --- MAIN PROJECT ---

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
list(FILTER SOURCES EXCLUDE REGEX "src/glad/.*")

# PRESERVE FOLDER STRUCTURE IN VS
# This tells CMake to replicate the physical folder structure inside the Solution Explorer
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(OpenGL REQUIRED)

# PCH
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    imgui 
    glad 
    SDL2::SDL2 
    SDL2::SDL2main
    OpenGL::GL
)

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()