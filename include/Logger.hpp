#pragma once 

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

enum class LogLevel { DEBUG = 0, WARNING = 1, INFO = 2, SIMULATION = 3, ERROR = 4 };

const LogLevel LEVEL_LOGGER = LogLevel::INFO;

static const std::string  RESET       = "\033[0m"  ;
static const std::string  BLACK       = "\033[30m" ;            /* Black */
static const std::string  RED         = "\033[31m" ;            /* Red */
static const std::string  GREEN       = "\033[32m" ;            /* Green */
static const std::string  YELLOW      = "\033[33m" ;            /* Yellow */
static const std::string  BLUE        = "\033[34m" ;            /* Blue */
static const std::string  MAGENTA     = "\033[35m" ;            /* Magenta */
static const std::string  CYAN        = "\033[36m" ;            /* Cyan */
static const std::string  WHITE       = "\033[37m" ;            /* White */
static const std::string  BOLDBLACK   = "\033[1m\033[30m";      /* Bold Black */
static const std::string  BOLDRED     = "\033[1m\033[31m";      /* Bold Red */
static const std::string  BOLDGREEN   = "\033[1m\033[32m";      /* Bold Green */
static const std::string  BOLDYELLOW  = "\033[1m\033[33m";      /* Bold Yellow */
static const std::string  BOLDBLUE    = "\033[1m\033[34m";      /* Bold Blue */
static const std::string  BOLDMAGENTA = "\033[1m\033[35m";      /* Bold Magenta */
static const std::string  BOLDCYAN    = "\033[1m\033[36m";      /* Bold Cyan */
static const std::string  BOLDWHITE   = "\033[1m\033[37m";      /* Bold White */

std::string logLevelToString(LogLevel level) {
    switch(level) {
        case LogLevel::DEBUG:      return "DEBUG";
        case LogLevel::WARNING:    return "WARNING";
        case LogLevel::INFO:       return "INFO";
        case LogLevel::SIMULATION: return "SIMULATION";
        case LogLevel::ERROR:      return "ERROR";
        default:                   return "UNKNOWN";
    }
}

class Logger {
    private:
    std::string filename;
    bool with_time, with_date;
    LogLevel log_level;
    
    std::stringstream simulation_buffer;  
    
    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        if(with_date) ss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d") << "] ";
        if(with_time) ss << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "]";
        
        return ss.str();
    }
    
    std::string beautify(std::string message, std::string color) 
    {
        return std::string(color + message + RESET);
    }

    void log(LogLevel level_enum, const std::string message) {
        if(level_enum < log_level) return;
        
        std::stringstream entry;
        
        entry << getCurrentTime()
        << "[" << logLevelToString(level_enum) << "] "
        << message << "\n";
        
        std::cout << entry.str();                         // console
        if(level_enum == LogLevel::SIMULATION) simulation_buffer << entry.str();  // buffer
    }
    
    public:
    Logger(const std::string& filename, bool with_time = true, bool with_date = true, LogLevel level = LogLevel::INFO) : 
    filename(filename), with_time(with_time), with_date(with_date), log_level(level) {}
    
    void set_log_level(LogLevel level) {
        log_level = level;
    }
    
    void info(const std::string& message) {
        log(LogLevel::INFO, beautify(message, BOLDBLUE));
    }
    
    void error(const std::string& message) {
        log(LogLevel::ERROR, beautify(message, BOLDRED));
    }
    
    void warn(const std::string& message) {
        log(LogLevel::WARNING, beautify(message, BOLDYELLOW));
    }
    
    void debug(const std::string& message) {
        log(LogLevel::DEBUG, beautify(message, BOLDWHITE));
    }
    
    void simulation(const std::string& message) {
        log(LogLevel::SIMULATION, message);
    }

    std::string get_log_buffer() {
        return simulation_buffer.str();
    }
};


// Global Logger Instance
static Logger sys_logger("Application", true, true, LEVEL_LOGGER);
