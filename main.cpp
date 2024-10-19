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
float cameraSpeed = 0.05f;
vec3 cameraPos = vec3(0.0f, 15.0f, 5.0f);

float timeOfDay = 0.0f; 
float dayDuration = 10.0f;
float nightDuration = 10.0f;

vec3 spherePosition = vec3(0.0f, 13.5f, 0.0f); 
float sphereRotationAngle = 0.0f;

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

const char* fragment_shader_source_solid = R"(
    #version 330 core
    out vec4 fragColor;
    void main() {
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
)";

const char* fragment_shader_source_texture = R"(
    #version 330 core
    out vec4 fragColor;
    in vec2 TexCoord;
    uniform sampler2D texture1;
    uniform vec3 lightColor;
    uniform vec3 lightPos; 
    void main() {
        vec4 texColor = texture(texture1, TexCoord);
        fragColor = texColor * vec4(lightColor, 1.0);
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
    ShapeRenderer(const vector<float>& vertices) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    ~ShapeRenderer() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }
    void render(Shader& shader, GLuint texture, int count) {
        shader.use();
        glBindTexture(GL_TEXTURE_2D, texture); 
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, count);
    }
private:
    GLuint vao, vbo;
};

class Sphere {
public:
    Sphere(float radius, int sectorCount, int stackCount) {
        createSphere(radius, sectorCount, stackCount);
    }

