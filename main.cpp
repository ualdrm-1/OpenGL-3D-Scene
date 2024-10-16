#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace glm;

float alfa = 0;
float zalfa = 0;
double lastX = 250.0;
double lastY = 250.0;
bool firstMouse = true;

const char* vertex_shader_source = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    uniform mat4 transform;
    void main() {
        gl_Position = transform * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }
)";

const char* fragment_shader_source = R"(
    #version 330 core
    out vec4 fragColor;
    in vec2 TexCoord;
    uniform sampler2D texture1;
    void main() {
        fragColor = texture(texture1, TexCoord);
    }
)";

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (firstMouse) {
        lastX = xposIn;
        lastY = yposIn;
        firstMouse = false;
    }
    double xoffset = xposIn - lastX;
    double yoffset = lastY - yposIn;
    lastX = xposIn;
    lastY = yposIn;
    float sensitivity = 0.1;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    zalfa -= xoffset; 
    alfa -= yoffset; 
    if (alfa > 89.0f) alfa = 89.0f; 
    if (alfa < -89.0f) alfa = -89.0f;
}

class Shader {
public:
    Shader(const char* vertexSource, const char* fragmentSource) {
        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
        program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        checkProgram(program);
    }
    
    ~Shader() {
        glDeleteProgram(program);
    }

    void use() {
        glUseProgram(program);
    }

    GLuint getProgram() const {
        return program;
    }

private:
    GLuint program;

    GLuint compileShader(GLenum type, const char* src) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        checkShader(shader);
        return shader;
    }

    void checkShader(GLuint shader) {
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLint logLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            vector<char> log(logLength);
            glGetShaderInfoLog(shader, logLength, nullptr, log.data());
            cerr << "Shader compile error: " << log.data() << endl;
        }
    }

    void checkProgram(GLuint program) {
        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            GLint logLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            vector<char> log(logLength);
            glGetProgramInfoLog(program, logLength, nullptr, log.data());
            cerr << "Program link error: " << log.data() << endl;
        }
    }
};

class ShapeRenderer {
public:
    ShapeRenderer(const vector<float>& vertices, const vector<float>& texCoords) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &tbo);
        glBindVertexArray(vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), texCoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~ShapeRenderer() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &tbo);
    }

    void render(Shader& shader, GLuint texture, int count) {
        shader.use();
        glBindTexture(GL_TEXTURE_2D, texture); 
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, count);
    }

private:
    GLuint vao, vbo, tbo;
};

GLuint loadTexture() {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("/home/ualdrm/Studies/KR/eagle.jpeg", &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 1) ? GL_RED : (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        cerr << "Failed to load texture: " << endl;
        cerr << "stbi_load error: " << stbi_failure_reason() << endl;
        return 0;
    }
    stbi_image_free(data);
    return textureID;
}

int main() {
    try {
        if (!glfwInit()) {
            throw runtime_error("GLFW error");
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        GLFWwindow* window = glfwCreateWindow(500, 500, "GRAPH-3", nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            throw runtime_error("Window create error");
        }
        glfwMakeContextCurrent(window);
        if (glewInit() != GLEW_OK) {
            throw runtime_error("Glew error");
        }
        
        glfwSetCursorPosCallback(window, mouse_callback);

        vector<float> floorVertices = {
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f
        };
        vector<float> floorTexCoords = {
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f
        };

        ShapeRenderer floorRenderer(floorVertices, floorTexCoords);
        Shader shader(vertex_shader_source, fragment_shader_source);
        GLuint transformLoc = glGetUniformLocation(shader.getProgram(), "transform");
        GLuint texture = loadTexture();

        if (texture == 0) {
            throw runtime_error("Failed to load texture");
        }

        while (!glfwWindowShouldClose(window)) {
            glEnable(GL_DEPTH_TEST);
            glfwSetKeyCallback(window, key_callback);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mat4 transformMatrix = mat4(1.0f);
            mat4 projection = perspective(radians(45.0f), 500.0f / 500.0f, 0.1f, 100.0f);
            mat4 model = translate(transformMatrix, vec3(0, 0, 0));
            mat4 view = lookAt(vec3(0, 0, 3), vec3(0, 0, 0), vec3(0, 1, 0));
            view = rotate(view, radians(alfa), vec3(1, 0, 0));
            view = rotate(view, radians(zalfa), vec3(0, 1, 0));

            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(projection * view * model));
            //floorRenderer.render(shader, texture, floorVertices.size() / 3);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        glfwDestroyWindow(window);
        glfwTerminate();
    } catch (const runtime_error& e) {
        cerr << "Runtime error: " << e.what() << endl;
        return -1;
    }
    return 0;
}