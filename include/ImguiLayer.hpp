#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdexcept>
#include <cstdio>
#include <iostream>
#include <glm/glm.hpp>
#include "GraphicsWrapper.hpp"
#include "SimulationWrapper.hpp"
#include "Logger.hpp"

typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiDemoMarkerCallback      GImGuiDemoMarkerCallback;
extern void*                        GImGuiDemoMarkerCallbackUserData;
ImGuiDemoMarkerCallback             GImGuiDemoMarkerCallback = NULL;
void*                               GImGuiDemoMarkerCallbackUserData = NULL;
#define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback("imgui_demo.cpp", __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)

// Graphic & UI Includes
// NOTE: Ensure these files exist in your include path

namespace gui
{
    // Window dimensions
    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 600;
    constexpr char VIEW_TITLE[] = "Simulation Viewer";    
    
    static float MIN_RADIUS = 0.1f;
    static float MAX_RADIUS = 10.0f;

    static float MIN_MASS = 0.1f;
    static float MAX_MASS = 50.0f;

    static float MIN_TIMESCALE = 01.0f;
    static float MAX_TIMESCALE = 10.0f;


    // Persistent Data for Window Managing
    struct ImGuiWindowData {
        bool ShowWelcomeWindow       = false; //TODO: set to true to show at start
        bool ShowConfigurationWindow = false;
        bool ShowSimulationWindow    = true;
        bool ShowAppFullscreen       = false;
        
        // About Data import for simulation
        bool ImportSimData_example  = false;
        bool ImportSimData_gpredict = false;
        
        // Debugger Tools
        bool ShowDebugLog = false;
        bool ShowAbout = false;

        // Parameters Constraints
    };
    
    enum Options
    {
        SIMPLE_SIMULATION,
        SOLAR_SIMULATION,
        REAL_TIME_SIMULATION,
        CONFIGURE_SETTINGS
    };
    
    class ImguiLayer
    {
        private:
        GLFWwindow* window = nullptr;
        bool setupDone = false;
        ImGuiWindowData WMData;
        graphics::GraphicsWrapper *gfx; // Reference to GraphicsWrapper
        SimulationWrapper *simulation;// reference to simulation
        // Private helper functions for drawing specific windows
        void WelcomeWindow();
        void ConfigurationWindow(bool *page_open);
        void SimulationWindow();
        public:
        
        //constructor
        ImguiLayer(graphics::GraphicsWrapper &graphics, SimulationWrapper &simulation);
        ImguiLayer(){};
        ~ImguiLayer();
        
        void setup();
        void clean_up();
        bool ShouldClose();
        void processEvents();
        
        
        // Lifecycle methods
        void beginFrame();
        void drawUI(); // Just submits widgets, does not Render/Swap
        void endFrame();
    };
    
    // ---------------------------------------------------------
    // ImguiLayer Implementation
    // ---------------------------------------------------------
    // Marked inline because they are implemented in a .hpp file
    
    inline ImguiLayer::ImguiLayer(graphics::GraphicsWrapper &graphics, SimulationWrapper &simulation): 
            simulation(&simulation), gfx(&graphics){
        sys_logger.debug("ImguiLayer constructor called");
    }
    
    inline ImguiLayer::~ImguiLayer() {
        if (window) {
            clean_up();
        }
    }
    
    inline void ImguiLayer::setup()
    {
        if(!glfwInit()) 
        throw std::runtime_error("[ERROR] initialize GLFW");
        
        sys_logger.debug("GLFW initialized successfully");
        
        // Decide GL+GLSL versions
        #if defined(__APPLE__)
        // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
        #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        #endif
        
        this->window = glfwCreateWindow(WIDTH, HEIGHT, VIEW_TITLE, nullptr, nullptr);
        
        if (!this->window) {
            glfwTerminate();
            throw std::runtime_error("[ERROR] create GLFW window");
        }
        
        sys_logger.debug("GLFW window created successfully");
        
        glfwMakeContextCurrent(this->window);
        glfwSwapInterval(1); // Enable vsync
        
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            glfwDestroyWindow(this->window);
            glfwTerminate();
            throw std::runtime_error("[ERROR] initialize GLAD");
        }
        
        
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        
        // Init backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        
        this->setupDone = true;
        
