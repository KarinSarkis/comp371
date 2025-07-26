#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#define GLEW_STATIC 1
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

using namespace glm;
using namespace std;

// --- Global Variables ---
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

vec3 cameraPosition(0.0f, 0.0f, 3.0f);
vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);

float cameraSpeed = 5.0f;
float cameraFastSpeed = 2 * cameraSpeed;
float cameraHorizontalAngle = 90.0f;
float cameraVerticalAngle = 0.0f;

float lastFrameTime = 0.0f;
double lastMousePosX, lastMousePosY;

// --- Function Prototypes ---
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

unsigned int compileShader(const char* shaderSource, GLenum type);
unsigned int createShaderProgram(const char* vertexSource, const char* fragmentSource);
unsigned int loadCubeMapTexture(std::vector<std::string> faces);
void renderSky(mat4 projection, mat4 view, unsigned int skyboxShaderProgram, unsigned int skyboxVAO, unsigned int cubemapTexture);

// Shaders
const char* skyboxVertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform mat4 projection;
    uniform mat4 view;

    out vec3 TexCoords;

    void main()
    {
        TexCoords = aPos;
        // Remove translation from the view matrix to keep skybox fixed
        vec4 pos = projection * view * vec4(aPos, 1.0);
        gl_Position = pos.xyww; // Force z to w to ensure it's always at max depth (background)
    }
)";

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


int main(int argc, char*argv[])
{
    glfwInit();

#if defined(__APPLE__)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Comp371 - Skybox Project", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glGetError(); // Clear any potential error from glewInit()

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    unsigned int skyboxShaderProgram = createShaderProgram(skyboxVertexShaderSource, skyboxFragmentShaderSource);
    glUseProgram(skyboxShaderProgram);
    glUniform1i(glGetUniformLocation(skyboxShaderProgram, "skybox"), 0);

    // --- Skybox Specific Setup ---
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
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
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
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Load cubemap textures
    std::vector<std::string> faces = {
        // Order: Right, Left, Top, Bottom, Front, Back
        "Textures/skybox/right.png",  // GL_TEXTURE_CUBE_MAP_POSITIVE_X
        "Textures/skybox/left.png",   // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
        "Textures/skybox/top.png",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
        "Textures/skybox/bottom.png", // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
        "Textures/skybox/front.png",  // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
        "Textures/skybox/back.png"    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };
    unsigned int cubemapTexture = loadCubeMapTexture(faces);

    // Get initial mouse position to avoid a jump when the program starts
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    while(!glfwWindowShouldClose(window))
    {
        float dt = static_cast<float>(glfwGetTime()) - lastFrameTime;
        lastFrameTime += dt;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- Camera Input Handling ---
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;

        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        const float cameraAngularSpeed = 5.0f; // degrees per second
        cameraHorizontalAngle -= static_cast<float>(dx) * cameraAngularSpeed * dt;
        cameraVerticalAngle   -= static_cast<float>(dy) * cameraAngularSpeed * dt;

        // Clamp vertical angle to [-85, 85] degrees to prevent camera flip
        cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));

        // Update cameraLookAt vector from angles (spherical coordinates)
        float theta = glm::radians(cameraHorizontalAngle);
        float phi = glm::radians(cameraVerticalAngle);

        cameraLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));
        // Calculate cameraSideVector for strafing
        vec3 cameraSideVector = glm::normalize(glm::cross(cameraLookAt, cameraUp)); // Normalize to ensure consistent speed

        bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;

        if (glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS)
        {
            cameraPosition += cameraLookAt * dt * currentCameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_S ) == GLFW_PRESS)
        {
            cameraPosition -= cameraLookAt * dt * currentCameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS)
        {
            cameraPosition += cameraSideVector * dt * currentCameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS)
        {
            cameraPosition -= cameraSideVector * dt * currentCameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // Move up
            cameraPosition += cameraUp * dt * currentCameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) // Move down
            cameraPosition -= cameraUp * dt * currentCameraSpeed;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // --- Render Loop ---
        mat4 projection = glm::perspective(glm::radians(70.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 1000.0f);
        mat4 view = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

        renderSky(projection, view, skyboxShaderProgram, skyboxVAO, cubemapTexture);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteProgram(skyboxShaderProgram);
    glDeleteTextures(1, &cubemapTexture);

    glfwTerminate();
    return 0;
}

// --- Function Implementations ---

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Utility function to compile a single shader
unsigned int compileShader(const char* shaderSource, GLenum type)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED of type " << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

// Utility function to create a shader program from vertex and fragment sources
unsigned int createShaderProgram(const char* vertexSource, const char* fragmentSource)
{
    unsigned int vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    if (vertexShader == 0 || fragmentShader == 0) {
        return 0;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// Utility function to load a cubemap texture
unsigned int loadCubeMapTexture(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false); // Important for cubemaps, do NOT flip!
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format = GL_RGB;
            if (nrChannels == 4) format = GL_RGBA;
            else if (nrChannels == 1) format = GL_RED;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    stbi_set_flip_vertically_on_load(true); // Reset for other 2D textures if you load them later!

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void renderSky(mat4 projection, mat4 view, unsigned int skyboxShaderProgram, unsigned int skyboxVAO, unsigned int cubemapTexture)
{
    glDepthFunc(GL_LEQUAL); // Change depth function so depth test passes when values are equal to depth buffer's content

    glUseProgram(skyboxShaderProgram);
    // Remove translation from the view matrix for the skybox.
    mat4 skyboxView = mat4(mat3(view));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(skyboxView));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS); // Set depth function back to default
}