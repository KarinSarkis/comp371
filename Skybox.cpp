#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Skybox.hpp"
#include <iostream>

// Define GL_CHECK_ERROR for internal use within Skybox.cpp
#define GL_CHECK_ERROR() \
    do { \
        GLenum err; \
        while ((err = glGetError()) != GL_NO_ERROR) { \
            std::cerr << "OpenGL Error " << err << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        } \
    } while (0)


// Skybox vertex data
float skyboxVertices[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f
};

// Skybox Vertex Shader Source
const char* skyboxVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 TexCoords;

void main()
{
    TexCoords = aPos;
    vec4 clipPos = projection * view * vec4(aPos, 1.0);
    gl_Position = vec4(clipPos.xy, 1.0, 1.0);
}
)";

// Skybox Fragment Shader Source
const char* skyboxFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, TexCoords);
}
)";


Skybox::Skybox() {
    createGLResources();
}

Skybox::~Skybox() {
    destroyGLResources();
}

Skybox::Skybox(Skybox&& other) noexcept
    : m_vao(other.m_vao), m_vbo(other.m_vbo),
      m_textureID(other.m_textureID), m_shaderProgram(other.m_shaderProgram)
{
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_textureID = 0;
    other.m_shaderProgram = 0;
}

Skybox& Skybox::operator=(Skybox&& other) noexcept {
    if (this != &other) {
        destroyGLResources();
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_textureID = other.m_textureID;
        m_shaderProgram = other.m_shaderProgram;

        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_textureID = 0;
        other.m_shaderProgram = 0;
    }
    return *this;
}

void Skybox::createGLResources() {
    glGenVertexArrays(1, &m_vao);
    GL_CHECK_ERROR();
    glGenBuffers(1, &m_vbo);
    GL_CHECK_ERROR();

    glBindVertexArray(m_vao);
    GL_CHECK_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    GL_CHECK_ERROR();
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    GL_CHECK_ERROR();
    glEnableVertexAttribArray(0);
    GL_CHECK_ERROR();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    GL_CHECK_ERROR();
    glBindVertexArray(0);
    GL_CHECK_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_CHECK_ERROR();

    m_shaderProgram = createShaderProgram(skyboxVertexShaderSource, skyboxFragmentShaderSource);
    GL_CHECK_ERROR();
    if (m_shaderProgram == 0) {
        std::cerr << "Failed to create skybox shader program." << std::endl;
    } else {
        GLint aPosLoc = glGetAttribLocation(m_shaderProgram, "aPos");
        std::cout << "Skybox Shader aPos location: " << aPosLoc << std::endl;
        if (aPosLoc != 0) {
            std::cerr << "WARNING: aPos attribute location is not 0, it's " << aPosLoc << ". This might be an issue." << std::endl;
        }
        GL_CHECK_ERROR();
    }
}

void Skybox::destroyGLResources() {
    if (m_vao != 0) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo != 0) glDeleteBuffers(1, &m_vbo);
    if (m_textureID != 0) glDeleteTextures(1, &m_textureID);
    if (m_shaderProgram != 0) glDeleteProgram(m_shaderProgram);
}

bool Skybox::load(const std::vector<std::string>& faces) {
    if (faces.size() != 6) {
        std::cerr << "Skybox requires 6 faces." << std::endl;
        return false;
    }
    m_textureID = loadCubemap(faces);
    return m_textureID != 0;
}

void Skybox::draw(const glm::mat4& view, const glm::mat4& projection) {
    if (m_vao == 0 || m_shaderProgram == 0 || m_textureID == 0) {
        std::cerr << "Skybox not initialized or loaded properly. Skipping draw." << std::endl;
        return;
    }

    glDepthMask(GL_FALSE);
    GL_CHECK_ERROR();
    glDisable(GL_CULL_FACE);
    GL_CHECK_ERROR();

    glUseProgram(m_shaderProgram);
    GL_CHECK_ERROR();

    glDepthFunc(GL_LEQUAL);
    GL_CHECK_ERROR();

    // Remove translation from the view matrix for the skybox
    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(skyboxView));
    GL_CHECK_ERROR();
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    GL_CHECK_ERROR();

    glUniform1i(glGetUniformLocation(m_shaderProgram, "skybox"), 0);
    GL_CHECK_ERROR();

    glBindVertexArray(m_vao);
    GL_CHECK_ERROR();
    glActiveTexture(GL_TEXTURE0);
    GL_CHECK_ERROR();
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
    GL_CHECK_ERROR();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    GL_CHECK_ERROR();
    glBindVertexArray(0);
    GL_CHECK_ERROR();

    glUseProgram(0);
    GL_CHECK_ERROR();

    glEnable(GL_CULL_FACE);
    GL_CHECK_ERROR();
    glDepthMask(GL_TRUE);
    GL_CHECK_ERROR();
    glDepthFunc(GL_LESS);
    GL_CHECK_ERROR();
}

// Static helper functions
GLuint Skybox::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint Skybox::createShaderProgram(const char* vsSource, const char* fsSource) {
    GLuint vertex = compileShader(GL_VERTEX_SHADER, vsSource);
    GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fsSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        program = 0;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return program;
}

GLuint Skybox::loadCubemap(const std::vector<std::string>& faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    GL_CHECK_ERROR();
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    GL_CHECK_ERROR();

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            if (nrChannels == 4) format = GL_RGBA;
            else if (nrChannels == 1) format = GL_RED;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            GL_CHECK_ERROR();
            stbi_image_free(data);
        } else {
            std::cerr << "Cubemap texture failed to load at path: " << faces[i] << ". Reason: " << stbi_failure_reason() << std::endl;
            stbi_image_free(data);
            return 0;
        }
    }
    stbi_set_flip_vertically_on_load(true);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL_CHECK_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL_CHECK_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL_CHECK_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL_CHECK_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    GL_CHECK_ERROR();

    return textureID;
}