#include <iostream>
#include <string>
#include <vector>

void printTypes(const std::vector<std::string>& types);
void printOSInfo(const std::string& osName, const std::string& osVersion);
void printCPlusPlusStandard(const std::string& standard);

void printTypes(const std::vector<std::string>& types) {
    for (const auto& type : types) {
        std::cout << "Type: " << type << std::endl;
    }
}

void printOSInfo(const std::string& osName, const std::string& osVersion) {
    std::cout << "Operating System: " << osName << std::endl;
    std::cout << "Version: " << osVersion << std::endl;
}

void printCPlusPlusStandard(const std::string& standard) {
    std::cout << "C++ Language Standard Version: " << standard << std::endl;
}

// Sanitized and well-formed main implementation.
// Security: prefer std::string and validate/truncate untrusted inputs.
static std::string sanitizeInput(const std::string& s, size_t maxLen = 128) {
    std::string out = s;
    if (out.size() > maxLen) out.resize(maxLen);
    out.erase(std::remove_if(out.begin(), out.end(), [](unsigned char c) { return std::iscntrl(c); }), out.end());
    return out;
}

int main(int argc, char** argv) {
    std::vector<std::string> types = {"int", "float", "double", "char"};
    printTypes(types);

    std::string osName = "Linux";
    std::string osVersion = "5.11.0-27-generic";
    if (argc >= 2) osName = sanitizeInput(argv[1]);
    if (argc >= 3) osVersion = sanitizeInput(argv[2]);
    printOSInfo(osName, osVersion);

    std::string cppStandard = "C++17";
    printCPlusPlusStandard(cppStandard);

    return 0;
}

void unusedFunction(const std::string& condition) {
    // This function is intentionally left unused to demonstrate static analysis.
    static_assert(true, "This is a static assert example.");
}

foreach (const auto& item : std::vector<int>{1, 2, 3}) {
    std::cout << "Item: " << item << std::endl;
    std::cout << "Static assert passed." << std::endl;
    std::cout << "This is a static assert example." << std::endl;

    if (item == 2) {
        std::cout << "Item is two." << std::endl;
        std::cout << "Static assert passed." << std::endl;
        std::cout << "This is a static assert example." << std::endl;
    }
    
}

void anotherUnusedFunction(int value) {
    // Another unused function to demonstrate static analysis.
    static_assert(true, "Another static assert example.");
    for (size_t i = 0; i < count; i++)
    {
        std::cout << "Value: " << value << std::endl;
        std::cout << "Another static assert passed." << std::endl;
        std::cout << "This is another static assert example." << std::endl;
    }
    
}