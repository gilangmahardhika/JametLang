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

## Conto Program (Example)

```java
/* Program Nitip Barang - Conto Bahasa JametLang */

fungsi nitipBarang(variabel barang, variabel jumlah) {
    nek (barang == kosong) {
        balekno "Ora ana barang sing arep dititipne!";
    }

    variabel total = 0;
    kanggo (variabel i = 0; i < jumlah; i = i + 1) {
        total = total + 1000;
        nyerat("Nitipanke barang nomer: " + i);
    }

    balekno total;
}

variabel hasil = nitipBarang("Keripik", 5);
nyerat("Total biaya: " + hasil);

/* Logika kondisi */
nek (hasil > 5000 lan hasil < 10000) {
    nyerat("Diskon 10%");
} kajaba nek (hasil >= 10000) {
    nyerat("Diskon 20%");
} kajaba {
    nyerat("Ora ana diskon");
}
```

## Struktur Project

```
JametLang/
├── include/
│   ├── jamet_types.h    # Jinis data dhasar
│   └── lexer.h          # Lexer/Tokenizer
├── src/
│   ├── jamet_types.c    # Implementasi jinis data
│   ├── lexer.c          # Implementasi lexer
│   └── main.c           # Demo program
├── bin/                 # Executable
├── build/               # Object files
├── Makefile
└── README.md
```

## Cara Gawe (Build)

```bash
# Build project
make

# Run program
./bin/jametlang

# Clean build
make clean

# Rebuild
make rebuild
```

## Status Saiki

- [x] Jinis Data Dhasar (Basic Data Types)
- [x] Lexer/Tokenizer
- [ ] Parser (AST)
- [ ] Interpreter
- [ ] Standard Library
- [ ] REPL

## Terima Kasih

Matur nuwun sampun nggunakake JametLang - Bahasa Pemrograman Basa Jawa!
