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
| `map`     | HashMap/Dictionary (key-value)  |
| `fungsi`  | Lambda/Function reference       |
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
| `jancuk`    | try         | Exception handling |
| `awas`      | catch       | Tangkep exception  |
| `rampungke` | finally     | Blok pungkasan     |
| `uncal`     | throw       | Lempar exception   |
| `saben`     | forEach     | Iterasi array/map  |
| `jupuk`     | import      | Import modul       |
| `kirim`     | export      | Export fungsi      |

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
| `>=`     | Lebih sama (greater equal) |
| `<=`     | Kurang sama (less equal)   |
| `+=`     | Plus sama (add assign)     |
| `-=`     | Minus sama (sub assign)    |
| `++`     | Increment                  |
| `--`     | Decrement                  |

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
│   ├── stdlib_jamet.h   # Standard Library header
│   └── repl.h           # REPL header
├── src/
│   ├── jamet_types.c    # Implementasi jinis data
│   ├── lexer.c          # Implementasi lexer
│   ├── parser.c         # Implementasi parser & interpreter
│   ├── stdlib_jamet.c   # Implementasi standard library
│   ├── repl.c           # Implementasi REPL
│   ├── cli.c            # CLI entry point
│   └── main.c           # Demo program
├── examples/
│   ├── halo.jmt         # Conto program dasar
│   ├── test_parser.jmt  # Test parser & interpreter
│   ├── test_stdlib.jmt  # Test standard library
│   ├── test_array.jmt   # Test array operations
│   ├── test_exception.jmt # Test exception handling
│   ├── test_features.jmt  # Test new features (operators, lambda, map, etc.)
│   ├── test_module.jmt    # Test module system (jupuk/kirim)
│   ├── math_module.jmt    # Example module file
│   ├── test_advanced.jmt  # Test datetime, crypto, JSON, CSV, concurrency
│   └── test_simple.jmt    # Test sederhana
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
- **Standard Library** — Fungsi bawaan kanggo string, matematika, konversi, lan utilitas
- **Array** — `[1, 2, 3]`, akses `arr[i]`, assignment `arr[i] = val`, negative index
- **Exception Handling** — `jancuk`/`awas`/`rampungke` (try/catch/finally), `uncal` (throw)
- **Comparison** — `>=` (lebih sama) lan `<=` (kurang sama)
- **Increment/Decrement** — `x++` lan `x--`
- **forEach** — `saben (variabel item : arr) { ... }`
- **Lambda** — `variabel fn = fungsi(x) { balekno x * 2; };`
- **HashMap/Map** — `{"key": value}`, akses `map["key"]`, iterasi karo `saben`
- **File I/O** — `baca_file`, `tulis_file`, `tambah_file`, `ana_file`
- **HTTP Client** — `http_get(url)`, `http_post(url, body)`
- **Module System** — `jupuk` (import) / `kirim` (export)
- **String Escape** — `\n`, `\t`, `\r`, `\"`, `\\`
- **DateTime** — `tanggal_saiki`, `format_tanggal`, `parsing_tanggal`, `wektu_mili`, `turu`, `zona_wektu`
- **Concurrency** — `gawe_tugas`, `enteni_tugas`, `kunci_mutex`, `buka_mutex`
- **Networking** — `soket_tcp`, `soket_kirim`, `soket_tampa`, `soket_server`, `soket_terima`
- **Kriptografi** — `hash_md5`, `hash_sha256`, `hash_sha512`, `hmac_sha256`, `base64_encode/decode`
- **Serialisasi** — `json_parsing`, `json_format`, `csv_parsing`, `csv_format`

## Standard Library

### Fungsi String

| Fungsi | Keterangan | Conto |
| ------ | ---------- | ----- |
| `panjang(str)` | Panjang string/array | `panjang("Halo")` → `4` |
| `potong(str, mulai, akhir)` | Potong substring | `potong("Halo Dunia", 0, 4)` → `"Halo"` |
| `huruf_gedhe(str)` | Huruf gedhe (uppercase) | `huruf_gedhe("halo")` → `"HALO"` |
| `huruf_cilik(str)` | Huruf cilik (lowercase) | `huruf_cilik("HALO")` → `"halo"` |
| `ganti(str, lama, anyar)` | Ganti substring | `ganti("Halo", "Halo", "Hi")` → `"Hi"` |
| `ngandhut(str, sub)` | Cek ngandhut substring | `ngandhut("Halo", "al")` → `bener` |
| `pisah(str, delimiter)` | Pisah string dadi array | `pisah("a,b,c", ",")` → `["a","b","c"]` |

