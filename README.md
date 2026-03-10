# JametLang
## Bahasa Pemrograman Basa Jawa

JametLang is a programming language that uses Bahasa Jawa (Javanese language) syntax. Created as a fun and educational project to preserve and promote the use of traditional Indonesian language in programming.

## Jinis Data (Data Types)

| JametLang | Keterangan |
|-----------|------------|
| `integer` | Nomor bulat (whole numbers) |
| `float` | Nomor desimal (decimal numbers) |
| `string` | Teks (text) |
| `boolean` | Bener/Salah (True/False) |
| `array` | Kumpulan data (collection) |
| `kosong` | Null/None value |

## Tembung Kunci (Keywords)

| Bahasa Jawa | English | Keterangan |
|-------------|---------|------------|
| `nek` | if | Kanggo kondisi |
| `kajaba` | else | Alternatif kondisi |
| `kanggo` | for | Loop |
| `sawise` | while | Loop kondisi |
| `balekno` | return | Mbalekno nilai |
| `nyerat` | write/print | Nampilake output |
| `moco` | read/input | Maca input |
| `fungsi` | function | Fungsi |
| `variabel` | var/let | Variabel |
| `bener` | true | True |
| `salah` | false | False |
| `kosong` | null | Null value |
| `pegat` | break | Pegat loop |
| `terusake` | continue | Terusake loop |
| `lan` | and | Logika AND |
| `utawa` | or | Logika OR |
| `ora` | not | Logika NOT |
| `kelas` | class | Kelas |
| `anyar` | new | Anyar object |
| `piwulang` | extends | Pewarisan |

## Operators

| Operator | Keterangan |
|----------|------------|
| `+` | Tambah (plus) |
| `-` | Kurang (minus) |
| `*` | Kali (multiply) |
| `/` | Bagi (divide) |
| `%` | Modulo |
| `==` | Samadengan (equal) |
| `!=` | Beda (not equal) |
| `>` | Lebih besik (greater than) |
| `<` | Lebih ciyut (less than) |
| `+=` | Plus sama (add assign) |
| `-=` | Minus sama (sub assign) |

## Cara Gawe (Build)

```bash
# Build CLI dan Demo
make

# Build CLI only
make cli

# Build Demo only
make demo
```

## Cara Instalasi (Installation)

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

# Copy ke /usr/local/bin
sudo cp bin/jamet /usr/local/bin/
sudo chmod +x /usr/local/bin/jamet
```

### Uninstall

```bash
# Menggunakan script uninstall
sudo ./uninstall.sh

# Utawa nggunakake make
sudo make uninstall
```

## CLI / REPL

Setelah instalasi, jalankan `jamet` saka terminal:

```bash
# Mlebet mode REPL
jamet

# Jalankan file
jamet file.jmt

# Tampilkan bantuan
jamet --help

# Tampilkan versi
jamet --version
```

### REPL Commands

Ing mode REPL, sampeyan bisa nggunakake perintah khusus:

| Perintah | Keterangan |
|----------|------------|
| `.help` | Nampilake bantuan |
| `.exit` | Metu saka REPL |
| `.clear` | Bersihna layar |
| `.tokens` | Mode token debug |

### Conto REPL Session

```
$ jamet
   __  __          _____   _____
  |  \/  |   /\   |  __ \ / ____|
  | \  / |  /  \  | |__) | |
  | |\/| | / /\ \ |  _  /| |
  | |  | |/ ____ \| | \ \| |____
  |_|  |_/_/    \_\_|  \_\_____|

  Ketik '.help' kanggo bantuan
  Ketik '.exit' utawa tekan Ctrl-D kanggo metu

jamet> variabel x = 10
=> Sukses tokenizing (4 token)

jamet> nek (x > 5) { nyerat("Gede"); }
=> Sukses tokenizing (9 token)

jamet> .exit
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
│   ├── jamet_types.h    # Jinis data dhasar
│   ├── lexer.h          # Lexer/Tokenizer
│   └── repl.h           # REPL header
├── src/
│   ├── jamet_types.c    # Implementasi jinis data
│   ├── lexer.c          # Implementasi lexer
│   ├── repl.c           # Implementasi REPL
│   ├── cli.c            # CLI entry point
│   └── main.c           # Demo program
├── examples/
│   └── halo.jmt         # Conto program
├── bin/                 # Executable
├── build/               # Object files
├── install.sh           # Installer script
├── uninstall.sh         # Uninstaller script
├── Makefile
└── README.md
```

## Status Saiki

- [x] Jinis Data Dhasar (Basic Data Types)
- [x] Lexer/Tokenizer
- [x] CLI / REPL
- [x] Installer
- [ ] Parser (AST)
- [ ] Interpreter
- [ ] Standard Library

## Terima Kasih

Matur nuwun sampun nggunakake JametLang - Bahasa Pemrograman Basa Jawa!
