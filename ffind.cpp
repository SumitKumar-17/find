/**
 * @find - A blazingly fast, interactive file finder for terminal power users
 */

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>

namespace fs = std::filesystem;

const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string UNDERLINE = "\033[4m";
const std::string HIGHLIGHT = "\033[1;32m"; 

const char BACKSPACE = 127;
const char CTRL_C = 3;
const char ENTER = 10;

char getCharNonBlocking() {
    struct termios oldSettings, newSettings;
    tcgetattr(STDIN_FILENO, &oldSettings);
    newSettings = oldSettings;
    newSettings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
    
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    
    char c = 0;
    if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout) > 0) {
        read(STDIN_FILENO, &c, 1);
    }
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
    return c;
}

void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

bool containsCaseInsensitive(const std::string& str, const std::string& substr) {
    auto it = std::search(
        str.begin(), str.end(),
        substr.begin(), substr.end(),
        [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
    );
    return it != str.end();
}

std::string highlightMatch(const std::string& filename, const std::string& searchTerm) {
    if (searchTerm.empty()) {
        return filename;
    }
    
    std::string result = filename;
    std::string upperFilename = filename;
    std::string upperSearchTerm = searchTerm;
    
    std::transform(upperFilename.begin(), upperFilename.end(), upperFilename.begin(), ::toupper);
    std::transform(upperSearchTerm.begin(), upperSearchTerm.end(), upperSearchTerm.begin(), ::toupper);
    
    size_t pos = upperFilename.find(upperSearchTerm);
    if (pos != std::string::npos) {
        result = filename.substr(0, pos) + 
                 HIGHLIGHT + UNDERLINE + 
                 filename.substr(pos, searchTerm.length()) + 
                 RESET + 
                 filename.substr(pos + searchTerm.length());
    }
    
    return result;
}

std::vector<fs::path> findMatchingFiles(const std::string& searchTerm) {
    std::vector<fs::path> matches;
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(fs::current_path())) {
            if (fs::is_regular_file(entry) || fs::is_directory(entry)) {
                std::string filename = entry.path().filename().string();
                if (containsCaseInsensitive(filename, searchTerm)) {
                    matches.push_back(entry.path());
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error accessing filesystem: " << e.what() << std::endl;
    }
    
    std::sort(matches.begin(), matches.end(), [](const fs::path& a, const fs::path& b) {
        return a.string().length() < b.string().length(); 
        });
    
    return matches;
}

void displayResults(const std::vector<fs::path>& matches, const std::string& searchTerm) {
    clearScreen();
    std::cout << BOLD << "@find " << searchTerm << RESET << "\n\n";
    
    if (matches.empty()) {
        std::cout << "No matches found.\n";
        return;
    }
    
    std::cout << "Found " << matches.size() << " matches:\n";
    
    for (const auto& path : matches) {
        fs::path relativePath = fs::relative(path, fs::current_path());
        std::string displayPath = "./" + relativePath.string();
        
        std::string filename = path.filename().string();
        std::string highlightedFilename = highlightMatch(filename, searchTerm);
        
        size_t pos = displayPath.rfind(filename);
        if (pos != std::string::npos) {
            displayPath = displayPath.substr(0, pos) + highlightedFilename;
        }
        
        std::cout << "  " << displayPath << "\n";
    }
}

int main(int argc, char* argv[]) {
    std::string searchTerm;
    
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            searchTerm += argv[i];
            if (i < argc - 1) {
                searchTerm += " ";
            }
        }
    }
    
    clearScreen();
    std::cout << BOLD << "@find " << searchTerm << RESET << "\n\n";
    std::cout << "Searching... (Press Ctrl+C to exit)\n";
    
    bool running = true;
    while (running) {
        auto matches = findMatchingFiles(searchTerm);
        displayResults(matches, searchTerm);
        
        bool inputReceived = false;
        while (!inputReceived) {
            char c = getCharNonBlocking();
            
            if (c == CTRL_C) {
                running = false;
                inputReceived = true;
            } else if (c == BACKSPACE || c == 8) { 
                // Handle backspace (8 is also backspace on some systems)
                if (!searchTerm.empty()) {
                    searchTerm.pop_back();
                    inputReceived = true;
                }
            } else if (c == ENTER) {
                running = false;
                inputReceived = true;
            } else if (c >= 32 && c <= 126) {
                searchTerm += c;
                inputReceived = true;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    clearScreen();
    
    if (!searchTerm.empty()) {
        auto matches = findMatchingFiles(searchTerm);
        displayResults(matches, searchTerm);
    }
    
    return 0;
}
