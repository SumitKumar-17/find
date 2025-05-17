#!/bin/bash
#
# Installer for @find - A global file finder tool for terminal power users
# This script compiles and installs the @find utility globally on the system

set -e 

GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

# Root privileges
if [ "$EUID" -ne 0 ]; then
  echo -e "${YELLOW}This installer requires root privileges to install @find globally.${NC}"
  echo -e "Please run with sudo: ${BLUE}sudo ./installer.sh${NC}"
  exit 1
fi

echo -e "${BLUE}Checking dependencies...${NC}"

if ! command -v g++ &> /dev/null; then
    echo -e "${RED}Error: g++ is not installed.${NC}"
    echo -e "Please install g++ using: ${YELLOW}pacman -S gcc${NC}"
    exit 1
fi

echo -e "${BLUE}Checking C++17 support...${NC}"
if ! g++ -std=c++17 -E -x c++ - <<<'#include <filesystem>' &> /dev/null; then
    echo -e "${RED}Error: C++17 filesystem library not available.${NC}"
    echo -e "Please ensure your g++ version supports C++17."
    exit 1
fi

echo -e "${BLUE}Compiling @find...${NC}"
g++ -std=c++17 -O3 -Wall -Wextra ffind.cpp -o @find

if [ ! -f "@find" ]; then
    echo -e "${RED}Error: Compilation failed.${NC}"
    exit 1
fi

chmod +x @find

echo -e "${BLUE}Installing @find to /usr/local/bin...${NC}"
cp @find /usr/local/bin/

if [ -f "/usr/local/bin/@find" ] && [ -x "/usr/local/bin/@find" ]; then
    echo -e "${GREEN}@find successfully installed!${NC}"
    echo -e "You can now use ${YELLOW}@find${NC} from anywhere in your terminal."
    echo -e "Try it out with: ${YELLOW}@find <partial-filename>${NC}"
else
    echo -e "${RED}Installation failed. Could not copy @find to /usr/local/bin/${NC}"
    exit 1
fi

# Create uninstaller script
echo -e "${BLUE}Creating uninstaller script...${NC}"
cat > /usr/local/bin/uninstall-@find << 'EOL'
#!/bin/bash
# Uninstaller for @find

if [ "$EUID" -ne 0 ]; then
  echo "Please run with sudo: sudo uninstall-@find"
  exit 1
fi

if [ -f "/usr/local/bin/@find" ]; then
  rm /usr/local/bin/@find
  echo "@find has been uninstalled."
else
  echo "@find is not installed."
fi

# Remove this uninstaller
rm /usr/local/bin/uninstall-@find
EOL

chmod +x /usr/local/bin/uninstall-@find
echo -e "${GREEN}Uninstaller created at /usr/local/bin/uninstall-@find${NC}"

echo -e "\n${GREEN}=========================================${NC}"
echo -e "${GREEN}@find installation complete!${NC}"
echo -e "${GREEN}=========================================${NC}"
echo -e "Usage: ${YELLOW}@find <partial-filename>${NC}"
echo -e "To uninstall: ${YELLOW}sudo uninstall-@find${NC}"
echo -e "\nEnjoy your blazingly fast file searching!\n"