### Fungsi Matematika

| Fungsi | Keterangan | Conto |
| ------ | ---------- | ----- |
| `mutlak(n)` | Nilai mutlak (absolut) | `mutlak(-42)` → `42` |
| `pangkat(a, b)` | Pangkat (power) | `pangkat(2, 10)` → `1024` |
| `akar(n)` | Akar kuadrat (sqrt) | `akar(144)` → `12` |
| `acak(min, max)` | Angka acak (random) | `acak(1, 100)` → `42` |
| `bulatke(n)` | Bulatke (round) | `bulatke(3.7)` → `4` |
| `lantai(n)` | Lantai (floor) | `lantai(3.9)` → `3` |
| `langit(n)` | Langit-langit (ceil) | `langit(3.1)` → `4` |
| `sisa(a, b)` | Sisa bagi (fmod) | `sisa(10.5, 3)` → `1.5` |

### Konversi Jinis (Type Conversion)

| Fungsi | Keterangan | Conto |
| ------ | ---------- | ----- |
| `dadi_angka(val)` | Konversi dadi integer | `dadi_angka("123")` → `123` |
| `dadi_desimal(val)` | Konversi dadi float | `dadi_desimal(42)` → `42.0` |
| `dadi_teks(val)` | Konversi dadi string | `dadi_teks(42)` → `"42"` |
| `jinis(val)` | Entuk jeneng jinis | `jinis(42)` → `"integer"` |

### Fungsi Array

| Fungsi | Keterangan | Conto |
| ------ | ---------- | ----- |
| `tambah(arr, elem)` | Tambah elemen ing mburi | `tambah([1,2], 3)` → `[1,2,3]` |
| `hapus(arr, idx)` | Hapus elemen ing indeks | `hapus([1,2,3], 1)` → `[1,3]` |
| `gabung(arr1, arr2)` | Gabung loro array | `gabung([1,2], [3,4])` → `[1,2,3,4]` |
| `balik(arr)` | Balik urutan array | `balik([1,2,3])` → `[3,2,1]` |
| `urutke(arr)` | Urutke array (ascending) | `urutke([3,1,2])` → `[1,2,3]` |
| `irisan(arr, mulai, akhir)` | Potong array (slice) | `irisan([1,2,3,4], 1, 3)` → `[2,3]` |
| `indeks(arr, val)` | Golek indeks elemen | `indeks([10,20,30], 20)` → `1` |

### Fungsi Map

| Fungsi | Keterangan | Conto |
| ------ | ---------- | ----- |
| `kunci_map(map)` | Entuk kabeh kunci | `kunci_map(m)` → `["a", "b"]` |
| `nilai_map(map)` | Entuk kabeh nilai | `nilai_map(m)` → `[1, 2]` |
| `ana_kunci(map, key)` | Cek kunci ana | `ana_kunci(m, "a")` → `bener` |

### File I/O

| Fungsi | Keterangan | Conto |
| ------ | ---------- | ----- |
| `baca_file(path)` | Maca file dadi string | `baca_file("data.txt")` |
| `tulis_file(path, isi)` | Tulis file (overwrite) | `tulis_file("out.txt", "halo")` |
| `tambah_file(path, isi)` | Tambah ing file (append) | `tambah_file("log.txt", "baris")` |
| `ana_file(path)` | Cek file ana | `ana_file("data.txt")` → `bener` |

### HTTP Client

| Fungsi | Keterangan | Conto |
| ------ | ---------- | ----- |
| `http_get(url)` | HTTP GET request | `http_get("https://api.example.com")` |
| `http_post(url, body, type)` | HTTP POST request | `http_post(url, data, "application/json")` |

### DateTime (Tanggal & Waktu)

