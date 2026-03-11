# JametLang

## Bahasa Pemrograman Basa Jawa

JametLang is a programming language that uses Bahasa Jawa (Javanese language) syntax. Created as a fun and educational project to preserve and promote the use of traditional Indonesian language in programming.

## Jinis Data (Data Types)

| JametLang | Keterangan                      |
| --------- | ------------------------------- |
| `integer` | Nomor bulat (whole numbers)     |
| `float`   | Nomor desimal (decimal numbers) |
| `string`  | Teks (text)                     |
| `boolean` | Bener/Salah (True/False)        |
| `array`   | Kumpulan data (collection)      |
| `kosong`  | Null/None value                 |

## Tembung Kunci (Keywords)

| Bahasa Jawa | English     | Keterangan         |
| ----------- | ----------- | ------------------ |
| `nek`       | if          | Kanggo kondisi     |
| `kajaba`    | else        | Alternatif kondisi |
| `kanggo`    | for         | Loop               |
| `sawise`    | while       | Loop kondisi       |
| `balekno`   | return      | Mbalekno nilai     |
| `nyerat`    | write/print | Nampilake output   |
| `moco`      | read/input  | Maca input         |
| `fungsi`    | function    | Fungsi             |
| `variabel`  | var/let     | Variabel           |
| `bener`     | true        | True               |
| `salah`     | false       | False              |
| `kosong`    | null        | Null value         |
| `pegat`     | break       | Pegat loop         |
| `terusake`  | continue    | Terusake loop      |
| `lan`       | and         | Logika AND         |
| `utawa`     | or          | Logika OR          |
| `ora`       | not         | Logika NOT         |
| `kelas`     | class       | Kelas              |
| `anyar`     | new         | Anyar object       |
| `piwulang`  | extends     | Pewarisan          |

## Operators

| Operator | Keterangan                 |
| -------- | -------------------------- |
| `+`      | Tambah (plus)              |
| `-`      | Kurang (minus)             |
| `*`      | Kali (multiply)            |
| `/`      | Bagi (divide)              |
| `%`      | Modulo                     |
| `==`     | Samadengan (equal)         |
| `!=`     | Beda (not equal)           |
| `>`      | Lebih besik (greater than) |
| `<`      | Lebih ciyut (less than)    |
| `+=`     | Plus sama (add assign)     |
| `-=`     | Minus sama (sub assign)    |

## Komentar (Comments)

```java
// Komentar siji baris (single-line comment)

/* Komentar blok
   bisa pirang-pirang baris
   (multi-line block comment) */
```

## Cara Gawe (Build)

```bash
# Build CLI dan Demo
make

# Build CLI only
make cli

# Build Demo only
make demo
```

## Cara Pasang (Installation)

### Otomatis (Automatic)

```bash
# Jalankan installer
sudo ./install.sh

# Utawa nggunakake make
sudo make install
```

### Manual

```bash
# Build
make cli

# Kopi menyang /usr/local/bin
sudo cp bin/jamet /usr/local/bin/
sudo chmod +x /usr/local/bin/jamet
```

### Uninstall

```bash
# Nganggo script uninstall
sudo ./uninstall.sh

# Utawa nggunakake make
sudo make uninstall
```

## CLI / REPL

Setelah instalasi, jalankan `jamet` saka terminal:

```bash
# Mlebet mode REPL
jamet

# Lakokno file
jamet file.jmt

# Tampilake bantuan
jamet --help

# Tampilake versi
jamet --version
```

### REPL Commands

Ing mode REPL, sampeyan bisa nggunakake perintah khusus:

| Perintah  | Keterangan        |
| --------- | ----------------- |
| `.tulung` | Nampilake bantuan |
| `.metu`   | Metu saka REPL    |
| `.busak`  | Bersihna layar    |
| `.tokens` | Mode token debug  |

### Conto REPL Session

