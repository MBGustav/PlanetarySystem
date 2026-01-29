#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "PlanetProperties.hpp"
#include "Planet.hpp"
#include "imgui.h"
#include "Logger.hpp"

namespace graphics {

    static const float CAMERA_MIN_DISTANCE = 1.0f;
    static const float CAMERA_MAX_DISTANCE = 500.0f;

    struct Camera {
        
        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 15.0f);
        glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 Target   = glm::vec3(0.0f); // Ponto central do gimbal
        
        // Euler Angles
        float Yaw = -90.0f;
        float Pitch = 0.0f;
        
        // Opções de lente
        float Distance = 15.0f;
        float Fov = 45.0f;
        
        float VisualScale = 1.0f;
        
        
        void updateVectors() {
            // Calcula posição usando coordenadas esféricas
            Position.x = Target.x + Distance * cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
            Position.y = Target.y + Distance * sin(glm::radians(Pitch));
            Position.z = Target.z + Distance * cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
            
            // Front aponta sempre para o centro
            Front = glm::normalize(Target - Position);
            
            // Right e Up
            glm::vec3 Right = glm::normalize(glm::cross(Front, WorldUp));
            Up = glm::normalize(glm::cross(Right, Front));
        }
        
        glm::vec3 scalePos(const glm::vec3& p) const {
            return p * VisualScale;
        }
        
        void rotate(float dx, float dy) {
            const float sensitivity = 0.3f;
            Yaw   += dx * sensitivity;
            Pitch += dy * sensitivity;
            
            // Limita Pitch para não virar de cabeça para baixo
            if (Pitch > 89.0f)  Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
            
            updateVectors();
        }
        
        void zoom(float delta) {
            Distance -= delta;
            if (Distance < CAMERA_MIN_DISTANCE) Distance = CAMERA_MIN_DISTANCE;
            if (Distance > CAMERA_MAX_DISTANCE) Distance = CAMERA_MAX_DISTANCE;
            updateVectors();
        }
        
        float scaleSize(float r) const {
            return r * VisualScale;
        }

        void ComputeVisualScale(const std::vector<PlanetProperties<float>>& planets) {
            float maxDist = 0.0f;
            for (const auto& p : planets) {
                maxDist = std::max(maxDist, glm::length(p.get_position()));
            }
            
            if (maxDist <= 0.0f) VisualScale = 1.0f;
            else VisualScale = 50.0f / maxDist;
        }
        

        // Matrix view for shader
        glm::mat4 getViewMatrix() const {
            return glm::lookAt(Position, Target, Up);
        }
    };
    
    
    
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
        // glm::vec3 cameraPos   = {0.0f, 0.0f, 0.0f};
        // glm::vec3 cameraFront = {10.0f, 0.0f, 0.0f};
        // glm::vec3 cameraUp    = {5.0f, 5.0f, 0.0f};
        float pos[3] = {0.0f, 0.0f, 3.0f};
        float yaw = -90.0f;
        float pitch = 0.0f;
        float fov = 45.0f;
        float front[3] = {0.0f, 0.0f, -1.0f}; // Vetor calculado
        
        
        
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
        
        
        Camera camera;
        
        // void set_cameraPos(glm::vec3 pos)   { cameraPos = pos; }
        // void set_cameraFront(glm::vec3 pos) { cameraFront = pos; }
        // void set_cameraUp(glm::vec3 pos)    { cameraUp = pos; }
        
        
        // void set_cameraPos(float x, float y,float z)   { cameraPos   = {x,y,z}; }
        // void set_cameraFront(float x, float y,float z) { cameraFront = {x,y,z}; }
        // void set_cameraUp(float x, float y,float z)    { cameraUp    = {x,y,z}; }
        
        
        glm::vec3 get_cameraPos()     const { return camera.Position; }
        glm::vec3 get_cameraFront()   const { return camera.Front; }
        glm::vec3 get_cameraUp()      const { return camera.Up; }
        
        
        
        /**
         * @brief Creates and configures a Vertex Array Object (VAO) with vertex data.
         * 
         * Generates a VAO and VBO, binds them, uploads vertex data to GPU memory,
         * and configures the vertex attribute pointer for position data (3 floats per vertex).
         * The vertex data is stored as static data that doesn't change after upload.
         */
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
                sys_logger.warn("Framebuffer is not complete!");
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);  // allow resizing (default)
        }
        
        
        void RenderPlanets(const std::vector<PlanetProperties<float>> &planets,
            float viewportWidth,
            float viewportHeight)
            {
                glEnable(GL_DEPTH_TEST);
                glUseProgram(shaderProgram);
                glBindVertexArray(VAO);
                
                glm::mat4 view = glm::lookAt(
                    camera.Position,
                    camera.Position + camera.Front, 
                    camera.Up
                );

                
                glm::mat4 projection = glm::perspective(
                    glm::radians(45.0f),
                    viewportWidth / viewportHeight,
                    0.01f,
                    1000.0f
                );
                
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"view"),
                1, GL_FALSE, &view[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"projection"),
                1, GL_FALSE, &projection[0][0]);
                glm::vec3 color;
                
                for (const auto& p : planets) {

                    // Translation object
                    glm::mat4 model = glm::translate(
                        glm::mat4(1.0f),
                        camera.scalePos(p.get_position())
                    );
                    
                    // Fixed scale for visibility
                    float radius = std::max(camera.scaleSize(p.get_radius()), 0.1f); 
                    model = glm::scale(model, glm::vec3(radius));
                    
                    glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),
                    1, GL_FALSE, &model[0][0]);
                    
                    // Color implementation
                    color = p.get_color();
                    glUniform3fv(glGetUniformLocation(shaderProgram,"color"), 1, &color[0]); 
                    
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
                
                glBindVertexArray(0);
                glUseProgram(0);
            }
            
            void RenderToFramebuffer(const std::vector<PlanetProperties<float>>& planets)
            {
                if (framebuffer == 0) {
                    std::cerr << "Framebuffer not initialized\n";
                    return;
                }
                
                sys_logger.debug("RenderToFramebuffer called");
                
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
                glViewport(0, 0, (int)width, (int)height);
                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                RenderPlanets(planets, width, height);
                // RenderScene(width, height);
                
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            
            
            
            
            // DEMO FUNCTION -- TEST PURPOSES
            void RenderScene(float viewportWidth, float viewportHeight) {
                glUseProgram(shaderProgram);
                glBindVertexArray(VAO);
                
                glm::mat4 view = glm::lookAt(
                    camera.Position,
                    camera.Position + camera.Front, 
                    camera.Up
                );
                
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
            
        
            float GetWidth() const { return width; }
            float GetHeight() const { return height; }
        };
        
    } // namespace graphics
    