| Fungsi | Keterangan | Conto |
| ------ | ---------- | ----- |
| `tanggal_saiki()` | Map tanggal saiki | `tanggal_saiki()` → `{taun: 2026, ...}` |
| `format_tanggal(fmt, ts?)` | Format tanggal karo strftime | `format_tanggal("%Y-%m-%d")` |
| `parsing_tanggal(str, fmt)` | Parse string dadi timestamp | `parsing_tanggal("2025-01-15", "%Y-%m-%d")` |
| `wektu_mili()` | Wektu saiki ing milidetik | `wektu_mili()` → `1773231335131` |
| `turu(ms)` | Sleep milidetik | `turu(1000)` — turu 1 detik |
| `zona_wektu()` | Zona wektu saiki | `zona_wektu()` → `"WIB (UTC+0700)"` |
| `beda_wektu(ts1, ts2)` | Beda antarane 2 timestamp | `beda_wektu(ts2, ts1)` → detik |

### Concurrency (Tugas & Mutex)

| Fungsi | Keterangan | Conto |
| ------ | ---------- | ----- |
| `gawe_tugas()` | Gawe tugas background | `gawe_tugas()` → task ID |
| `enteni_tugas(id)` | Enteni tugas rampung | `enteni_tugas(0)` |
| `tugas_rampung(id)` | Cek tugas wis rampung | `tugas_rampung(0)` → `bener` |
| `kunci_mutex()` | Kunci mutex | `kunci_mutex()` |
| `buka_mutex()` | Buka mutex | `buka_mutex()` |

### Networking (Socket)

| Fungsi | Keterangan | Conto |
| ------ | ---------- | ----- |
| `soket_tcp(host, port)` | Sambung TCP | `soket_tcp("localhost", 8080)` |
| `soket_kirim(fd, data)` | Kirim data | `soket_kirim(fd, "GET / HTTP/1.1\r\n\r\n")` |
| `soket_tampa(fd, max?)` | Tampa data | `soket_tampa(fd, 4096)` |
| `soket_tutup(fd)` | Tutup soket | `soket_tutup(fd)` |
| `soket_server(port)` | Gawe server TCP | `soket_server(8080)` |
| `soket_terima(server_fd)` | Terima koneksi | `soket_terima(server_fd)` |

### Kriptografi & Hashing

| Fungsi | Keterangan | Conto |
| ------ | ---------- | ----- |
| `hash_md5(str)` | Hash MD5 | `hash_md5("halo")` |
| `hash_sha1(str)` | Hash SHA1 | `hash_sha1("halo")` |
| `hash_sha256(str)` | Hash SHA256 | `hash_sha256("halo")` |
| `hash_sha512(str)` | Hash SHA512 | `hash_sha512("halo")` |
| `hmac_sha256(key, data)` | HMAC-SHA256 | `hmac_sha256("kunci", "data")` |
| `base64_encode(str)` | Encode Base64 | `base64_encode("Halo")` → `"SGFsbw=="` |
| `base64_decode(str)` | Decode Base64 | `base64_decode("SGFsbw==")` → `"Halo"` |

### Serialisasi (JSON & CSV)

| Fungsi | Keterangan | Conto |
| ------ | ---------- | ----- |
| `json_parsing(str)` | Parse JSON dadi value | `json_parsing("{\"a\":1}")` → map |
| `json_format(value)` | Format value dadi JSON | `json_format(map)` → string |
| `csv_parsing(str, delim?)` | Parse CSV dadi array | `csv_parsing("a,b\n1,2")` |
| `csv_format(arr, delim?)` | Format array dadi CSV | `csv_format(data)` |

### Utilitas

| Fungsi | Keterangan | Conto |
| ------ | ---------- | ----- |
| `wektu()` | Wektu saiki (detik epoch) | `wektu()` → `1773228105` |

## Array Operations

```jmt
// Array literal
variabel angka = [1, 2, 3, 4, 5];
variabel kosong = [];

// Akses elemen (0-indexed, support negative index)
nyerat(angka[0]);   // 1
nyerat(angka[-1]);  // 5

// Assignment elemen
angka[0] = 10;

// String indexing
variabel teks = "Halo";
nyerat(teks[0]);    // H

// Nggawe array ing loop
variabel arr = [];
kanggo (variabel i = 0; i < 5; i = i + 1) {
    arr = tambah(arr, i);
}
```

