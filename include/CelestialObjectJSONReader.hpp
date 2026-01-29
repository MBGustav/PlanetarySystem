#pragma once

#include <string>
#include <map>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include "CelestialObjectProperties.hpp"
#include "JSONReader.hpp"
#include <glm/glm.hpp>

template <typename T>
class CelestialObjectJSONReader : public JSONReader {
public:
    std::vector<CelestialObjectProperties<T>> planets;
    std::vector<T> axis;
    std::vector<T> eccentricity;
    
    CelestialObjectJSONReader(const std::string &path) : JSONReader(path) {
        parse_planets();
        parse_simulation_settings();
    }
    
    std::vector<CelestialObjectProperties<T>> get_planets() const {
        return planets;
    }

private:
    void parse_planets() {
        auto it = data_map.find("planets");
        if (it != data_map.end()) {
            planets = parse_planets_array(it->second);
        }
    }

    void parse_simulation_settings() {
        auto it = data_map.find("simulation_settings");
        if (it != data_map.end()) {
            std::string settings_str = it->second;
            // Remove colchetes externos se existirem
            if (!settings_str.empty() && settings_str.front() == '[' && settings_str.back() == ']') {
                settings_str = settings_str.substr(1, settings_str.size() - 2);
            }

            // Procura o objeto interno { ... }
            size_t start = settings_str.find('{');
            size_t end   = settings_str.rfind('}');
            if (start != std::string::npos && end != std::string::npos && end > start) {
                std::string obj_str = settings_str.substr(start + 1, end - start - 1);
                std::map<std::string, std::string> settings_map;

                char c;
                std::string key, value;
                std::istringstream file(obj_str);
                
                while (file >> c) {
                    if (c == '"') {
                        std::getline(file, key, '"');
                        file >> c; // :
                        file >> c;
                        if (c == '[') {
                            value.clear();
                            int bracket_count = 1;
                            char arr_char;
                            while (file.get(arr_char) && bracket_count > 0) {
                                if (arr_char == '[') bracket_count++;
                                else if (arr_char == ']') bracket_count--;
                                if (bracket_count > 0) value += arr_char;
                            }
                        } else {
                            value.clear();
                            value += c;
                            while (file.get(c) && !std::isspace(c) && c != ',' && c != '}') {
                                value += c;
                            }
                        }
                        settings_map[key] = value;
                    }
                }

                if (settings_map.count("axis"))        axis        = parse_array(settings_map["axis"]);
                if (settings_map.count("eccentricity")) eccentricity = parse_array(settings_map["eccentricity"]);
            }
        }
    }

    // Parses a comma-separated string into a glm::tvec3<T>
    static glm::tvec3<T> parse_vec3(const std::string &str) {
        glm::tvec3<T> result(0);
        std::stringstream ss(str);
        std::string item;
        int i = 0;
        while (std::getline(ss, item, ',') && i < 3) {
            size_t start = item.find_first_not_of(" \t\n\r");
            size_t end   = item.find_last_not_of(" \t\n\r");
            if (start != std::string::npos && end != std::string::npos)
                item = item.substr(start, end - start + 1);
            if (!item.empty()) {
                result[i] = static_cast<T>(std::stod(item));
                i++;
            }
        }
        return result;
    }

    // Parses a comma-separated string into a std::vector<T>
    static std::vector<T> parse_array(const std::string &str) {
        std::vector<T> result;
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, ',')) {
            size_t start = item.find_first_not_of(" \t\n\r");
            size_t end   = item.find_last_not_of(" \t\n\r");
            if (start != std::string::npos && end != std::string::npos)
                item = item.substr(start, end - start + 1);
            if (!item.empty())
                result.push_back(static_cast<T>(std::stod(item)));
        }
        return result;
    }

    static CelestialObjectProperties<T> parse_planet_object(const std::string &obj_str) {
        CelestialObjectProperties<T> planet;
        std::map<std::string, std::string> temp_map;
        
        char c;
        std::string key, value;
        std::istringstream file(obj_str);
        while (file >> c) {
            if (c == '"') {
                std::getline(file, key, '"');
                file >> c; // :
                file >> c;
                if (c == '"') {
                    std::getline(file, value, '"');
                } else if (c == '[') {
                    value.clear();
                    int bracket_count = 1;
                    char arr_char;
                    while (file.get(arr_char) && bracket_count > 0) {
                        if (arr_char == '[') bracket_count++;
                        else if (arr_char == ']') bracket_count--;
                        if (bracket_count > 0) value += arr_char;
                    }
                } else {
                    value.clear();
                    value += c;
                    while (file.get(c) && !std::isspace(c) && c != ',' && c != '}') {
                        value += c;
                    }
                }
                temp_map[key] = value;
            }
        }

        if(temp_map.count("name"))     planet.set_name(temp_map["name"]);
        if(temp_map.count("mass"))     planet.set_mass(static_cast<T>(std::stod(temp_map["mass"])));
        if(temp_map.count("radius"))   planet.set_radius(static_cast<T>(std::stod(temp_map["radius"])));
        if(temp_map.count("color"))    planet.set_color(parse_vec3(temp_map["color"]));
        if(temp_map.count("position")) planet.set_position(parse_vec3(temp_map["position"]));
        if(temp_map.count("velocity")) planet.set_velocity(parse_vec3(temp_map["velocity"]));
        
        return planet;
    }

public:
    static std::vector<CelestialObjectProperties<T>> parse_planets_array(const std::string &array_str) {
        std::vector<CelestialObjectProperties<T>> planets;
        int brace_count = 0;
        std::string current_obj;
        for (char c : array_str) {
            if (c == '{') brace_count++;
            if (brace_count > 0) current_obj += c;
            if (c == '}') {
                brace_count--;
                if (brace_count == 0 && !current_obj.empty()) {
                    planets.push_back(parse_planet_object(current_obj));
                    current_obj.clear();
                }
            }
        }
        return planets;
    }
    std::vector<T> get_axis() const {
        return axis;
    }
    
    std::vector<T> get_eccentricity() const {
        return eccentricity;
    }
    
};