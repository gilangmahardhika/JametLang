#!/bin/bash
# JametLang Uninstaller Script

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo -e "${YELLOW}Nggunakake sudo kanggo uninstall...${NC}"
    exec sudo bash "$0" "$@"
fi

echo -e "${YELLOW}[*] Uninstal JametLang...${NC}"

# Remove binaries
if [ -f "/usr/local/bin/jamet" ]; then
    rm -f "/usr/local/bin/jamet"
    echo -e "${GREEN}[+] Dihapus: /usr/local/bin/jamet${NC}"
fi

if [ -f "/usr/local/bin/jametlang" ]; then
    rm -f "/usr/local/bin/jametlang"
    echo -e "${GREEN}[+] Dihapus: /usr/local/bin/jametlang${NC}"
fi

# Remove library
if [ -d "/usr/local/lib/jametlang" ]; then
    rm -rf "/usr/local/lib/jametlang"
    echo -e "${GREEN}[+] Dihapus: /usr/local/lib/jametlang${NC}"
fi

# Remove man page
if [ -f "/usr/local/share/man/man1/jamet.1.gz" ]; then
    rm -f "/usr/local/share/man/man1/jamet.1.gz"
    echo -e "${GREEN}[+] Dihapus: man page${NC}"
fi

echo ""
echo -e "${GREEN}[!] JametLang wis diuninstal sakabehane!${NC}"
echo "Matur nuwun sampun nggunakake JametLang!"
