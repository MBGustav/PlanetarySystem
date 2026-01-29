#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#include <CelestialObjectProperties.hpp>
#include <Physics.hpp>

// ================= CAMERA =================
glm::vec3 cameraPos(0, 0, 5);
glm::vec3 cameraFront(0, 0, -1);
glm::vec3 cameraUp(0, 1, 0);

// ================= SHADERS =================
const char* vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 color;
void main() {
    FragColor = vec4(color, 1.0);
}
)";

// ================= SPHERE GENERATOR =================
void generateSphere(float radius, int stacks, int sectors,
                    std::vector<float>& vertices,
                    std::vector<unsigned int>& indices)
{
    const float PI = 3.1415926f;

    for (int i = 0; i <= stacks; ++i) {
        float phi = PI * i / stacks;
        float y = cos(phi);
        float r = sin(phi);

        for (int j = 0; j <= sectors; ++j) {
            float theta = 2 * PI * j / sectors;
            vertices.push_back(radius * r * cos(theta));
            vertices.push_back(radius * y);
            vertices.push_back(radius * r * sin(theta));
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int first = i * (sectors + 1) + j;
            int second = first + sectors + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}

// ================= SHADER UTILS =================
GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    return s;
}

// ================= MAIN =================
int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800,600,"CelestialObject Simulation",nullptr,nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateSphere(1.0f, 32, 32, vertices, indices);

    GLuint VAO,VBO,EBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(float),vertices.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(unsigned int),indices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    // ================= PLANETS =================
    CelestialObjectProperties<float> p1({0,0,0},{0,0,0},{0,0,0},0.1f,0.01f,{1.0f, 0.8f, 0.2f},"P1");
    CelestialObjectProperties<float> p2({0,0,0},{0,0,0},{0,0,0},0.1f,0.01f,{0.4f, 0.8f, 0.8f},"P2");

    init_elliptical_orbit_visviva(p1,p2,2.0f,0.7f);

    std::vector<CelestialObjectProperties<float>> planets = {p1,p2};

    // ================= LOOP =================
    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.05,0.05,0.1,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        velocity_verlet(planets);

        glUseProgram(program);
        glm::mat4 view = glm::lookAt(cameraPos,cameraPos+cameraFront,cameraUp);
        glm::mat4 proj = glm::perspective(glm::radians(45.0f),800.f/600.f,0.1f,100.f);

        glUniformMatrix4fv(glGetUniformLocation(program,"view"),1,GL_FALSE,&view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(program,"projection"),1,GL_FALSE,&proj[0][0]);

        for(auto& p : planets) {
            glm::mat4 model = glm::translate(glm::mat4(1), p.get_position());
            model = glm::scale(model, glm::vec3(p.get_radius()));
            glUniformMatrix4fv(glGetUniformLocation(program,"model"),1,GL_FALSE,&model[0][0]);
            glUniform3fv(glGetUniformLocation(program,"color"),1,&p.get_color()[0]);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
