#include "Renderer.h"

Renderer::~Renderer()
{
    // Cleanup if we store lists of textures later
}


void Renderer::Init()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLuint Renderer::CreateTexture(int width, int height)
{
    GLuint id;
    // 1. Generate and Bind (Required for WebGL) [cite: 60]
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // 2. Allocate storage [cite: 61]
    // Use glTexImage2D instead of glTextureStorage2D for WebGL compatibility
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // 3. Set parameters on the currently bound texture (GL_TEXTURE_2D) [cite: 61]
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Unbind to prevent accidental modifications elsewhere
    glBindTexture(GL_TEXTURE_2D, 0);

    return id;
}

void Renderer::UpdateTexture(GLuint textureID, int width, int height, const void* data)
{
    // 1. Must bind the texture first [cite: 61]
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 2. Upload using the standard SubImage call [cite: 61]
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);
}



// Below is OpenGL 4.5 code
// To support the web, and since the graphics code is so basic, let's just downgrade it.

//void Renderer::Init()
//{
//    // Maybe enable some blend modes if needed later
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//}
//
//GLuint Renderer::CreateTexture(int width, int height)
//{
//    GLuint id;
//    glCreateTextures(GL_TEXTURE_2D, 1, &id);
//
//    // Allocate storage (RGBA8)
//    glTextureStorage2D(id, 1, GL_RGBA8, width, height);
//
//    // Nearest Neighbor filtering is MANDATORY for crisp pixel art
//    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//    // Clamp to edge prevents "bleeding" at the sides
//    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//    return id;
//}
//
//void Renderer::UpdateTexture(GLuint textureID, int width, int height, const void* data)
//{
//    // Upload pixel data to the GPU
//    glTextureSubImage2D(textureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
//}