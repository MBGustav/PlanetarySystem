
#include "include/Application.hpp"
#include "include/PlanetJSONReader.hpp"
#include "include/SimulationFiles.hpp"
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


void dbg_simulationPath()
{

    SimulationFiles SF;

    auto a = SF.availableSimulations();

    
    SF.loadSimulation(a[0]);
    

    for (auto &it : a){
        cout << it << "\n";
    }

    cout << SF; 

    auto plnt = SF.loadSimulation("solar_system.json");

    SimulationWrapper sim;


    sim.setInitialState(plnt, true);

    cout <<  "dt: " << sim.getDeltaTime() << endl;
    cout << "names:";
    for(auto i : sim.getPlanetNames()) cout << i << endl;
    cout << "planets\n"; 
    for(auto it : sim.getPlanets()){
        cout << it << endl;
    }
}


int main() {
    // GLFW init

    Application app;
    app.run();
    
    // dbg_simulationPath();
    return 0;
}

