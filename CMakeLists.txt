cmake_minimum_required(VERSION 3.24)

# Enable Solution Folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(FeelsEmuGB VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- DEPENDENCIES ---

include(FetchContent)

if(NOT EMSCRIPTEN)
    # ----------------------------------------------------------------
    # NATIVE ONLY
    # ----------------------------------------------------------------

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

    set(SDL_SHARED ON  CACHE BOOL "" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS  OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.0
    )
    FetchContent_MakeAvailable(SDL2)
    set_target_properties(SDL2 SDL2main PROPERTIES FOLDER "External/SDL")

    # GLAD — not needed on web, WebGL functions are provided by the browser
    add_library(glad STATIC "src/glad/src/glad.c")
    target_include_directories(glad PUBLIC "src/glad/include")
    set_target_properties(glad PROPERTIES FOLDER "External/GLAD")

endif()

# --- IMGUI (shared, compiles on both targets) ---

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
target_include_directories(imgui PUBLIC
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/backends"
)

if(EMSCRIPTEN)
    # SHELL: keeps "-s USE_SDL=2" as one token.
    # Without it, CMake splits on the space and clang receives "USE_SDL=2"
    # as a bare filename, causing "no such file or directory" errors.
    target_compile_options(imgui PRIVATE "SHELL:-s USE_SDL=2")
else()
    target_link_libraries(imgui PRIVATE SDL2::SDL2)
endif()

set_target_properties(imgui PROPERTIES FOLDER "External/ImGui")

# --- MAIN PROJECT ---

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
list(FILTER SOURCES EXCLUDE REGEX "src/glad/.*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})

# PCH
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)

# --- LINKING ---

if(EMSCRIPTEN)
    # ----------------------------------------------------------------
    # WEB
    # ----------------------------------------------------------------

    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

    # Compile flags — SHELL: required for all -s KEY=VALUE pairs
    target_compile_options(${PROJECT_NAME} PRIVATE
        "SHELL:-s USE_SDL=2"
        "SHELL:-s USE_WEBGL2=1"
        "SHELL:-s FULL_ES3=1"
    )

    # Link flags as a single space-separated string (LINK_FLAGS property)
    # USE_SDL=2              - Emscripten SDL2 port (input, audio, window)
    # USE_WEBGL2=1           - WebGL2 context
    # FULL_ES3=1             - Full GLES3 API surface
    # MIN/MAX_WEBGL_VERSION  - Lock to WebGL2 only
    # ALLOW_MEMORY_GROWTH    - WASM heap can grow at runtime
    # FORCE_FILESYSTEM=1     - Include Emscripten's full FS layer
    # EXPORTED_RUNTIME_METHODS - Expose FS to JS for syncfs() save calls
    # -lidbfs.js             - IndexedDB FS backend for persistent saves
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS
        "-s USE_SDL=2 \
         -s USE_WEBGL2=1 \
         -s FULL_ES3=1 \
         -s MIN_WEBGL_VERSION=2 \
         -s MAX_WEBGL_VERSION=2 \
         -s ALLOW_MEMORY_GROWTH=1 \
         -s FORCE_FILESYSTEM=1 \
         -s EXPORTED_RUNTIME_METHODS=['FS','ccall','cwrap','lengthBytesUTF8','stringToUTF8','HEAPU8'] \
         -s EXPORTED_FUNCTIONS=['_malloc','_free','_main'] \
         -lidbfs.js \
         -s ASSERTIONS=1"
         #--shell-file ${CMAKE_SOURCE_DIR}/shell.html
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

else()
    # ----------------------------------------------------------------
    # NATIVE
    # ----------------------------------------------------------------

    find_package(OpenGL REQUIRED)

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

endif()