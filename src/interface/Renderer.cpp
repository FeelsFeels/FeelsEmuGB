#include "Renderer.h"

Renderer::~Renderer()
{
    // Cleanup if we store lists of textures later
}

void Renderer::Init()
{
    // Maybe enable some blend modes if needed later
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLuint Renderer::CreateTexture(int width, int height)
{
    GLuint id;
    glCreateTextures(GL_TEXTURE_2D, 1, &id);

    // Allocate storage (RGBA8)
    glTextureStorage2D(id, 1, GL_RGBA8, width, height);

    // Nearest Neighbor filtering is MANDATORY for crisp pixel art
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Clamp to edge prevents "bleeding" at the sides
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return id;
}

void Renderer::UpdateTexture(GLuint textureID, int width, int height, const void* data)
{
    // Upload pixel data to the GPU
    glTextureSubImage2D(textureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
}