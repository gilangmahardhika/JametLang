#!/bin/bash
# JametLang Installer Script
# Bahasa Pemrograman Basa Jawa

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print banner
echo -e "${BLUE}"
echo "     _    _    __  __ _____ _____ "
echo "    | |  / \  |  \/  | ____|_   _|"
echo " _  | | / _ \ | |\/| |  _|   | |  "
echo "| |_| |/ ___ \| |  | | |___  | |  "
echo " \___//_/   \_\_|  |_|_____| |_|  "
echo "                                          "
echo "    Bahasa Pemrograman Basa Jawa          "
echo -e "${NC}"

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo -e "${YELLOW}Nggunakake sudo kanggo instalasi...${NC}"
    exec sudo bash "$0" "$@"
fi

# Installation directory
INSTALL_DIR="/usr/local/bin"
LIB_DIR="/usr/local/lib/jametlang"
MAN_DIR="/usr/local/share/man/man1"

echo -e "${GREEN}[*] Mulai instalasi JametLang...${NC}"

# Create directories
echo -e "${YELLOW}[*] Mbangun direktori...${NC}"
mkdir -p "$LIB_DIR"
mkdir -p "$MAN_DIR"

# Build the project
echo -e "${YELLOW}[*] Compile JametLang...${NC}"
if [ -f "Makefile" ]; then
    make clean > /dev/null 2>&1 || true
    make cli
else
    echo -e "${RED}[!] Makefile ora ditemukna!${NC}"
    echo "Jalanake saka direktori source JametLang."
    exit 1
fi

# Install binary
echo -e "${YELLOW}[*] Instal binary...${NC}"
cp bin/jamet "$INSTALL_DIR/jamet"
chmod +x "$INSTALL_DIR/jamet"

# Install library files
echo -e "${YELLOW}[*] Instal library...${NC}"
cp -r include "$LIB_DIR/"
cp -r src "$LIB_DIR/"

# Create symlink for jametlang
ln -sf "$INSTALL_DIR/jamet" "$INSTALL_DIR/jametlang"

# Create man page (basic)
echo -e "${YELLOW}[*] Instal man page...${NC}"
cat > "$MAN_DIR/jamet.1" << 'EOF'
.TH JAMET 1 "JametLang Manual"
.SH NAME
jamet \- Bahasa Pemrograman Basa Jawa
.SH SYNOPSIS
.B jamet
[OPSI] [FILE]
.SH DESCRIPTION
JametLang adalah bahasa pemrograman yang menggunakan Bahasa Jawa (Javanese language) syntax.
.SH OPTIONS
.TP
\fB\-h, --help\fR
Tampilkan bantuan
.TP
\fB\-v, --version\fR
Tampilkan versi
.TP
\fB\-r, --repl\fR
Masuk mode REPL
.TP
\fB\fIFILE\fR
Jalankan file JametLang
.SH EXAMPLES
jamet
.RS
Masuk mode REPL
.RE
.PP
jamet program.jmt
.RS
Jalankan file program.jmt
.RE
.SH AUTHOR
JametLang Project
.SH SEE ALSO
JametLang GitHub Repository
EOF

gzip -f "$MAN_DIR/jamet.1"

echo -e "${GREEN}[!] Instalasi Sampun!${NC}"
echo ""
echo -e "${BLUE}JametLang wis diinstal ing sistem sampeyan!${NC}"
echo ""
echo "Perintah sing tersedia:"
echo -e "  - ${GREEN}jamet${NC}      : Jalankan JametLang CLI/REPL"
echo -e "  - ${GREEN}jametlang${NC}  : Alias kanggo jamet"
echo ""
echo "Conto penggunaan:"
echo -e "  ${YELLOW}jamet${NC}              - Mlebet mode REPL"
echo -e "  ${YELLOW}jamet file.jmt${NC}     - Jalankan file"
echo -e "  ${YELLOW}man jamet${NC}          - Deleng manual"
echo ""
echo "Kanggo uninstall, jalankan:"
echo -e "  ${YELLOW}sudo make uninstall${NC}"
echo "  utawa"
echo -e "  ${YELLOW}sudo /usr/local/lib/jametlang/uninstall.sh${NC}"
echo ""
