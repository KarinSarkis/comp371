#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Island.hpp"
#include "Skybox.hpp"
#include "Camera.hpp"
#include "Windmill.hpp"

#define GL_CHECK_ERROR() \
    do { \
        GLenum err; \
        while ((err = glGetError()) != GL_NO_ERROR) { \
            std::cerr << "OpenGL Error " << err << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        } \
    } while (0)

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open()){
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }else{
        return 0;
    }

    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    }

    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    }

    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}


Camera camera(glm::vec3(-626.257f, 40.885f, -605.891f), glm::vec3(0.0f, 1.0f, 0.0f), -274.58f, 8.27f);
float lastX = 1280.0f / 2.0f;
float lastY = 720.0f / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Windmill windmill;
GLuint windmillShaderProgram;


static void glfw_error_cb(int code, const char* desc) {
    std::fprintf(stderr, "GLFW error %d: %s\n", code, desc ? desc : "(null)");
}

static void framebuffer_size_cb(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float currentCameraSpeed = camera.MovementSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
    {
        camera.MovementSpeed *= 3.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    camera.MovementSpeed = currentCameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


int main() {
    glfwSetErrorCallback(glfw_error_cb);
    if (!glfwInit()) {
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Island Demo", nullptr, nullptr);
    if (!window) {
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_cb);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return 1;
    }
    glGetError();

    glEnable(GL_DEPTH_TEST);
    GL_CHECK_ERROR();
    glCullFace(GL_BACK);
    GL_CHECK_ERROR();
    glFrontFace(GL_CCW);
    GL_CHECK_ERROR();

    windmillShaderProgram = LoadShaders("shaders/SimpleColor.vert", "shaders/SimpleColor.frag");
    if (windmillShaderProgram == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    windmill.setup(windmillShaderProgram);

    Island island("assets/heightmap.png", /*heightScale=*/350.0f, /*gridScale=*/1.5f, /*center=*/true, /*sampleStep=*/1);
    GL_CHECK_ERROR();

    if (!island.isValid()) {
        return 1;
    }

    if (!island.setTextures("assets/sand.png",
                            "assets/grass.png",
                            "assets/rock.png")) {
        return 1;
    }
    GL_CHECK_ERROR();

    island.setBlendParams(/*seaLevel=*/0.0f, /*sandTop=*/30.0f, /*grassTop=*/100.0f, /*slopeRockStart=*/0.50f);
    island.setTiling(4.0f, 6.0f, 8.0f);

    SunLight sun;
    sun.direction = glm::normalize(glm::vec3(-0.7f, -1.0f, -0.2f));
    sun.color = glm::vec3(1.0f);
    sun.intensity = 1.0f;
    island.setSun(sun);

    Skybox skybox;
    GL_CHECK_ERROR();

    std::vector<std::string> skyboxFaces = {
        "assets/right.png",
        "assets/left.png",
        "assets/top.png",
        "assets/bottom.png",
        "assets/front.png",
        "assets/back.png"
    };

    if (!skybox.load(skyboxFaces)) {
        return -1;
    }
    GL_CHECK_ERROR();


    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glfwPollEvents();
        GL_CHECK_ERROR();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        GL_CHECK_ERROR();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_CHECK_ERROR();

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)w / (float)h, 0.1f, 4000.0f);

        skybox.draw(view, proj);
        GL_CHECK_ERROR();

        island.draw(view, proj, camera.Position);
        GL_CHECK_ERROR();

        windmill.draw(view, proj, currentFrame);
        GL_CHECK_ERROR();

        glfwSwapBuffers(window);
        GL_CHECK_ERROR();
    }

    glDeleteProgram(windmillShaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}