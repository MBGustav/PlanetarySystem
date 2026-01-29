# Introduction
Notre intention dans ce projet est d’associer des concepts d’organisation de l’architecture du code - avec l’utilisation, par exemple, de design pattern - avec la programmation orientée objet. Dans un premier temps, nous utiliserons des bibliothèques open-source comme [Imgui](https://github.com/ocornut/imgui) et [Glad](https://github.com/Dav1dde/glad). Cela permet même que le projet soit disponible pour différentes plates-formes de systèmes d’exploitation.



# date livraison - 26 janv
## Critéres de evaluation
- Qualidade de codigo
	- boa construção
	- evitar estilo de C
	- comentaire -> documentation (dOxygen)s
- Qualidade de Arquitetura
- Qualidade de apresentação
- Conjuntos de função
	- funções complicadas de se realizar, trade-off
	- riqueza de dificuldade.


<!-- rapidJSON -para salvar dados de ficheiro -->
<!-- - Gpredict -->

# Installing Dependences (LINUX):

## SDL
```sh
git clone https://github.com/libsdl-org/SDL.git -b SDL2
cd SDL
mkdir build
cd build
../configure
make
sudo make install
```
## LIB. GLFW3

```sh
sudo apt install libglfw3 libglfw3-dev
```

## Download and Execute

```sh
#This install imgui and glad in the machine
git clone --recursive https://github.com/mbgustav/PlanetarySystem
mkdir build ; cd build
cmake ..
make

```



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

	class JSONReader{
        - string filepath
        - map<char, cmd> dataMap
        + file_exists()
        + parse_content()
		+ get_data_as_map()
		+ get_data_as_vector()
    }

	class CelestialObjectJSONReader{
        - string filepath
        - map<char, cmd> dataMap
        + file_exists()
        + parse_content()
		+ get_data_as_map()
		+ get_data_as_vector()
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
		+ sim_only(message)
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

    namespace graphics {
		class Camera {
			- Position, Front
			- Up, WorldUp, Target
			- Pitch Yaw
			+ updateVectors()
			+ rotate()
			+ zoom(dx, dy)
			+ ComputeVisualScale(vector planets)
			+ viewMatrix()
		}


		class GraphicsWrapper {
		- shaderProgram;
		- VBO, VAO;
		- framebuffer;
		- mainVAO, mainVBO;
		- figureVAO, figureVBO;
		- framebufferTexture ;
		- Camera cam

	    + SetupCamera()
		+ CameraViewPosition(x,y,z)
		+ CameraViewRotate(x,y,z)
    }
	}

	namespace gui {

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
	}

    class SimulationCorp {
	    - vector~CelestialObject~ planets
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

    Application      --> ImguiLayer      : (Compose)
    Application      --> SimulationCorp  : (Compose)
	Application      --> GraphicsWrapper : (Compose)
	ImguiLayer       ..> GraphicsWrapper : (Renders)
    ImguiLayer       ..> ExternalLibs    : (Imports)
    ImguiLayer       ..> SimulationCorp  : (Control data)
	GraphicsWrapper  ..> SimulationCorp  : (Import objects)

```


## WINDOW LANDING PAGES
Drafted ideas about how the pages are expected to be.
<img src='img/image.png'>

