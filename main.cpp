// main.cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static void checkCompileErrors(GLuint shader, const std::string &name)
{
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[10240];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << name << "\n" 
                  << infoLog << "\n";
    }
}

static void checkLinkErrors(GLuint prog)
{
    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[10240];
        glGetProgramInfoLog(prog, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n";
    }
}

std::string readFile(const char* path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        exit(1);
    }
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

GLuint compileShader(const char* path, GLenum type) {
    std::string src = readFile(path);
    const char* cstr = src.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &cstr, nullptr);
    glCompileShader(shader);
    checkCompileErrors(shader, path);
    return shader;
}

GLuint createProgram(const char* vertPath, const char* geomPath, const char* fragPath) {
    GLuint vert = compileShader(vertPath, GL_VERTEX_SHADER);
    GLuint geom = compileShader(geomPath, GL_GEOMETRY_SHADER);
    GLuint frag = compileShader(fragPath, GL_FRAGMENT_SHADER);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, geom);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    checkLinkErrors(prog);

    glDeleteShader(vert);
    glDeleteShader(geom);
    glDeleteShader(frag);
    return prog;
}

// Globals for input handling
int depth = 3;
bool depthChanged = true;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);
        if (key == GLFW_KEY_UP || key == GLFW_KEY_KP_ADD) {
            if (depth < 6) { depth++; depthChanged = true; }
        }
        if (key == GLFW_KEY_DOWN || key == GLFW_KEY_KP_SUBTRACT) {
            if (depth > 0) { depth--; depthChanged = true; }
        }
        if (key == GLFW_KEY_R) {
            depth = 3; depthChanged = true;
        }
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return -1;
    }

    // Request OpenGL 4.1 core (macOS supports up to 4.1)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(900, 700, "Sierpinski Pyramid (Geometry Shader)", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    glewExperimental = GL_TRUE;
    GLenum glewInitResult = glewInit();
    if (glewInitResult != GLEW_OK) {
        std::cerr << "GLEW init failed: " << glewGetErrorString(glewInitResult) << std::endl;
        return -1;
    }

    // Create program
    GLuint program = createProgram("sierpinski.vert", "sierpinski.geom", "sierpinski.frag");

    // Single point VBO
    float point[3] = {0.0f, 0.0f, 0.0f};
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    glBindVertexArray(0);

    // Uniform locations
    glUseProgram(program);
    GLint loc_uMVP = glGetUniformLocation(program, "uMVP");
    GLint loc_uDepth = glGetUniformLocation(program, "uDepth");
    GLint loc_uScale = glGetUniformLocation(program, "uScale");
    GLint loc_uColor = glGetUniformLocation(program, "uColor");

    // initial uniforms
    glUniform1i(loc_uDepth, depth);
    glUniform1f(loc_uScale, 0.8f);
    glUniform3f(loc_uColor, 0.9f, 0.5f, 0.1f); // orange-ish

    glEnable(GL_DEPTH_TEST);

    float angle = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        // update uniforms if depth changed
        if (depthChanged) {
            glUseProgram(program);
            glUniform1i(loc_uDepth, depth);
            std::cout << "Depth set to " << depth << std::endl;
            depthChanged = false;
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = width / (float)height;
        glViewport(0,0,width,height);
        glClearColor(0.05f, 0.06f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // build MVP
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 50.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(2.5f,2.0f,2.5f),
                                     glm::vec3(0.0f,0.0f,0.0f),
                                     glm::vec3(0.0f,1.0f,0.0f));
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.2f, 1.0f, 0.0f));
        glm::mat4 mvp = proj * view * model;

        glUseProgram(program);
        glUniformMatrix4fv(loc_uMVP, 1, GL_FALSE, &mvp[0][0]);

        // optional: change scale/color with depth (visual)
        float scale = 0.9f;
        glUniform1f(loc_uScale, scale);
        // color vary with depth
        float t = depth / 6.0f;
        glUniform3f(loc_uColor, 0.2f + 0.8f * (1.0f - t), 0.2f + 0.6f * t, 0.1f + 0.4f * (1.0f - t));

        // render the single point that triggers the geometry shader
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, 1);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // animate rotation
        angle += 0.003f;
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);

    glfwTerminate();
    return 0;
}