        sys_logger.debug("ImGuiLayer setup completed successfully");
    }
    
    inline void ImguiLayer::clean_up()
    {
        if(setupDone) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            glfwDestroyWindow(this->window);
            glfwTerminate();
            setupDone = false;
        }
    }
    
    inline bool ImguiLayer::ShouldClose() {
        return glfwWindowShouldClose(window);
    }
    
    inline void ImguiLayer::processEvents()
    {
        glfwPollEvents();
    }
    
    inline void ImguiLayer::beginFrame()
    {
        
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    
    inline void ImguiLayer::drawUI()
    {
        sys_logger.debug("Drawing ImGui UI elements");
        if (WMData.ShowWelcomeWindow) 
        this->WelcomeWindow();
        
        if (WMData.ShowConfigurationWindow) 
        this->ConfigurationWindow(&WMData.ShowConfigurationWindow);
        
        if (WMData.ShowSimulationWindow) this->SimulationWindow();
    }
    
    inline void ImguiLayer::endFrame()
    {
        // Rendering
        ImGui::Render();
        
        sys_logger.debug("Rendering ImGui frame");
        
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
    }
    
    inline void ImguiLayer::WelcomeWindow()
    {
        static Options selectedOption = SIMPLE_SIMULATION;
        
        if(ImGui::Begin("Welcome", &WMData.ShowWelcomeWindow, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Welcome to the Simulation Application!");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::Text("Choose a simulation mode:");
            ImGui::RadioButton("Simple Simulation", (int*)&selectedOption, SIMPLE_SIMULATION);
            ImGui::RadioButton("Solar Simulation", (int*)&selectedOption, SOLAR_SIMULATION);
            ImGui::RadioButton("Real Time Simulation", (int*)&selectedOption, REAL_TIME_SIMULATION);
            ImGui::RadioButton("Config Setting", (int*)&selectedOption, CONFIGURE_SETTINGS);
            
            ImGui::Spacing();
            ImGui::Spacing();
            
            if (ImGui::Button("Start / Open")) {
                switch (selectedOption)
                {
                    case SOLAR_SIMULATION:
                    WMData.ImportSimData_example = true;
                    WMData.ShowSimulationWindow = true;
                    
                    break;
                    case SIMPLE_SIMULATION:
                    
                    WMData.ShowSimulationWindow = true;
                    WMData.ShowWelcomeWindow = false;
                    break;
                    case REAL_TIME_SIMULATION:
                    WMData.ImportSimData_gpredict = true;
                    WMData.ShowSimulationWindow = true;
                    WMData.ShowWelcomeWindow = false;
                    break;
                    case CONFIGURE_SETTINGS:
                    WMData.ShowConfigurationWindow = true;
                    break;
                    default: break;
                }
            }
        }
        ImGui::End();
    }
    
    inline void ImguiLayer::ConfigurationWindow(bool *page_open)
    {
        ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Configuration", page_open, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Close", "Ctrl+W")) { *page_open = false; }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            
            static int selected = 0;
            
            // Left Pane
            {
                ImGui::BeginChild("left pane", ImVec2(150, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
                for (int i = 0; i < 20; i++)
                {
                    char label[128];
                    sprintf(label, "MyObject %d", i);
                    if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SelectOnNav))
                    selected = i;
                }
                ImGui::EndChild();
            }
            ImGui::SameLine();
            
            // Right Pane
            {
                ImGui::BeginGroup();
                ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); 
                ImGui::Text("MyObject: %d", selected);
                ImGui::Separator();
                if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
                {
                    if (ImGui::BeginTabItem("Description"))
                    {
                        ImGui::TextWrapped("Configuration details for item %d.", selected);
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Details"))
                    {
                        ImGui::Text("ID: %08d", selected * 1050);
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::EndChild();
                
                if (ImGui::Button("Revert")) {}
                ImGui::SameLine();
                if (ImGui::Button("Save")) {}
                ImGui::EndGroup();
            }
        }
        ImGui::End();
    }
    
    inline void ImguiLayer::SimulationWindow()
    {
        
        //2. Control Panel Variables
        static float timeScale = 1.0f;
        static bool isPaused = true;
        static float gravity[3] = { 0.0f, -9.81f, 0.0f };
        static float camPos[3]   = { 0.0f, 0.0f, 0.0f };
        static float camFront[3] = {10.0f, 0.0f, 0.0f };
        static float camUp[3]    = { 5.0f, 5.0f, 0.0f };

        static bool showGrid = true;
        simulation->setDeltaTime(0.001f); // assuming 60 FPS base

        if (!isPaused){
            simulation->UpdateSimulation(simulation->getDeltaTime());
        }
        
        
        // --------- CAMERA UPDATE -----------
        gfx->set_cameraPos(camPos[0], camPos[1], camPos[2]);
        gfx->set_cameraFront(camFront[0], camFront[1], camFront[2]);
        gfx->set_cameraUp(camUp[0], camUp[1], camUp[2]);
        

        // ---------- PLANET CONTROLS UPDATE ---------
        // 3. Planet Management Variables

        // std::vector<std::string> items = {"Earth", "Mars", "Pluto", "Sun"};

        std::vector<std::string> planet_names = simulation->getPlanetNames();
        static int planet_idx = 0;
        static int item = -1;
        
        static float radius = 0.0f;
        static float mass = 0.0f;
        static float velocity[3] = { 0.0f, 0.0f, 0.0f };


        if (item != -1 && item != planet_idx) {
            planet_idx = item;
            radius = simulation->getPlanets()[item].get_radius();
            mass = simulation->getPlanets()[item].get_mass();
            glm::vec3 vel = simulation->getPlanets()[item].get_velocity();
            velocity[0] = vel.x;
            velocity[1] = vel.y;
            velocity[2] = vel.z;
        }
        
        bool valuesChanged = true;
        if (item != -1) {
            valuesChanged |= ImGui::SliderFloat("Radius##planet", &radius, MIN_RADIUS, MAX_RADIUS);
            valuesChanged |= ImGui::SliderFloat("Mass##planet", &mass, MIN_MASS, MAX_MASS);
            valuesChanged |= ImGui::InputFloat3("Velocity##planet", velocity);
        }
        
        
        if (item != -1) {
            simulation->setPlanet_radius(item, radius);
            simulation->setPlanet_mass(item, mass);
            simulation->setPlanet_velocity(item, glm::vec3(velocity[0], velocity[1], velocity[2]));
            sys_logger.info("Planet param changed: radius=" + std::to_string(radius) + 
                                 ", mass=" + std::to_string(mass) + 
                                 ", velocity=(" + std::to_string(velocity[0]) + ", " + 
                                std::to_string(velocity[1]) + ", " + std::to_string(velocity[2]) + ")");
        }
        // valuesChanged = false;

        



        // ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
        // Get the viewport of the main window (the OS window)
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        WMData.ShowWelcomeWindow = false;
        // Force the ImGui window to cover the whole viewport
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        
        
        
        // Main Window with Menu Bar
        if (ImGui::Begin("Simulation Dashboard", &WMData.ShowSimulationWindow, ImGuiWindowFlags_MenuBar))
        {
            // 1. Menu Bar
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Simulation"))
                {
                    if (ImGui::MenuItem("Restart")) { /* Add restart logic */ }
                    if (ImGui::MenuItem("Export Data")) { /* Add export logic */ }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Close")) { 
                        WMData.ShowSimulationWindow = false; 
                        WMData.ShowWelcomeWindow = true; //reopens welcome
                        
                    }
                    ImGui::EndMenu();
                }
                
                if (ImGui::BeginMenu("View"))
                {
                    ImGui::MenuItem("Show Grid", "", &showGrid);
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            
            // 2. Left Sidebar (Controls)
            // -------------------------------------------------
            ImGui::BeginChild("Controls", ImVec2(250, -150), true);
            {
                ImGui::TextColored(ImVec4(1,1,0,1), "CONTROL PANEL");
                ImGui::Separator();
                ImGui::Spacing();
                
                // Playback controls
                if (ImGui::Button(isPaused ? "PLAY" : "PAUSE", ImVec2(80, 30))) {
                    isPaused = !isPaused;
                }
                ImGui::SameLine();
                if (ImGui::Button("RESET", ImVec2(80, 30))) {
                    timeScale = 1.0f;
                    isPaused = true;
                }
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Physics Parameters");
                
                // --------- Sliders - Physiscs Params -----------
                ImGui::DragFloat("Time Scale", &timeScale, 0.1f, MIN_TIMESCALE, MAX_TIMESCALE);
                // ImGui::SliderInt("Iterations", &iterations, 1, 1000);
                ImGui::InputFloat3("Gravity", gravity);
                

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Camera Controller");

                ImGui::InputFloat3("Position", camPos);
                ImGui::InputFloat3("Front", camFront);
                ImGui::InputFloat3("Up", camUp);
                



                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("System Status");
                
                // Read-only text
                ImGui::Text("Objects: %d", simulation->getPlanetCount());
                ImGui::Text("delta-time: %.4f", isPaused ? 0.0f : simulation->getDeltaTime());

                ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                if (isPaused) ImGui::TextColored(ImVec4(1, 0, 0, 1), "Status: PAUSED");
                else          ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: RUNNING");
            }
            ImGui::EndChild();
            
            // 3. Left Sidebar(Planets management)
            ImGui::BeginChild("Controls", ImVec2(250, -150), true);
            {
                ImGui::Separator();
                ImGui::TextColored(ImVec4(1,1,0,1), "Planet Manager");
                ImGui::Spacing();
                
                // Playback controls
                if (ImGui::BeginCombo("Combo", item == -1 ? "" : planet_names[item].c_str())){
                    for (int n = 0; n < planet_names.size(); n++){
                        bool is_selected = (item == n);
                        if (ImGui::Selectable(planet_names[n].c_str(), is_selected))
                        item = n;
                        if (is_selected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::SliderFloat("Radius", &radius, MIN_RADIUS, MAX_RADIUS); //ImGui::SameLine();
                ImGui::SliderFloat("Mass", &mass, MIN_MASS, MAX_MASS); //ImGui::SameLine();
                // ImGui::SliderFloat("Velocity", &planet_parameters[2], 0.0f, 5.0f);
                ImGui::Spacing();
                
                // ImGui::SameLine();
                // if (ImGui::Button("SAVE / SET", ImVec2(80, 30))) {
                    //send values to simulation (TODO)
                // }
                
                
                // if (isPaused) ImGui::TextColored(ImVec4(1, 0, 0, 1), "Status: PAUSED");
                // else          ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: RUNNING");
            }
            ImGui::EndChild();
            
            
            
            //Simulation Window Layout Separation
            // 3. Right Viewport (Visualization)
            ImGui::SameLine(); // Puts the next child to the right of the previous one
            // -------------------------------------------------
            ImGui::BeginGroup();
            {
                // Top Tab Bar
                if (ImGui::BeginTabBar("SimTabs"))
                {
                    if (ImGui::BeginTabItem("3D Viewport"))
                    {
                        
                        // EXAMPLE 3D VIEWPORT AREA                        
                        ImGui::BeginChild("ViewportRegion", ImVec2(0, -150), true, ImGuiWindowFlags_NoScrollbar);
                        ImGui::Text("Simulation rendering goes here...");
                        
                        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
                        
                        if (viewportSize.x != gfx->GetWidth() || viewportSize.y != gfx->GetHeight()) {
                            gfx->SetupFramebuffer(viewportSize);
                        }
                        
                        // Render the scene into the ImGui child area
                        ImGui::Image((void*)(intptr_t)gfx->GetFramebufferTexture(), viewportSize, ImVec2(0,1), ImVec2(1,0));
                        
                        // Redimensiona framebuffer se o tamanho mudou
                        
                        ImGui::EndChild();
                        
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("I don't work yet"))
                    {
                        ImGui::Text("Graph data would appear here.");
                        // You could use ImPlot here in the future
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                
                // Bottom Console / Log
            }
            ImGui::EndGroup();
            
            ImGui::TextColored(ImVec4(1,1,0,1), "SYSTEM LOG");
            ImGui::Separator();
            
            ImGui::BeginChild("LogRegion", ImVec2(0, 0), true);
            
            //TODO: link to a file from simulation (.simlog)
            // read from logBuffer instead of static text in the future
            static ImGuiTextBuffer logBuffer;
            for (auto i = 0; i < 5; ++i)   {
                ImGui::TextColored(ImVec4(0.5f,0.5f,0.5f,1.0f), "[10:00:01] System initialized.");
                ImGui::TextColored(ImVec4(0.5f,0.5f,0.5f,1.0f), "[10:00:05] Loaded assets.");
            }
            if (!isPaused) 
            ImGui::Text("[INFO] Calculating step...");
            ImGui::EndChild();
        }
        ImGui::End();
    }
} // namespace gui