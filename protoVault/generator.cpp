#include "generator.h"

std::string readFile(const std::string filePath) {
    std::ifstream file(filePath); // Open file
    if (!file) {
        std::cerr << "Error: Could not open file!" << std::endl;
        return "";
    }

    std::ostringstream contentStream;
    contentStream << file.rdbuf(); // Read entire file into stringstream

    file.close(); // Close file
    return contentStream.str(); // Convert stream to string and return
}
