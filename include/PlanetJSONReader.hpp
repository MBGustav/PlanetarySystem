#include <string>
#include <map>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include "PlanetProperties.hpp"
#include "JSONReader.hpp"

class PlanetJSONReader : private JSONReader {
public:
    std::vector<PlanetProperties> planets;

    PlanetJSONReader(const std::string &path) : JSONReader(path) {}

    std::vector<PlanetProperties> get_planets() const {
        return planets;
    }

    // Lê o JSON e popula o vetor planets
    void parse_planets() {
        parse_content(); // usa método genérico da base

        auto it = data_map.find("planets");
        if (it != data_map.end()) {
            planets = parse_planets_array(it->second);
        }
    }

private:
    // Converte string de array para vector<double>
    static std::vector<double> parse_array(const std::string &str) {
        std::vector<double> result;
        std::stringstream ss(str);
        std::string item;

        while (std::getline(ss, item, ',')) {
            size_t start = item.find_first_not_of(" \t\n\r");
            size_t end = item.find_last_not_of(" \t\n\r");
            if (start != std::string::npos && end != std::string::npos)
                item = item.substr(start, end - start + 1);
            if (!item.empty())
                result.push_back(std::stod(item));
        }

        return result;
    }

    // Converte string de objeto planeta em struct Planet
    static PlanetProperties parse_planet_object(const std::string &obj_str) {
        PlanetProperties planet;
        std::map<std::string, std::string> data_map;

        char c;
        std::string key, value;

        auto to_lower = [](std::string &str) {
            std::transform(str.begin(), str.end(), str.begin(),
                           [](unsigned char c){ return std::tolower(c); });
        };

        std::istringstream file(obj_str);
        while (file >> c) {
            if (c == '"') {
                std::getline(file, key, '"');
                file >> c; // :
                file >> c; // próximo char do valor

                if (c == '"') { // string
                    std::getline(file, value, '"');
                } else if (c == '[') { // array
                    value.clear();
                    int bracket_count = 1;
                    char arr_char;
                    while (file.get(arr_char) && bracket_count > 0) {
                        if (arr_char == '[') bracket_count++;
                        else if (arr_char == ']') bracket_count--;
                        if (bracket_count > 0) value += arr_char;
                    }
                } else { // número
                    value.clear();
                    while (!std::isspace(c) && c != ',' && c != '}') {
                        value += c;
                        file.get(c);
                    }
                }

                to_lower(key);
                data_map[key] = value;
            }
        }

        // Fill with planet prop.
        planet.set_name(data_map["name"]) ;
        planet.set_mass(std::stod(data_map["mass"])) ;
        planet.set_radius(std::stod(data_map["radius"])) ;
        planet.set_position(parse_array(data_map["position"])) ;
        planet.set_velocity(parse_array(data_map["velocity"])) ;

        return planet;
    }

    // Converte array de objetos em vector<Planet>
    static std::vector<Planet> parse_planets_array(const std::string &array_str) {
        std::vector<Planet> planets;
        int brace_count = 0;
        std::string current_obj;

        for (char c : array_str) {
            if (c == '{') brace_count++;
            if (brace_count > 0) current_obj += c;
            if (c == '}') {
                brace_count--;
                if (brace_count == 0) {
                    planets.push_back(parse_planet_object(current_obj));
                    current_obj.clear();
                }
            }
        }

        return planets;
    }
};