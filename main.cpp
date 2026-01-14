
#include "include/Application.hpp"
#include "include/PlanetJSONReader.hpp"

using namespace std;

void dbg_PlanetJSON()
{
PlanetJSONReader r("../simulations/simple.json");

    std::cout << "\n--- PARSED PLANETS ---\n";
    std::vector<PlanetProperties> my_planets = r.get_planets();
    
    // Test to see if there is more than one object
    for (const auto &planet : my_planets) {
        // Assumindo que PlanetProperties tem getters
        std::cout << "Name: " << planet.get_name() << "\n"; 
        std::cout << "Mass: " << planet.get_mass() << "\n";
        std::cout << "----------------\n";
    }
}

int main() {
    // GLFW init

    Application app;

    app.run();
    
    return 0;

}