```
$ jamet
    _    _    __  __ _____ _____
   | |  / \  |  \/  | ____|_   _|
_  | | / _ \ | |\/| |  _|   | |
| |_| |/ ___ \| |  | | |___  | |
\___//_/   \_\_|  |_|_____| |_|

    Bahasa Pemrograman Basa Jawa

  Ketik '.tulung' kanggo bantuan
  Ketik '.metu' utawa tekan Ctrl-D kanggo metu

jamet> variabel x = 10
=> x = 10

jamet> x + 5
=> 15

jamet> nyerat("Halo");
Halo

jamet> .metu
Matur nuwun! Sampun jumpa!
```

## Conto Program (Example)

File: `examples/halo.jmt`

```java
/* Conto Program JametLang - Halo Dunia */

variabel pesan = "Halo, Dunia!";
nyerat(pesan);

variabel angka = 42;
nyerat("Nomere: " + angka);

/* Ngitung */
variabel hasil = 10 + 5;
nyerat("10 + 5 = " + hasil);

/* Kondisi */
nek (hasil > 10) {
    nyerat("Hasile luwih gede saka 10");
}

/* Loop */
kanggo (variabel i = 0; i < 5; i = i + 1) {
    nyerat("Iterasi: " + i);
}
```

Jalankan:

```bash
jamet examples/halo.jmt
```

## Struktur Project

```
JametLang/
├── include/
│   ├── banner.h         # Banner ASCII art
│   ├── jamet_types.h    # Jinis data dhasar
│   ├── lexer.h          # Lexer/Tokenizer
│   ├── parser.h         # Parser/Interpreter (AST)
│   └── repl.h           # REPL header
├── src/
│   ├── jamet_types.c    # Implementasi jinis data
│   ├── lexer.c          # Implementasi lexer
│   ├── parser.c         # Implementasi parser & interpreter
│   ├── repl.c           # Implementasi REPL
│   ├── cli.c            # CLI entry point
│   └── main.c           # Demo program
├── examples/
│   ├── halo.jmt         # Conto program dasar
│   ├── test_parser.jmt  # Test parser & interpreter
│   └── test_simple.jmt  # Test sederhana
├── jametlang-vscode/    # VSCode Extension
│   ├── syntaxes/        # TextMate grammar
│   ├── snippets/        # Code snippets
│   └── package.json     # Extension manifest
├── bin/                 # Executable
├── build/               # Object files
├── install.sh           # Installer script
├── uninstall.sh         # Uninstaller script
├── Makefile
└── README.md
```

## Fitur (Features)

- **Komentar** — `//` lan `/* */` (line & block comments)
- **Variabel** — `variabel x = 10;`
- **Fungsi** — `fungsi tambah(a, b) { balekno a + b; }`
- **Kondisi** — `nek` / `kajaba` (if/else)
- **Loop** — `kanggo` (for) lan `sawise` (while)
- **Break/Continue** — `pegat` / `terusake`
- **String Concatenation** — `"Halo " + 42` → `"Halo 42"`
- **Boolean** — `bener` / `salah`
- **Input/Output** — `nyerat()` / `moco()`

## Status Saiki

- [x] Jinis Data Dhasar (Basic Data Types)
- [x] Lexer/Tokenizer
- [x] Komentar (Comments) — `//` lan `/* */`
- [x] Parser (AST)
- [x] Interpreter
- [x] CLI / REPL
- [x] Installer
- [ ] Standard Library
- [ ] Array Operations
- [ ] Class / OOP (`kelas`, `anyar`, `piwulang`)

## VSCode Extension

JametLang duwe extension VSCode kanggo syntax highlighting, snippets, lan language support.

```bash
# Install kanggo development
ln -sf /path/to/JametLang/jametlang-vscode ~/.vscode/extensions/jametlang

# Utawa build VSIX
cd jametlang-vscode
npx @vscode/vsce package
code --install-extension jametlang-0.1.0.vsix
```

Fitur extension:
- **Syntax Highlighting** — Pewarnaan kode kanggo file `.jmt`
- **Code Snippets** — Template cepet (`variabel`, `fungsi`, `kanggo`, `nek`, lsp.)
- **Comment Toggle** — `Ctrl+/` lan `Shift+Alt+A`
- **Auto-Closing** — Brackets, parentheses, quotes
- **Code Folding** — Lempit/bukak blok kode

## Terima Kasih

Matur nuwun sampun nggunakake JametLang - Bahasa Pemrograman Basa Jawa!
