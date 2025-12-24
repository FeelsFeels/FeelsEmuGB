#pragma once
#include <glad/glad.h>
#include <vector>

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();

    // Initialize generic OpenGL states
    void Init();

    // Create a texture (e.g., 160x144 for GameBoy) and return its ID
    GLuint CreateTexture(int width, int height);

    // Upload new pixel data to the GPU
    // 'data' should be RGBA (32-bit per pixel)
    void UpdateTexture(GLuint textureID, int width, int height, const void* data);

private:
    // We will add Shader/FBO stuff here later!
};