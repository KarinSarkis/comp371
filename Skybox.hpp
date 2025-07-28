#pragma once

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Skybox {
public:
    Skybox();
    ~Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;
    Skybox(Skybox&&) noexcept;
    Skybox& operator=(Skybox&&) noexcept;

    // Loads the cubemap textures.
    bool load(const std::vector<std::string>& faces);
    // Draws the skybox.
    void draw(const glm::mat4& view, const glm::mat4& projection);

    // Getter for texture ID for debugging
    GLuint getTextureID() const { return m_textureID; }

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_textureID = 0;
    GLuint m_shaderProgram = 0;

    // Creates OpenGL resources (VAO, VBO, Shader Program).
    void createGLResources();
    // Destroys OpenGL resources.
    void destroyGLResources();

    // Helper functions (implemented in .cpp)
    // Compiles a single shader.
    static GLuint compileShader(GLenum type, const char* source);
    // Creates a shader program from vertex and fragment shader sources.
    static GLuint createShaderProgram(const char* vsSource, const char* fsSource);
    // Loads a cubemap texture from a list of face paths.
    static GLuint loadCubemap(const std::vector<std::string>& faces);
};