## Operator Anyar

```jmt
// >= dan <=
nyerat(5 >= 3);    // bener
nyerat(3 <= 5);    // bener

// ++ dan --
variabel x = 10;
x++;               // x = 11
x--;               // x = 10

// Ing loop
kanggo (variabel i = 0; i < 5; i++) {
    nyerat(i);
}
```

## forEach (saben)

```jmt
// Iterasi array
variabel arr = [10, 20, 30];
saben (variabel item : arr) {
    nyerat(item);
}

// Iterasi map (kunci)
variabel m = {"a": 1, "b": 2};
saben (variabel kunci : m) {
    nyerat(kunci + " = " + m[kunci]);
}
```

## Lambda Functions

```jmt
// Lambda sederhana
variabel kali_dua = fungsi(x) {
    balekno x * 2;
};
nyerat(kali_dua(5));   // 10

// Lambda karo pirang-pirang parameter
variabel jumlah = fungsi(a, b) {
    balekno a + b;
};
nyerat(jumlah(3, 4));  // 7
```

## HashMap / Map

```jmt
// Map literal
variabel orang = {
    "nama": "Budi",
    "umur": 25,
    "kota": "Surabaya"
};

// Akses karo kunci
nyerat(orang["nama"]);   // Budi

// Assignment kunci
orang["umur"] = 26;

// Map helpers
nyerat(kunci_map(orang));          // ["nama", "umur", "kota"]
nyerat(ana_kunci(orang, "nama"));  // bener
nyerat(panjang(orang));            // 3
```

## File I/O

```jmt
// Tulis file
tulis_file("output.txt", "Halo saka JametLang!");

// Baca file
variabel isi = baca_file("output.txt");
nyerat(isi);

// Tambah (append)
tambah_file("output.txt", "\nBaris anyar");

// Cek file ana
nyerat(ana_file("output.txt"));  // bener
```

## HTTP Client

```jmt
// HTTP GET
variabel response = http_get("https://httpbin.org/get");
nyerat(response);

// HTTP POST
variabel data = '{"name": "JametLang"}';
variabel result = http_post("https://httpbin.org/post", data, "application/json");
nyerat(result);
```

## Module System (jupuk / kirim)

JametLang nggunakake sistem modul Go-like karo `jupuk` (import) lan `kirim` (export).

```jmt
// math_module.jmt
kirim fungsi kuadrat(x) {
    balekno x * x;
}

kirim fungsi faktorial(n) {
    nek (n <= 1) { balekno 1; }
    balekno n * faktorial(n - 1);
}
```

```jmt
// main.jmt
jupuk "math_module.jmt";

nyerat(kuadrat(5));      // 25
nyerat(faktorial(10));   // 3628800
```

## DateTime (Tanggal & Waktu)

```jmt
// Tanggal saiki
variabel saiki = tanggal_saiki();
nyerat(saiki["taun"]);   // 2026
nyerat(saiki["sasi"]);   // 3
nyerat(saiki["dina"]);   // 11

// Format tanggal
nyerat(format_tanggal("%Y-%m-%d %H:%M:%S"));   // 2026-03-11 19:15:35
nyerat(format_tanggal("%A, %d %B %Y"));         // Wednesday, 11 March 2026

// Parsing tanggal
variabel ts = parsing_tanggal("2025-01-15", "%Y-%m-%d");
nyerat(format_tanggal("%Y-%m-%d", ts));          // 2025-01-15

// Wektu mili & zona
nyerat(wektu_mili());     // 1773231335131
nyerat(zona_wektu());     // WIB (UTC+0700)

// Sleep
turu(1000);               // turu 1 detik
```

## Kriptografi & Hashing

```jmt
// Hashing
nyerat(hash_md5("halo"));      // hex string
nyerat(hash_sha256("halo"));   // hex string
nyerat(hash_sha512("halo"));   // hex string

// HMAC
nyerat(hmac_sha256("kunci_rahasia", "data"));

// Base64
variabel enc = base64_encode("Hello, World!");  // SGVsbG8sIFdvcmxkIQ==
variabel dec = base64_decode(enc);              // Hello, World!
```

## Serialisasi (JSON & CSV)

