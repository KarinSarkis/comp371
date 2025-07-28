#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector> 

class Windmill {
public:
    Windmill();
    ~Windmill();

    void setup(GLuint shaderProgram);
    void draw(const glm::mat4& view, const glm::mat4& projection, float currentTime);

private:
    GLuint m_shaderProgram;

    GLuint m_baseVAO, m_baseVBO;
    GLuint m_headVAO, m_headVBO;
    GLuint m_bladesVAO, m_bladesVBO;

    GLuint m_baseTextureID;
    GLuint m_whiteTextureID;

    void setupPart(GLuint& vao, GLuint& vbo, const std::vector<float>& vertices);
};