#include "Windmill.hpp"
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <stb_image.h>


// Function to load a texture
GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Texture loaded: " << path << " (ID: " << textureID << ", " << width << "x" << height << "px)" << std::endl;
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}


// Helper function to create vertex data for a colored cube
std::vector<float> createCubeVertices(glm::vec3 color) {
    return {
        // positions          // colors          // texture coords
        // Back face
        -0.5f, -0.5f, -0.5f,  color.x, color.y, color.z,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  color.x, color.y, color.z,  2.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  color.x, color.y, color.z,  2.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  color.x, color.y, color.z,  2.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  color.x, color.y, color.z,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  color.x, color.y, color.z,  0.0f, 0.0f,

        // Front face
        -0.5f, -0.5f,  0.5f,  color.x, color.y, color.z,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  color.x, color.y, color.z,  2.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  color.x, color.y, color.z,  2.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  color.x, color.y, color.z,  2.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  color.x, color.y, color.z,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  color.x, color.y, color.z,  0.0f, 0.0f,

        // Left Face
        -0.5f,  0.5f,  0.5f,  color.x, color.y, color.z,  2.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  color.x, color.y, color.z,  2.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  color.x, color.y, color.z,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  color.x, color.y, color.z,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  color.x, color.y, color.z,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  color.x, color.y, color.z,  2.0f, 0.0f,

        // Right Face
         0.5f,  0.5f,  0.5f,  color.x, color.y, color.z,  2.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  color.x, color.y, color.z,  2.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  color.x, color.y, color.z,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  color.x, color.y, color.z,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  color.x, color.y, color.z,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  color.x, color.y, color.z,  2.0f, 0.0f,

        // Bottom Face
        -0.5f, -0.5f, -0.5f,  color.x, color.y, color.z,  0.0f, 2.0f,
         0.5f, -0.5f, -0.5f,  color.x, color.y, color.z,  2.0f, 2.0f,
         0.5f, -0.5f,  0.5f,  color.x, color.y, color.z,  2.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  color.x, color.y, color.z,  2.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  color.x, color.y, color.z,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  color.x, color.y, color.z,  0.0f, 2.0f,

        // Top Face
        -0.5f,  0.5f, -0.5f,  color.x, color.y, color.z,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  color.x, color.y, color.z,  2.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  color.x, color.y, color.z,  2.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  color.x, color.y, color.z,  2.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  color.x, color.y, color.z,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  color.x, color.y, color.z,  0.0f, 1.0f
    };
}

// Helper function to create vertex data for a colored blade
std::vector<float> createBladeVertices(glm::vec3 color) {
    return {
        // positions          // colors          // texture coords
        -0.05f, -0.5f, 0.0f,  color.x, color.y, color.z,  0.0f, 0.0f,
         0.05f, -0.5f, 0.0f,  color.x, color.y, color.z,  1.0f, 0.0f,
         0.05f,  0.5f, 0.0f,  color.x, color.y, color.z,  1.0f, 1.0f,

        -0.05f, -0.5f, 0.0f,  color.x, color.y, color.z,  0.0f, 0.0f,
         0.05f,  0.5f, 0.0f,  color.x, color.y, color.z,  1.0f, 1.0f,
        -0.05f,  0.5f, 0.0f,  color.x, color.y, color.z,  0.0f, 1.0f
    };
}


Windmill::Windmill()
    : m_shaderProgram(0),
      m_baseVAO(0), m_baseVBO(0),
      m_headVAO(0), m_headVBO(0),
      m_bladesVAO(0), m_bladesVBO(0),
      m_baseTextureID(0), m_whiteTextureID(0)
{
}

Windmill::~Windmill() {
    glDeleteVertexArrays(1, &m_baseVAO);
    glDeleteBuffers(1, &m_baseVBO);
    glDeleteVertexArrays(1, &m_headVAO);
    glDeleteBuffers(1, &m_headVBO);
    glDeleteVertexArrays(1, &m_bladesVAO);
    glDeleteBuffers(1, &m_bladesVBO);
    glDeleteTextures(1, &m_baseTextureID);
    glDeleteTextures(1, &m_whiteTextureID);
}

