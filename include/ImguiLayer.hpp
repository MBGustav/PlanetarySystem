#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdexcept>

typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiDemoMarkerCallback      GImGuiDemoMarkerCallback;
extern void*                        GImGuiDemoMarkerCallbackUserData;
ImGuiDemoMarkerCallback             GImGuiDemoMarkerCallback = NULL;
void*                               GImGuiDemoMarkerCallbackUserData = NULL;
#define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback("imgui_demo.cpp", __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)


#include <stdexcept>
#include <cstdio>
#include <iostream>

// Graphic & UI Includes
// NOTE: Ensure these files exist in your include path
#include <glad/glad.h>   // Initialize GLAD before GLFW
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace gui
{
    // Window dimensions
    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 600;
    constexpr char VIEW_TITLE[] = "Simulation Viewer";    
    
    // Persistent Data for Window Managing
    struct ImGuiWindowData {
        bool ShowWelcomeWindow = true;
        bool ShowConfigurationWindow = false;
        bool ShowSimulationWindow = false;
        bool ShowAppFullscreen = false;
        
        // About Data import for simulation
        bool ImportSimData_example  = false;
        bool ImportSimData_gpredict = false;
        
        // Debugger Tools
        bool ShowDebugLog = false;
        bool ShowAbout = false;
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
        
        // Private helper functions for drawing specific windows
        void WelcomeWindow();
        void ConfigurationWindow(bool *page_open);
        void SimulationWindow();
    public:
        ImguiLayer();
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

    inline ImguiLayer::ImguiLayer() {}

    inline ImguiLayer::~ImguiLayer() {
        if (window) {
            clean_up();
        }
    }
    
    inline void ImguiLayer::setup()
    {
        if(!glfwInit()) 
            throw std::runtime_error("[ERROR] initialize GLFW");
        
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
        if (WMData.ShowWelcomeWindow) 
            this->WelcomeWindow();
        
        if (WMData.ShowConfigurationWindow) 
            this->ConfigurationWindow(&WMData.ShowConfigurationWindow);
            
        if (WMData.ShowSimulationWindow) this->SimulationWindow();
            // ImGui::Begin("Simulation Data", &WMData.ShowSimulationWindow);
            // ImGui::Text("Simulation is running...");
            // ImGui::End();
    }

    inline void ImguiLayer::endFrame()
    {
        // Rendering
        ImGui::Render();
        
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
        // hide the Welcome window
        

        // Static variables to hold state just for this UI demo
        static float timeScale = 1.0f;
        static bool isPaused = true;
        static int iterations = 100;
        static float gravity[3] = { 0.0f, -9.81f, 0.0f };
        static bool showGrid = true;
        
        // ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
        // Get the viewport of the main window (the OS window)
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        
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

                // Sliders
                ImGui::DragFloat("Time Scale", &timeScale, 0.1f, 0.0f, 5.0f);
                ImGui::SliderInt("Iterations", &iterations, 1, 1000);
                ImGui::InputFloat3("Gravity", gravity);

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("System Status");
                
                // Read-only text
                ImGui::Text("Objects: %d", 42);
                ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                if (isPaused) ImGui::TextColored(ImVec4(1, 0, 0, 1), "Status: PAUSED");
                else          ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: RUNNING");
            }
            ImGui::EndChild();

            ImGui::SameLine(); // Puts the next child to the right of the previous one

            // 3. Right Viewport (Visualization)
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
                        
                        if (showGrid) {
                            ImDrawList* draw_list = ImGui::GetWindowDrawList();
                            ImVec2 p = ImGui::GetCursorScreenPos();
                            draw_list->AddRect(p, ImVec2(p.x + 500, p.y + 300), IM_COL32(100, 100, 100, 255));
                            draw_list->AddLine(p, ImVec2(p.x + 500, p.y + 300), IM_COL32(255, 0, 0, 255));
                        }
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
            ImGui::Separator();
            ImGui::BeginChild("LogRegion", ImVec2(0, 0), true);
            
            ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "SYSTEM LOG");
            ImGui::Separator();
            
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