    void render(Shader& shader, GLuint texture) {
        shader.use();
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

    GLuint getVAO() const {
        return vao;
    }

    int getVertexCount() const {
        return vertexCount;
    }

private:
    GLuint vao;
    int vertexCount;

    void createSphere(float radius, int sectorCount, int stackCount) {
        vector<float> vertices;
        for (int i = 0; i <= stackCount; ++i) {
            float stackAngle = M_PI / 2 - i * M_PI / stackCount;
            float xy = radius * cosf(stackAngle);
            float z = radius * sinf(stackAngle);
            for (int j = 0; j <= sectorCount; ++j) {
                float sectorAngle = j * 2 * M_PI / sectorCount;
                float x = xy * cosf(sectorAngle);
                float y = xy * sinf(sectorAngle);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
                float s = (float)j / sectorCount;
                float t = (float)i / stackCount;
                vertices.push_back(s);
                vertices.push_back(t);
            }
        }

        vertexCount = (sectorCount + 1) * (stackCount + 1);
        glGenVertexArrays(1, &vao);
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

vector<float> generateCubeVertices() {
    return {
        -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
         1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
         1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
    };
}

vector<float> generatePlaneVertices() {
    return {
        -50.0f, 0.0f, -50.0f, 0.0f, 0.0f,
         50.0f, 0.0f, -50.0f, 1.0f, 0.0f,
         50.0f, 0.0f,  50.0f, 1.0f, 1.0f,
         50.0f, 0.0f,  50.0f, 1.0f, 1.0f,
        -50.0f, 0.0f,  50.0f, 0.0f, 1.0f,
        -50.0f, 0.0f, -50.0f, 0.0f, 0.0f,
    };
}

vector<float> generateSecondPlaneVertices() {
    return {
        -10.0f, 13.0f, -10.0f, 0.0f, 0.0f,
         10.0f, 13.0f, -10.0f, 1.0f, 0.0f,
         10.0f, 13.0f,  10.0f, 1.0f, 1.0f,
         10.0f, 13.0f,  10.0f, 1.0f, 1.0f,
        -10.0f, 13.0f,  10.0f, 0.0f, 1.0f,
        -10.0f, 13.0f, -10.0f, 0.0f, 0.0f,
    };
}

vector<float> generateWallVertices() {
    return {
        -50.0f, -1.0f, -50.0f, 0.0f, 1.0f,
         50.0f, -1.0f, -50.0f, 1.0f, 1.0f,
         50.0f,  50.0f, -50.0f, 1.0f, 0.0f,
         50.0f,  50.0f, -50.0f, 1.0f, 0.0f,
        -50.0f,  50.0f, -50.0f, 0.0f, 0.0f,
        -50.0f, -1.0f, -50.0f, 0.0f, 1.0f,
        
        -50.0f, -1.0f, 50.0f, 1.0f, 1.0f, 
         50.0f, -1.0f, 50.0f, 0.0f, 1.0f, 
         50.0f,  50.0f, 50.0f, 0.0f, 0.0f,
         50.0f,  50.0f, 50.0f, 0.0f, 0.0f,
        -50.0f,  50.0f, 50.0f, 1.0f, 0.0f,
        -50.0f, -1.0f, 50.0f, 1.0f, 1.0f, 
        
        -50.0f, -1.0f, -50.0f, 0.0f, 1.0f,
        -50.0f, -1.0f, 50.0f, 1.0f, 1.0f,
        -50.0f,  50.0f, 50.0f, 1.0f, 0.0f,
        -50.0f,  50.0f, 50.0f, 1.0f, 0.0f,
        -50.0f,  50.0f, -50.0f, 0.0f, 0.0f,
        -50.0f, -1.0f, -50.0f, 0.0f, 1.0f,
        
        50.0f, -1.0f, -50.0f, 1.0f, 1.0f, 
        50.0f, -1.0f, 50.0f, 0.0f, 1.0f,
        50.0f,  50.0f, 50.0f,  0.0f, 0.0f,
        50.0f,  50.0f, 50.0f,  0.0f, 0.0f,
        50.0f,  50.0f, -50.0f, 1.0f, 0.0f,
        50.0f, -1.0f, -50.0f, 1.0f, 1.0f, 
    };
}

vector<float> generateCeilingVertices() {
    return {
        -50.0f, 50.0f, -50.0f, 0.0f, 0.0f,
         50.0f, 50.0f, -50.0f, 1.0f, 0.0f,
         50.0f, 50.0f,  50.0f, 1.0f, 1.0f,
         50.0f, 50.0f,  50.0f, 1.0f, 1.0f,
        -50.0f, 50.0f,  50.0f, 0.0f, 1.0f,
        -50.0f, 50.0f, -50.0f, 0.0f, 0.0f,
    };
}

GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1) {
            format = GL_RED;
        } else if (nrChannels == 3) {
            format = GL_RGB;
        } else if (nrChannels == 4) {
            format = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        cerr << "error with download texture: " << path << endl;
        cerr << "stbi_load err: " << stbi_failure_reason() << endl;
        return 0;
    }

    stbi_image_free(data); 
    return textureID;
}

vector<float> generatePyramidVertices() {
    return {
        -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, -1.0f, 1.0f, 1.0f,
         1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
         1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
        -1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
        -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
         0.0f, 3.0f, 0.0f, 0.5f, 1.0f,
         1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
         1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
         0.0f, 3.0f, 0.0f, 0.5f, 1.0f,
         1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
        -1.0f, 0.0f,  1.0f, 0.0f, 1.0f,
         0.0f, 3.0f, 0.0f, 0.5f, 1.0f,
        -1.0f, 0.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         0.0f, 3.0f, 0.0f, 0.5f, 1.0f 
    };
}

void processInput(GLFWwindow *window) {
    vec3 front;
    front.x = cos(radians(zalfa)) * cos(radians(alfa));
    front.y = sin(radians(alfa));
    front.z = sin(radians(zalfa)) * cos(radians(alfa));
    front = normalize(front);
    vec3 right = normalize(cross(front, vec3(0.0f, 1.0f, 0.0f))); 
    vec3 up = normalize(cross(right, front)); 

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += front * cameraSpeed; 
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= front * cameraSpeed; 
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= right * cameraSpeed; 
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += right * cameraSpeed; 

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        spherePosition += front * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        spherePosition -= front * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        spherePosition -= right * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        spherePosition += right * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        sphereRotationAngle += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        sphereRotationAngle -= 1.0f;
}

int main() {
    try {
        if (!glfwInit()) {
            throw runtime_error("GLFW error");
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "3D-Scene", monitor, nullptr);

        if (!window) {
            glfwTerminate();
            throw runtime_error("Window create error");
        }
        glfwMakeContextCurrent(window);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetKeyCallback(window, key_callback);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (glewInit() != GLEW_OK) {
            throw runtime_error("Glew error");
        }

        float radius = 1.5f;
        int sectorCount = 36;
        int stackCount = 1000;
        vector<float> sphereVertices;
        Sphere sphere(radius, sectorCount, stackCount); 

        Shader shaderSolid(vertex_shader_source, fragment_shader_source_solid);
        Shader shaderTexture(vertex_shader_source, fragment_shader_source_texture);

        GLuint transformLoc = glGetUniformLocation(shaderSolid.getProgram(), "transform");

        GLuint textureSphere = loadTexture("texture/sphere.jpg");
        GLuint textureSquare = loadTexture("texture/cube.jpg");
        GLuint texturePyramide = loadTexture("texture/pyramid.jpg");
        GLuint floorTexture = loadTexture("texture/second_floor.jpg"); 
        GLuint wallTexture = loadTexture("texture/wall.jpg");
        GLuint topTexture = loadTexture("texture/floor+ceiling.jpg");

        vector<float> planeVertices = generatePlaneVertices();
        ShapeRenderer planeRenderer(planeVertices);
        ShapeRenderer pyramidRenderer(generatePyramidVertices());

        vector<float> cubeVertices = generateCubeVertices();
        ShapeRenderer cubeRenderer(cubeVertices);

        vector<float> wallVertices = generateWallVertices();
        ShapeRenderer wallRenderer(wallVertices);

        vector<float> ceilingVertices = generateCeilingVertices();
        ShapeRenderer ceilingRenderer(ceilingVertices);

        vector<float> secondFloorVertices = generateSecondPlaneVertices();
        ShapeRenderer secondFloorRenderer(secondFloorVertices);

        while (!glfwWindowShouldClose(window)) {
            timeOfDay += (1.0f / 60.0f);
            if (timeOfDay > (dayDuration + nightDuration)) {
                timeOfDay = 0.0f;
            }

            vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
            vec3 lightPos = vec3(5.0f * cos(timeOfDay * 2.0f * M_PI / (dayDuration + nightDuration)), 
                                5.0f * sin(timeOfDay * 2.0f * M_PI / (dayDuration + nightDuration)), 
                                0.0f);
            if (timeOfDay < dayDuration) {
                float intensity = timeOfDay / dayDuration; 
                lightColor *= intensity; 
            } else {
                float intensity = 1.0f - ((timeOfDay - dayDuration) / nightDuration);
                lightColor *= intensity; 
            }

            glEnable(GL_DEPTH_TEST);
            processInput(window);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mat4 model = mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(shaderTexture.getProgram(), "transform"), 1, GL_FALSE, value_ptr(model));
            glUniform3fv(glGetUniformLocation(shaderTexture.getProgram(), "lightColor"), 1, value_ptr(lightColor));
            glUniform3fv(glGetUniformLocation(shaderTexture.getProgram(), "lightPos"), 1, value_ptr(lightPos));
            planeRenderer.render(shaderTexture, floorTexture, planeVertices.size() / 5);

            mat4 transformMatrix = mat4(1.0f);
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            mat4 projection = perspective(radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
            model = translate(transformMatrix, vec3(0, 0, 0));
            mat4 view;
            vec3 front; 
            front.x = cos(radians(zalfa)) * cos(radians(alfa));
            front.y = sin(radians(alfa));
            front.z = sin(radians(zalfa)) * cos(radians(alfa));
            front = normalize(front);
            view = lookAt(cameraPos, cameraPos + front, vec3(0, 1, 0));
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(projection * view * model));

            mat4 floorModel = translate(mat4(1.0f), vec3(0.0f, -1.0f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(shaderTexture.getProgram(), "transform"), 1, GL_FALSE, value_ptr(projection * view * floorModel));
            planeRenderer.render(shaderTexture, topTexture, planeVertices.size() / 5);

            mat4 secondfloorModel =mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(shaderTexture.getProgram(), "transform"), 1, GL_FALSE, value_ptr(projection * view * floorModel));
            secondFloorRenderer.render(shaderTexture, floorTexture, planeVertices.size() / 5);
            
            mat4 sphereModel = translate(mat4(1.0f), spherePosition) * rotate(mat4(1.0f), radians(sphereRotationAngle), vec3(0.0f, 1.0f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(shaderTexture.getProgram(), "transform"), 1, GL_FALSE, value_ptr(projection * view * sphereModel));
            sphere.render(shaderTexture, textureSphere);

            mat4 cubeModel = translate(mat4(1.0f), vec3(5, 13.2, 0)); 
            glUniformMatrix4fv(glGetUniformLocation(shaderTexture.getProgram(), "transform"), 1, GL_FALSE, value_ptr(projection * view * cubeModel));
            cubeRenderer.render(shaderTexture, textureSquare, cubeVertices.size() / 5);

            mat4 pyramidModel = translate(mat4(1.0f), vec3(-5.0f, 12.2f, 0.0f)) * rotate(mat4(1.0f), radians(180.0f), vec3(0, 1, 0));
            glUniformMatrix4fv(glGetUniformLocation(shaderTexture.getProgram(), "transform"), 1, GL_FALSE, value_ptr(projection * view * pyramidModel));
            pyramidRenderer.render(shaderTexture, texturePyramide, 18);

            mat4 wallModel = mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(shaderTexture.getProgram(), "transform"), 1, GL_FALSE, value_ptr(projection * view * wallModel));
            wallRenderer.render(shaderTexture, wallTexture, wallVertices.size() / 5);
            
            mat4 ceilingModel = mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(shaderTexture.getProgram(), "transform"), 1, GL_FALSE, value_ptr(projection * view * ceilingModel));
            ceilingRenderer.render(shaderTexture, topTexture, ceilingVertices.size() / 5);

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