```jmt
// JSON format (value -> string)
variabel data = {"nama": "Budi", "umur": 25, "hobi": ["coding", "gaming"]};
variabel json_str = json_format(data);
nyerat(json_str);   // {"nama":"Budi","umur":25,"hobi":["coding","gaming"]}

// JSON parsing (string -> value)
variabel parsed = json_parsing("{\"nama\":\"Siti\",\"umur\":30}");
nyerat(parsed["nama"]);   // Siti

// CSV format
variabel csv_data = [["Nama", "Umur"], ["Budi", "25"], ["Siti", "30"]];
nyerat(csv_format(csv_data));

// CSV parsing
variabel csv = csv_parsing("nama,umur\nAlice,25\nBob,30");
saben (variabel row : csv) {
    nyerat(row);
}
```

## Networking (Socket)

```jmt
// TCP Client
variabel fd = soket_tcp("example.com", 80);
soket_kirim(fd, "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n");
variabel response = soket_tampa(fd);
nyerat(response);
soket_tutup(fd);

// TCP Server
variabel server = soket_server(8080);
variabel client = soket_terima(server);
variabel pesan = soket_tampa(client);
soket_kirim(client, "HTTP/1.1 200 OK\r\n\r\nHalo!");
soket_tutup(client);
soket_tutup(server);
```

## Exception Handling

JametLang nyedhiyakake exception handling nganggo kata kunci Jawa:

| Keyword | Arti | Padanan |
| ------- | ---- | ------- |
| `jancuk` | Try block | `try` |
| `awas` | Catch block | `catch` |
| `rampungke` | Finally block (opsional) | `finally` |
| `uncal` | Throw/raise exception | `throw` |

```jmt
// Basic try/catch
jancuk {
    uncal "Ada masalah!";
} awas (err) {
    nyerat("Ditangkep: " + err);
}

// Karo finally
jancuk {
    variabel hasil = bagi(10, 0);
} awas (e) {
    nyerat("Error: " + e);
} rampungke {
    nyerat("Tansah dieksekusi");
}

// Throw saka fungsi
fungsi bagi(a, b) {
    nek (b == 0) {
        uncal "Ora bisa bagi karo nol!";
    }
    balekno a / b;
}

// Throw macem-macem jinis
uncal "pesan error";   // string
uncal 404;              // integer
uncal 3.14;             // float
```

## Status Saiki

- [x] Jinis Data Dhasar (Basic Data Types)
- [x] Lexer/Tokenizer
- [x] Komentar (Comments) — `//` lan `/* */`
- [x] Parser (AST)
- [x] Interpreter
- [x] CLI / REPL
- [x] Installer
- [x] Standard Library (string, math, konversi, utilitas)
- [x] Array Operations (literal, index, assignment, stdlib functions)
- [x] Exception Handling (`jancuk`/`awas`/`rampungke`, `uncal`)
- [x] Comparison Operators (`>=`, `<=`)
- [x] Increment/Decrement (`++`, `--`)
- [x] forEach Loop (`saben`) — iterasi array lan map
- [x] Lambda/Anonymous Functions — closure scope
- [x] HashMap/Map Data Type — `{key: val}`, index, iterasi
- [x] File I/O — `baca_file`, `tulis_file`, `tambah_file`, `ana_file`
- [x] HTTP Client — `http_get`, `http_post` (via curl)
- [x] Module System — `jupuk` (import) / `kirim` (export)
- [x] String Escape Sequences — `\n`, `\t`, `\r`, `\"`, `\\`
- [x] DateTime — `tanggal_saiki`, `format_tanggal`, `parsing_tanggal`, `wektu_mili`, `turu`, `zona_wektu`
- [x] Concurrency — `gawe_tugas`, `enteni_tugas`, `tugas_rampung`, `kunci_mutex`, `buka_mutex`
- [x] Networking (Socket) — `soket_tcp`, `soket_kirim`, `soket_tampa`, `soket_tutup`, `soket_server`, `soket_terima`
- [x] Kriptografi & Hashing — `hash_md5`, `hash_sha1`, `hash_sha256`, `hash_sha512`, `hmac_sha256`, `base64_encode/decode`
- [x] Serialisasi — `json_parsing`, `json_format`, `csv_parsing`, `csv_format`

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
