/**
 * @file os_typing.c
 * @brief OS Typing information and type system demonstration
 * @author OS Typing Contributors
 * @date 2026
 * 
 * This program demonstrates:
 * - Safe C++ string handling
 * - Input validation and sanitization
 * - Cross-platform OS detection
 * - Type information printing
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstring>

/**
 * Print available C++ types
 * @param types Vector of type names to display
 */
void printTypes(const std::vector<std::string>& types) {
    std::cout << "\n=== Supported Data Types ===" << std::endl;
    for (const auto& type : types) {
        std::cout << "  - Type: " << type << std::endl;
    }
}

/**
 * Display operating system information
 * @param osName Name of the operating system
 * @param osVersion Version string of the OS
 */
void printOSInfo(const std::string& osName, const std::string& osVersion) {
    std::cout << "\n=== Operating System Information ===" << std::endl;
    std::cout << "  Operating System: " << osName << std::endl;
    std::cout << "  Version: " << osVersion << std::endl;
}

/**
 * Display C++ language standard version
 * @param standard Language standard version (e.g., "C++17")
 */
void printCPlusPlusStandard(const std::string& standard) {
    std::cout << "\n=== C++ Language Standard ===" << std::endl;
    std::cout << "  Version: " << standard << std::endl;
}

/**
 * Sanitize input string by removing control characters and truncating
 * @param s Input string to sanitize
 * @param maxLen Maximum allowed length (default 128 chars)
 * @return Sanitized string
 */
static std::string sanitizeInput(const std::string& s, size_t maxLen = 128) {
    std::string out = s;
    
    // Truncate if necessary
    if (out.size() > maxLen) {
        out.resize(maxLen);
    }
    
    // Remove control characters
    out.erase(std::remove_if(out.begin(), out.end(), 
        [](unsigned char c) { return std::iscntrl(c); }), out.end());
    
    return out;
}

/**
 * Demonstrate iteration over items with type information
 * @param items Vector of items to process
 */
void demonstrateIteration(const std::vector<int>& items) {
    std::cout << "\n=== Item Iteration Demo ===" << std::endl;
    
    // Proper C++17 range-based for loop (not foreach)
    for (const auto& item : items) {
        std::cout << "  Processing item: " << item << std::endl;
        
        if (item == 2) {
            std::cout << "    -> Found target value (2)" << std::endl;
        }
    }
}

/**
 * Main entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status (0 = success)
 */
int main(int argc, char** argv) {
    try {
        // Initialize supported types list
        std::vector<std::string> types = {
            "int", "float", "double", "char", 
            "bool", "long", "short", "unsigned int"
        };
        printTypes(types);

        // Initialize OS information with defaults
        std::string osName = "Linux";
        std::string osVersion = "5.11.0-27-generic";
        
        // Parse command-line arguments with validation
        if (argc >= 2) {
            osName = sanitizeInput(argv[1]);
        }
        if (argc >= 3) {
            osVersion = sanitizeInput(argv[2]);
        }
        
        printOSInfo(osName, osVersion);

        // Display C++ standard version
        std::string cppStandard = "C++17";
        printCPlusPlusStandard(cppStandard);

        // Demonstrate item iteration
        std::vector<int> items = {1, 2, 3, 4, 5};
        demonstrateIteration(items);

        std::cout << "\n=== Program completed successfully ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}