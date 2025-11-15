
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdexcept>


namespace gui
{
    // Window dimensions
    constexpr int WEIGHT = 800;
    constexpr int HEIGHT = 600;
    constexpr char VIEW_TITLE[] = "Simulation Viewer";    
    

    // redirect to different simulation options
    enum Options
    {
        SIMPLE_SIMULATION,
        SOLAR_SIMULATION,
        REAL_TIME_SIMULATION
    };

    class ImguiLayer
    {
        private:
            GLFWwindow* window;
            ImGuiIO io;
            bool setupDone = false;

            void clean_up();
            
            Options welcomeWindow();
            
            void SimulationWindow(Options option);
            
            
            
        public:
            void setup();
            void run();
            ImguiLayer();


        // ImguiLayer(/* args */);
        ~ImguiLayer();
    };

    ImguiLayer::ImguiLayer(){}

    void ImguiLayer::setup()
    {
        if(!glfwInit()) 
            throw std::runtime_error("[ERROR] initialize GLFW");
            
        this->window = glfwCreateWindow(WEIGHT, HEIGHT, VIEW_TITLE, nullptr, nullptr);

        if (!this->window) {
            glfwTerminate();
            throw std::runtime_error("[ERROR] create GLFW window");
        }

        glfwMakeContextCurrent(this->window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            glfwDestroyWindow(this->window);
            glfwTerminate();
            throw std::runtime_error("[ERROR] initialize GLAD");
        }


        IMGUI_CHECKVERSION();
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = ImGui::GetIO(); // If you need to access io, use this local reference


        // Init backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        this->setupDone = true;
    }

    void ImguiLayer::run()
    {
        if(!this->setupDone) this->setup();
        // Main loop
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();

            // ImGui Window
            Options opt = welcomeWindow();
            
            
            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }
        this->clean_up();
    }

    void ImguiLayer::clean_up()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(this->window);
        glfwTerminate();
    }

    Options ImguiLayer::welcomeWindow()
    {
        static Options selectedOption = SIMPLE_SIMULATION;

        ImGui::NewFrame();
        ImGui::Begin("Welcome", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("Welcome to the Simulation Application!");
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("Choose a simulation mode:");
        ImGui::RadioButton("Simple Simulation", (int*)&selectedOption, SIMPLE_SIMULATION);
        ImGui::RadioButton("Solar Simulation", (int*)&selectedOption, SOLAR_SIMULATION);
        ImGui::RadioButton("Real Time Simulation", (int*)&selectedOption, REAL_TIME_SIMULATION);

        ImGui::Spacing();
        ImGui::Spacing();

        if (ImGui::Button("Start Simulation")) {
            // Optionally, you could trigger something here
            // TODO: start simulation, maybe
        }

        ImGui::End();
        ImGui::Render();

        return selectedOption;
    }

    ImguiLayer::~ImguiLayer()
    {
    }

        // End of ImguiLayer class
};// namespace gui