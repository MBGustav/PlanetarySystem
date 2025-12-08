# Introduction
This project demonstrates a 3D Planetary Simulation. Considering that this project consists on concept application about Oriented Object Programming in practice, we decided to develop a robust simulation using some well known libraries in this subject:
- [Imgui](https://github.com/ocornut/imgui)
- [Glad](https://github.com/Dav1dde/glad)

<!-- - Gpredict -->

## System Organization
```mermaid
classDiagram
direction TB
namespace `Persistent Files(TO-DO)` {
	class SimulationFile {
		- Filename
		- List objs
		- TimeStopped
		+ ReadFile()
		+ SaveFile()
	}

    class ConfigFile{
        - CameraView
        - map<char, cmd> KeyboardHotKeys
        + ImportConfig()
        + ExportConfig()
    }
}

	class Logger{
        - Filename
        - log(message)
		- curr_time()
        + info(message)
        + debug(message)
		+ warning(message)
		+ error(message)
    }

    class Application {
	    - ImguiLayer uiLayer
	    - SimulationCorp simulation
		- GraphicsWrapper
	    + setup()
	    + run()
    }

    class GraphicsWrapper {
		- shaderProgram;
		- VBO, VAO;
		- framebuffer;
		- mainVAO, mainVBO;
		- figureVAO, figureVBO;
		- framebufferTexture ;
		- 



	    + SetupCamera() (TODO)
		+ CameraViewPosition(x,y,z) (TODO)
		+ CameraViewRotate(x,y,z) (TODO)
    }


    class ImguiLayer {
	    - GLFWwindow* window
	    - ImGuiWindowData data
		- GraphicsWrapper *gfx
        + ConfigWindow(ConfigFile) 
        + WelcomeWindow()
        + SimulationWindow()
	    + setup()
	    + processEvents()
	    + drawUI()
	    + SimulationWindow()
    }

    class SimulationCorp {
	    - vector~Planet~ planets
	    - float gravity
	    - float timeStep
	    + updatePhysics()
	    + calculateOrbits()
	    + resetSimulation()
    }

    class ExternalLibs {
	    GLFW / GLAD
	    Dear ImGui
        Standard Library
    }


	<<Library>> ExternalLibs

    Application      --> ImguiLayer : (Compose)
    Application      --> SimulationCorp : (Compose)
	ImguiLayer       ..> GraphicsWrapper : (Renders)
    ImguiLayer       ..> ExternalLibs : Uses
    ImguiLayer       ..> SimulationCorp : Control data
```


## WINDOW LANDING PAGES
Drafted ideas about how the pages are expected to be.
<img src='img/image.png'>

