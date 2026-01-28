#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>


#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include "GraphicsWrapper.hpp"
#include "SimulationWrapper.hpp"
#include "Logger.hpp"
#include "SimulationFiles.hpp"
#include "PlanetJSONReader.hpp"
typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiDemoMarkerCallback      GImGuiDemoMarkerCallback;
extern void*                        GImGuiDemoMarkerCallbackUserData;
ImGuiDemoMarkerCallback             GImGuiDemoMarkerCallback = NULL;
void*                               GImGuiDemoMarkerCallbackUserData = NULL;
#define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback("imgui_demo.cpp", __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)

// Graphic & UI Includes
// NOTE: Ensure these files exist in your include path


// To read the files from archives
struct SimFileInfo {
    std::string filename;
    std::string fullPath;
    int bodyCount;
};


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
    static float MAX_TIMESCALE = 50.0f;
    
    
    // Persistent Data for Window Managing
    struct ImGuiWindowData {
        bool ShowWelcomeWindow       = true; //TODO: set to true to show at start
        bool ShowConfigurationWindow = false;
        bool ShowSimulationWindow    = true;
        bool ShowAppFullscreen       = false;
        bool showPlanetOverlay       = false;
        // About Data import for simulation
        bool ImportSimData_example  = false;
        bool ImportSimData_gpredict = false;
        
        // Debugger Tools
        bool ShowDebugLog = false;
        bool ShowAbout = false;
        
        // Parameters Constraints
    };
    
    // Data from Planets we modify/check
    struct PlanetUIState {
        int selected = -1;
        float radius = 0.0f;
        float mass = 0.0f;
        glm::vec3 velocity = {0,0,0};
    };
    
    
    enum Options
    {
        SIMPLE_SIMULATION,
        THREE_BODY_PROBLEM,
        SOLAR_SYSTEM_SIMULATION,
        N_BODY_SIMULATION,
        CONFIGURE_SETTINGS
    };
    
    class ImguiLayer
    {
        private:
        GLFWwindow* window = nullptr;
        bool setupDone = false;
        ImGuiWindowData WMData;
        graphics::GraphicsWrapper *gfx; // Reference to GraphicsWrapper
        SimulationWrapper *simulation;  // reference to simulation
        
        SimulationFiles SimFiles;
        
        void load_simulation(string filepath);
        
        // Private helper functions for drawing specific windows
        void WelcomeWindow();
        void ConfigurationWindow(bool *page_open);
        void SimulationWindow();
        
        void DrawPlanetOverlay(PlanetProperties<float>& selected_planet);
        
        public:
        
        ImguiLayer() = default;
        
        //constructor
        ImguiLayer(graphics::GraphicsWrapper &graphics, SimulationWrapper &simulation);
        // ImguiLayer(){};
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
    inline void ImguiLayer::load_simulation(string filepath) {
        sys_logger.debug("Loading simulation via ImguiLayer");
        PlanetJSONReader<float> reader("../" + filepath);
        

        // Load the the file and set the simulation state
        simulation->setInitialState(reader, true);
        
    }
    inline ImguiLayer::ImguiLayer(graphics::GraphicsWrapper &graphics, SimulationWrapper &simulation): 
    simulation(&simulation), gfx(&graphics), SimFiles(""){
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
            ImGui::RadioButton("Solar System Simulation" , (int*)&selectedOption, SOLAR_SYSTEM_SIMULATION);
            ImGui::RadioButton("3 Body Problem" , (int*)&selectedOption, THREE_BODY_PROBLEM);
            // ImGui::RadioButton("N Body Simulation" , (int*)&selectedOption, N_BODY_SIMULATION);
            
            
            
            ImGui::Spacing();
            ImGui::Spacing();
            
            if (ImGui::Button("Start / Open")) {
                WMData.ShowWelcomeWindow = false;
                WMData.ShowSimulationWindow = true;
                switch (selectedOption)
                {
                    case SIMPLE_SIMULATION:
                    load_simulation("simulations/simple_simulation.json");
                    break;
                    
                    case THREE_BODY_PROBLEM:
                    load_simulation("simulations/three_body_problem.json");
                    break;
                    case SOLAR_SYSTEM_SIMULATION:
                    load_simulation("simulations/solar_system.json");
                    break;
                    case N_BODY_SIMULATION:
                    // load_simulation("n_body_simulation.json");
                    break;
                    default:
                    WMData.ShowWelcomeWindow = true;
                    WMData.ShowSimulationWindow = false;
                    break;
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
    
    void  ImguiLayer::DrawPlanetOverlay(PlanetProperties<float>& planet){
        
        ImGui::Begin("Planet Configuration", nullptr,
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoCollapse);
            
            ImGui::Text("Planet: %s", planet.get_name().c_str());
            ImGui::Separator();
            // --- Position ---
            glm::vec3 pos = planet.get_position();
            if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
                planet.set_position(pos);
            }
            
            // --- Velocity ---
            glm::vec3 vel = planet.get_velocity();
            if (ImGui::DragFloat3("Velocity", &vel.x, 0.01f)) {
                planet.set_velocity(vel);
            }
            
            // --- Mass ---
            float mass = planet.get_mass();
            if (ImGui::DragFloat("Mass", &mass, 0.1f, 0.001f, 10000.0f)) {
                planet.set_mass(mass);
                sys_logger.simulation("Planet " + planet.get_name() + " mass set to " + std::to_string(mass));
            }
            
            // --- Radius ---
            float radius = planet.get_radius();
            if (ImGui::DragFloat("Radius", &radius, 0.01f, 0.01f, 100.0f)) {
                planet.set_radius(radius);
                sys_logger.simulation("Planet " + planet.get_name() + " radius set to " + std::to_string(radius));
            }
            
            // --- Fixed toggle ---
            bool fixed = planet.is_fixed();
            if (ImGui::Checkbox("Fixed Position", &fixed)) {
                planet.set_fixed(fixed);
                sys_logger.simulation("Planet " + planet.get_name() + (fixed ? " set to fixed." : " set to movable."));
            }
            static ImVec4 color;
            // Inicializa color com o valor do planeta na primeira chamada
            if (ImGui::IsWindowAppearing()) {
                glm::vec3 c = planet.get_color();
                color = ImVec4(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1.0f);
            }
            
            if (ImGui::ColorEdit4("Color", (float*)&color, ImGuiColorEditFlags_NoInputs)) {
                // Atualiza diretamente o planeta
                planet.set_color(glm::vec3(color.x * 255.0f,
                    color.y * 255.0f,
                    color.z * 255.0f));
                }
                ImGui::Separator();

                
                if (ImGui::Button("Reset Forces")) {
                    // planet.set_force(glm::vec3(0,0,0));
                    planet.set_acceleration(glm::vec3(0,0,0));
                    sys_logger.simulation("Planet " + planet.get_name() + " forces and acceleration reset.");
                }
                
                ImGui::End();
            }
            
            
            inline void ImguiLayer::SimulationWindow()
            {
                
                //2. Control Panel Variables
                static float timeScale = 1.0f;
                static bool isPaused = true;
                static bool showGrid = true;
                static int item = -1;          
                static int planet_idx = 0, selectedPlanetIdx = 0;
                bool valuesChanged = true;
                
                
                
                // simulation->setDeltaTime(0.001f); // assuming 60 FPS base
                
                if (!isPaused){
                    simulation->UpdateSimulation(0.001f, timeScale);
                }
                
                
                std::vector<std::string> planet_names = simulation->getPlanetNames();
                
                if (item != -1 && item != planet_idx) {
                    planet_idx = item;            
                }
                
                
                if (WMData.showPlanetOverlay && selectedPlanetIdx != -1){
                    PlanetProperties<float>& planet = simulation->PlanetByIdx(selectedPlanetIdx);
                    DrawPlanetOverlay(planet);
                }
                
                
                
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
                            if (ImGui::MenuItem("Restart")) { simulation->reset_simulation();}
                            if (ImGui::MenuItem("Import Simultation")) { /* Add export logic */ }
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
                            simulation->reset_simulation();
                        }
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Text("Physics Parameters");
                        
                        // --------- Sliders - Physiscs Params -----------
                        ImGui::DragFloat("Time Scale", &timeScale, 0.1f, MIN_TIMESCALE, MAX_TIMESCALE);
                        // ImGui::SliderInt("Iterations", &iterations, 1, 1000);                    
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Text("Camera Controller");
                        
                        bool rotationChanged = false;
                        
                        // 2. Rotação (Edita Yaw/Pitch e chama updateVectors)
                        rotationChanged |= ImGui::DragFloat3("Position", &(gfx->camera.Position[0]), 0.1f);
                        rotationChanged |= ImGui::DragFloat("Yaw", &(gfx->camera.Yaw), 0.5f);
                        rotationChanged |= ImGui::SliderFloat("Pitch", &(gfx->camera.Pitch), -89.0f, 89.0f);
                        rotationChanged |= ImGui::SliderFloat("Zoom (FOV)", &(gfx->camera.Fov), 1.0f, 120.0f);
                        
                        
                        if (rotationChanged) {
                            sys_logger.simulation("Camera rotation changed: Yaw=" + std::to_string(gfx->camera.Yaw) + ", Pitch=" + std::to_string(gfx->camera.Pitch));
                            gfx->camera.updateVectors();
                        }
                        
                        
                        ImGui::Separator();
                        if (ImGui::Button("Reset View")) {
                            gfx->camera.Position = glm::vec3(0, 0, 15);
                            gfx->camera.Yaw = -90.0f;
                            gfx->camera.Pitch = 0.0f;
                            gfx->camera.Fov = 45.0f;
                            gfx->camera.updateVectors();
                        }                
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Text("System Status");
                        
                        // Read-only text
                        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                        if (isPaused) ImGui::TextColored(ImVec4(1, 0, 0, 1), "Status: PAUSED");
                        else          ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: RUNNING");
                        
                        ImGui::Text("Objects: %d", simulation->getPlanetCount());
                        ImGui::Text("delta-time: %.4f", isPaused ? 0.0f : simulation->getDeltaTime());
                        
                    }
                    ImGui::EndChild();
                    
                    // 3. Left Sidebar(Planets management)
                    ImGui::BeginChild("Controls", ImVec2(250, -150), true);
                    {
                        ImGui::Separator();
                        ImGui::TextColored(ImVec4(1,1,0,1), ": ");
                        ImGui::Spacing();
                        
                        // Playback controls
                        if (ImGui::BeginCombo("##SelectPlanet", item == -1 ? "" : planet_names[item].c_str())){
                            for (int n = 0; n < planet_names.size(); n++){
                                bool is_selected = (item == n);
                                if (ImGui::Selectable(planet_names[n].c_str(), is_selected))
                                item = n;
                                if (is_selected) ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                        static ImVec4 color = ImVec4(114 / 255.0f,  155 / 255.0f, 200/ 255.0f, 255 / 255.0f);
                        ImGui::ColorEdit4("TesteColor##2f", (float*)&color,ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);
                        
                        // ImGui::SliderFloat("Velocity", &planet_parameters[2], 0.0f, 5.0f);
                        ImGui::Spacing();
                        
                        // ImGui::SameLine();
                        if (ImGui::Button("SELECT", ImVec2(80,30))) {
                            selectedPlanetIdx = item;
                            WMData.showPlanetOverlay = true;
                            isPaused = true; //Pause simulation when being modified
                        }
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
                    
                    ImGui::BeginChild("LogRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
                    
                    static std::string logBuffer;
                    static std::mutex logMutex;
                    
                    // Update log buffer from separate thread
                    static std::thread logThread;
                    static bool logThreadRunning = false;
                    
                    if (!logThreadRunning) {
                        logThreadRunning = true;
                        logThread = std::thread([&]() {
                            while (logThreadRunning) {
                                {
                                    std::lock_guard<std::mutex> lock(logMutex);
                                    logBuffer = sys_logger.get_log_buffer();
                                }
                                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                            }
                        });
                    }
                    
                    {
                        std::lock_guard<std::mutex> lock(logMutex);
                        ImGui::Text("[INFO] Calculating step...");
                        if (!logBuffer.empty()) {
                            ImGui::TextUnformatted(logBuffer.c_str());
                            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                                ImGui::SetScrollHereY(1.0f);
                        } else {
                            ImGui::TextDisabled("No simulation logs yet.");
                        }
                    }
                    
                    ImGui::EndChild();
                }
                ImGui::End();
            }
        } // namespace gui