void Windmill::setupPart(GLuint& vao, GLuint& vbo, const std::vector<float>& vertices) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    GLsizei stride = 8 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Windmill::setup(GLuint shaderProgram) {
    m_shaderProgram = shaderProgram;

    // --- Load Textures ---
    m_baseTextureID = loadTexture("assets/bricks.jpg");

    glGenTextures(1, &m_whiteTextureID);
    glBindTexture(GL_TEXTURE_2D, m_whiteTextureID);
    unsigned char whitePixel[] = { 255, 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    std::cout << "White texture created (ID: " << m_whiteTextureID << ")" << std::endl;


    float baseWidthDepth = 3.0f;
    float baseHeight = 15.0f;
    glm::vec3 baseColor = glm::vec3(0.6f, 0.4f, 0.2f);

    std::vector<float> baseVertices = {
        // positions                                  // colors          // texture coords (U, V)
        // Back face (Z = -baseWidthDepth/2)
        -baseWidthDepth/2.0f, -baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 0.0f,
         baseWidthDepth/2.0f, -baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 0.0f,
         baseWidthDepth/2.0f,  baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 5.0f,
         baseWidthDepth/2.0f,  baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 5.0f,
        -baseWidthDepth/2.0f,  baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 5.0f,
        -baseWidthDepth/2.0f, -baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 0.0f,

        // Front face (Z = baseWidthDepth/2)
        -baseWidthDepth/2.0f, -baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 0.0f,
         baseWidthDepth/2.0f, -baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 0.0f,
         baseWidthDepth/2.0f,  baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 5.0f,
         baseWidthDepth/2.0f,  baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 5.0f,
        -baseWidthDepth/2.0f,  baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 5.0f,
        -baseWidthDepth/2.0f, -baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 0.0f,

        // Left face (X = -baseWidthDepth/2)
        -baseWidthDepth/2.0f,  baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 0.0f,
        -baseWidthDepth/2.0f,  baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 5.0f,
        -baseWidthDepth/2.0f, -baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 5.0f,
        -baseWidthDepth/2.0f, -baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 5.0f,
        -baseWidthDepth/2.0f, -baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 0.0f,
        -baseWidthDepth/2.0f,  baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 0.0f,

        // Right face (X = baseWidthDepth/2)
         baseWidthDepth/2.0f,  baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 0.0f,
         baseWidthDepth/2.0f,  baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 5.0f,
         baseWidthDepth/2.0f, -baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 5.0f,
         baseWidthDepth/2.0f, -baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 5.0f,
         baseWidthDepth/2.0f, -baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 0.0f,
         baseWidthDepth/2.0f,  baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 0.0f,

        // Bottom face (Y = -baseHeight/2)
        -baseWidthDepth/2.0f, -baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 2.0f,
         baseWidthDepth/2.0f, -baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 2.0f,
         baseWidthDepth/2.0f, -baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 0.0f,
         baseWidthDepth/2.0f, -baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 0.0f,
        -baseWidthDepth/2.0f, -baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 0.0f,
        -baseWidthDepth/2.0f, -baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 2.0f,

        // Top face (Y = baseHeight/2)
        -baseWidthDepth/2.0f,  baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 1.0f,
         baseWidthDepth/2.0f,  baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 1.0f,
         baseWidthDepth/2.0f,  baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 0.0f,
         baseWidthDepth/2.0f,  baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  2.0f, 0.0f,
        -baseWidthDepth/2.0f,  baseHeight/2.0f,  baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 0.0f,
        -baseWidthDepth/2.0f,  baseHeight/2.0f, -baseWidthDepth/2.0f,  baseColor.x, baseColor.y, baseColor.z,  0.0f, 1.0f
    };
    setupPart(m_baseVAO, m_baseVBO, baseVertices);


    std::vector<float> headVertices = createCubeVertices(glm::vec3(0.7f, 0.7f, 0.7f));
    setupPart(m_headVAO, m_headVBO, headVertices);

    std::vector<float> bladeVertices = createBladeVertices(glm::vec3(0.5f, 0.5f, 0.5f));
    setupPart(m_bladesVAO, m_bladesVBO, bladeVertices);

    std::cout << "Windmill setup complete." << std::endl;
}

void Windmill::draw(const glm::mat4& view, const glm::mat4& projection, float currentTime) {
    if (m_shaderProgram == 0) {
        std::cerr << "Warning: Windmill shader program not set." << std::endl;
        return;
    }

    glUseProgram(m_shaderProgram);

    GLint modelLoc = glGetUniformLocation(m_shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(m_shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(m_shaderProgram, "projection");
    GLint textureSamplerLoc = glGetUniformLocation(m_shaderProgram, "textureSampler");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(textureSamplerLoc, 0);

    glDisable(GL_CULL_FACE);

    // --- Hierarchical Transformation ---

    glm::mat4 baseModel = glm::mat4(1.0f);

    float baseHeightLocal = 15.0f;
    float windmillOverallScale = 4.0f;

    glm::vec3 windmillPosition = glm::vec3(-625.73f, 53.98f, -350.15f);
    baseModel = glm::translate(baseModel, windmillPosition);
    baseModel = glm::scale(baseModel, glm::vec3(windmillOverallScale, windmillOverallScale, windmillOverallScale));

    glBindTexture(GL_TEXTURE_2D, m_baseTextureID);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(baseModel));
    glBindVertexArray(m_baseVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    glm::mat4 headModel = glm::mat4(1.0f);

    float headHeight = 2.5f;
    float headTranslateY = (baseHeightLocal / 2.0f) + (headHeight / 2.0f);
    headModel = glm::translate(headModel, glm::vec3(0.0f, headTranslateY, 0.0f));

    float headRotationAngle = currentTime * 0.2f;
    headModel = glm::rotate(headModel, headRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    float headWidthDepth = 3.0f;
    headModel = glm::scale(headModel, glm::vec3(headWidthDepth, headHeight, headWidthDepth));

    headModel = baseModel * headModel;

    glBindTexture(GL_TEXTURE_2D, m_whiteTextureID);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(headModel));
    glBindVertexArray(m_headVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    glm::mat4 bladesModel = glm::mat4(1.0f);

    float bladesTranslateZ = (headWidthDepth / 2.0f) - 0.5f;
    bladesModel = glm::translate(bladesModel, glm::vec3(0.0f, 0.0f, bladesTranslateZ));

    float bladeRotationAngle = currentTime * 2.0f;
    bladesModel = glm::rotate(bladesModel, bladeRotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));

    bladesModel = glm::scale(bladesModel, glm::vec3(0.5f, 0.5f, 0.5f));

    bladesModel = headModel * bladesModel;

    glBindTexture(GL_TEXTURE_2D, m_whiteTextureID);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bladesModel));
    glBindVertexArray(m_bladesVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    int numBlades = 4;
    float bladeLength = 5.0f;
    float bladeWidth = 1.0f;

    for (int i = 0; i < numBlades; ++i) {
        glm::mat4 individualBladeModel = bladesModel;

        float angleOffset = glm::radians(360.0f / numBlades * i);
        individualBladeModel = glm::rotate(individualBladeModel, angleOffset, glm::vec3(0.0f, 0.0f, 1.0f));

        individualBladeModel = glm::translate(individualBladeModel, glm::vec3(0.0f, bladeLength / 2.0f, 0.0f));

        individualBladeModel = glm::scale(individualBladeModel, glm::vec3(bladeWidth, bladeLength, 1.0f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(individualBladeModel));
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
    glUseProgram(0);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error after windmill draw: " << err << std::endl;
    }
}