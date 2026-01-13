// #include <iostream>
// #include <string>
// #include <vector>
// #include <map>
// #include <filesystem>
// #include "PlanetProperties.hpp"
// #include "PlanetJSONReader.hpp"
// #include <set>

// using std::string, std::vector, std::map, std::set;


// #define SIMULATION_FOLDER "simulations/"


// static const string RELATIVE_PATH = "../" + string(SIMULATION_FOLDER);

// class SimulationFiles
// {
// private:
//     string sim_filename;
//     set<string> simulations_available; // contains names of available simulations

//     // List all .json files in SIMULATION_FOLDER
//     void loadAvailableSimulations();

    

// public:
//     SimulationFiles(string sim_filename);
//     ~SimulationFiles();
    
//     //getter
//     vector<string> getAvailableSimulations() const;
    


//     vector<PlanetProperties> getPlanetsfromJSONFile(const string &simulation_name);
// };

// SimulationFiles::SimulationFiles(string sim_filename)
//     : sim_filename(sim_filename)
// {
//     loadAvailableSimulations();
// }

// void SimulationFiles::loadAvailableSimulations()
// {
//     for (const auto& entry : std::filesystem::directory_iterator(RELATIVE_PATH)) {
//         if (entry.path().extension() == ".json") {
//             simulations_available.insert(entry.path().filename().string());
//         }
//     }
// }

// vector<string> SimulationFiles::getAvailableSimulations() const
// {
//     return vector<string>(simulations_available.begin(), simulations_available.end());
// }


// vector<PlanetProperties> SimulationFiles::getPlanetsfromJSONFile(const string &simulation_name)
// {
//     PlanetJSONReader reader(RELATIVE_PATH + simulation_name);

//     // map<string, string> jsonData = reader.readJSONFile(RELATIVE_PATH + simulation_name);
//     // auto jsonData = reader.get_data_as_map();
//     // vector<PlanetProperties> planets;
//     // Assuming jsonData contains a list of planet data
//     // Example: jsonData["planets"] is a JSON array of planet objects
//     // You would iterate through this and create PlanetProperties objects
//     // for (const auto& planetData : jsonData["planets"]) {
//     //     planets.push_back(createPlanetFromJSON(planetData));
//     // }
//     return planets;
// }


// SimulationFiles::~SimulationFiles()
// {
// }
