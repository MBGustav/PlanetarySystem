#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

namespace graphics {
    
    class GraphicsWrapper {
        private:
        // --- OpenGL objects ---
        GLuint shaderProgram = 0;
        GLuint VBO = 0, VAO = 0;
        GLuint framebuffer = 0;
        GLuint mainVAO, mainVBO;
        GLuint figureVAO, figureVBO;
        GLuint framebufferTexture = 0;
        GLuint rbo = 0; // depth/stencil renderbuffer
        
        // --- Viewport ---
        float width, height;
        
        // --- Camera Properties ---
        glm::vec3 cameraPos   = {0.0f, 0.0f, 5.0f};
        glm::vec3 cameraFront = {0.0f, 0.0f, 0.0f};
        glm::vec3 cameraUp    = {0.0f, 1.0f, 0.0f};
        
        // --- Cube vertices ---
        inline static float vertices[108] = {
            -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,0.5f,-0.5f,  0.5f,0.5f,-0.5f, -0.5f,0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
            -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f,0.5f, 0.5f,  0.5f,0.5f, 0.5f, -0.5f,0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
            -0.5f,0.5f, 0.5f,  -0.5f,0.5f,-0.5f,  -0.5f,-0.5f,-0.5f,  -0.5f,-0.5f,-0.5f, -0.5f,-0.5f,0.5f, -0.5f,0.5f,0.5f,
            0.5f,0.5f, 0.5f,   0.5f,0.5f,-0.5f,   0.5f,-0.5f,-0.5f,   0.5f,-0.5f,-0.5f,  0.5f,-0.5f,0.5f,  0.5f,0.5f,0.5f,
            -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f,0.5f,  0.5f,-0.5f,0.5f, -0.5f,-0.5f,0.5f, -0.5f,-0.5f,-0.5f,
            -0.5f,0.5f,-0.5f,   0.5f,0.5f,-0.5f,   0.5f,0.5f,0.5f,   0.5f,0.5f,0.5f,  -0.5f,0.5f,0.5f,  -0.5f,0.5f,-0.5f
        };
        
        public:
        void createVAO()
        {
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }
        
        
        GLuint compileShader(GLenum type, const char* src) {
            GLuint shader = glCreateShader(type);
            glShaderSource(shader, 1, &src, nullptr);
            glCompileShader(shader);
            
            int success;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(shader, 512, nullptr, infoLog);
                std::cerr << "Shader compile error: " << infoLog << std::endl;
            }
            return shader;
        }
        
        void SetupShader() {
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
            
            GLuint vShader = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
            GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
            
            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vShader);
            glAttachShader(shaderProgram, fShader);
            glLinkProgram(shaderProgram);
            
            glDeleteShader(vShader);
            glDeleteShader(fShader);
        }
        GraphicsWrapper(float w = 800.0f, float h = 600.0f)
        : width(w), height(h) {}
        
        ~GraphicsWrapper() {
            glDeleteProgram(shaderProgram);
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            if (framebuffer) glDeleteFramebuffers(1, &framebuffer);
            if (framebufferTexture) glDeleteTextures(1, &framebufferTexture);
            if (rbo) glDeleteRenderbuffers(1, &rbo);
        }
        
        GLuint GetFramebufferTexture() const { return framebufferTexture; }
        
        
        
        void SetupBuffers() {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            
        }
        
        void SetupFramebuffer(ImVec2 size) {
            width = size.x;
            height = size.y;
            
            
            if (framebuffer) {
                glDeleteFramebuffers(1, &framebuffer);
                glDeleteTextures(1, &framebufferTexture);
                glDeleteRenderbuffers(1, &rbo);
            }
            
            glGenFramebuffers(1, &framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            
            // Color texture
            glGenTextures(1, &framebufferTexture);
            glBindTexture(GL_TEXTURE_2D, framebufferTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);
            
            // Depth/stencil
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
            
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "[ERROR] Framebuffer not complete!" << std::endl;
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);  // allow resizing (default)
        }
        
        void RenderScene(float viewportWidth, float viewportHeight) {
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            
            glm::mat4 view = glm::lookAt(cameraPos, // camera position
                cameraFront, // look at cube
                cameraUp); // up vector
                
                // glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
                glm::mat4 projection = glm::perspective(glm::radians(45.0f), viewportWidth / viewportHeight, 0.1f, 100.0f);
                glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() ,
                glm::vec3(0.5f, 1.0f, 0.0f));
                
                
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
                
                glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 0.7f, 1.0f);
                
                glDrawArrays(GL_TRIANGLES, 0, 36);
                
                glBindVertexArray(0);
                glUseProgram(0);
                
                
            }
            
            void RenderToFramebuffer() {
                glEnable(GL_DEPTH_TEST);
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
                glViewport(0, 0, (int)width, (int)height);
                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                RenderScene(width, height);
                
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            
            float GetWidth() const { return width; }
            float GetHeight() const { return height; }
        };
        
    } // namespace graphics
    