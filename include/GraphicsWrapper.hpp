
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "PlanetProperties.hpp"
#include <vector>

typedef struct ViewPortRequest {
    bool valid = false;
    float x, y, width, height;
} ViewportRequest;

namespace graphics{
    
    
    
    float vertices[] = {
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,0.5f,-0.5f,  0.5f,0.5f,-0.5f, -0.5f,0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f,0.5f, 0.5f,  0.5f,0.5f, 0.5f, -0.5f,0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
        -0.5f,0.5f, 0.5f,  -0.5f,0.5f,-0.5f,  -0.5f,-0.5f,-0.5f,  -0.5f,-0.5f,-0.5f, -0.5f,-0.5f,0.5f, -0.5f,0.5f,0.5f,
        0.5f,0.5f, 0.5f,   0.5f,0.5f,-0.5f,   0.5f,-0.5f,-0.5f,   0.5f,-0.5f,-0.5f,  0.5f,-0.5f,0.5f,  0.5f,0.5f,0.5f,
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f,0.5f,  0.5f,-0.5f,0.5f, -0.5f,-0.5f,0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f,0.5f,-0.5f,   0.5f,0.5f,-0.5f,   0.5f,0.5f,0.5f,   0.5f,0.5f,0.5f,  -0.5f,0.5f,0.5f,  -0.5f,0.5f,-0.5f
    };
    
    // === Helper: compile shader ===
    
    
    
    class GraphicsWrapper
    {
        private:
        GLuint vShader;
        GLuint fShader;
        GLuint shaderProgram;
        GLuint framebuffer = 0;
        
        GLuint rbo = 0; // Renderbuffer para depth/stencil
        GLuint VBO, VAO, figureVAO;
        float width, height;
        GLuint framebufferTexture;
        // GLFWwindow* figureWindow;
        
        
        GLuint GetFramebufferTexture() const { return framebufferTexture; }
        
        // === Camera variables ===
        glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 5.0f);
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
        
        public:
        void SetupFramebuffer(int w, int h);
        void setup();
        void render();
        void RenderScene(float viewportWidth, float viewportHeight);
        
        float get_width() {return this->width;};
        float get_height(){return this->height;};
        
        
        GraphicsWrapper(/* args */);
        
        GLuint compileShader(GLenum type, const char* src) {
            GLuint shader = glCreateShader(type);
            glShaderSource(shader, 1, &src, NULL);
            glCompileShader(shader);
            int success;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(shader, 512, NULL, infoLog);
                std::cerr << "Shader compile error: " << infoLog << std::endl;
            }
            return shader;
        }
        
        ~GraphicsWrapper();
    };
    
    void GraphicsWrapper::SetupFramebuffer(int w, int h)
    {
        if (framebuffer != 0) {
            glDeleteFramebuffers(1, &framebuffer);
            glDeleteTextures(1, &framebufferTexture);
            glDeleteRenderbuffers(1, &rbo);
        }
        
        // Cria framebuffer
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        
        // Cria textura para color attachment
        glGenTextures(1, &framebufferTexture);
        glBindTexture(GL_TEXTURE_2D, framebufferTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);
        
        // Renderbuffer para depth/stencil
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "[ERROR] Framebuffer is not complete!" << std::endl;
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void GraphicsWrapper::RenderScene(float viewportWidth, float viewportHeight)
    {
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        
        // ----------- VIEW MATRIX (camera) -------------------
        glm::mat4 view = glm::lookAt(
            cameraPos,
            cameraPos + cameraFront,
            cameraUp
        );
        
        // ----------- PROJECTION MATRIX ----------------------
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            viewportWidth / viewportHeight,
            0.1f,
            100.0f
        );
        
        // send matrices to shader
        GLint modelLoc      = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc       = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc       = glGetUniformLocation(shaderProgram, "projection");
        GLint colorLoc      = glGetUniformLocation(shaderProgram, "color");
        
        glUniformMatrix4fv(viewLoc,  1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projLoc,  1, GL_FALSE, &projection[0][0]);
        
        // ----------- RENDER OBJETO --------------------------
        float time = glfwGetTime();
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, time, glm::vec3(0.5f, 1.0f, 0.0f));
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        
        // cor do cubo
        glUniform3f(colorLoc, 0.0f, 0.7f, 1.0f);
        
        // desenha 36 vertices (6 faces * 2 triângulos * 3 vertices)
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    
    void GraphicsWrapper::render()
    {
        glEnable(GL_DEPTH_TEST);
        
        // Bind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, (int)width, (int)height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Renderiza cena
        this->RenderScene(width, height);
        
        // Unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
    }
    
    void GraphicsWrapper::setup()
    {
        
        // === Simple shader sources ===
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
        
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        
        vShader = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
        fShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vShader);
        glAttachShader(shaderProgram, fShader);
        glLinkProgram(shaderProgram);
        glDeleteShader(vShader);
        glDeleteShader(fShader);
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
    
    GraphicsWrapper::GraphicsWrapper(/* args */)
    {
    }
    
    GraphicsWrapper::~GraphicsWrapper()
    {
    }
    
}