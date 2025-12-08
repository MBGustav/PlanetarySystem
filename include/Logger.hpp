#pragma once 

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>


class Logger {
private:
    std::string name;
    
    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void log(const std::string& level, const std::string& message) {
        std::cout << getCurrentTime() << " - " << name << " - " 
                  << level << " - " << message << std::endl;
    }
    
public:
    Logger(const std::string& name) : name(name) {}
    
    void info(const std::string& message) {
        log("INFO", message);
    }
    
    void error(const std::string& message) {
        log("ERROR", message);
    }
    
    void warning(const std::string& message) {
        log("WARNING", message);
    }
    
    void debug(const std::string& message) {
        log("DEBUG", message);
    }
};

// Glbal Logger Instance
static Logger sys_logger("Application");
