# JametLang VSCode Extension

Syntax highlighting and language support for **JametLang** — Bahasa Pemrograman Basa Jawa.

## Fitur (Features)

- **Syntax Highlighting** — Full syntax coloring for `.jmt` files
- **Code Snippets** — Quick templates for common JametLang constructs
- **Comment Toggle** — `Ctrl+/` for line comments, `Shift+Alt+A` for block comments
- **Auto-Closing** — Brackets, parentheses, and quotes
- **Code Folding** — Fold/unfold code blocks

## Tembung Kunci (Keyword Highlighting)

| Category      | Keywords                                                        |
| ------------- | --------------------------------------------------------------- |
| Control Flow  | `nek`, `kajaba`, `kanggo`, `sawise`, `balekno`, `pegat`, `terusake` |
| Declarations  | `variabel`, `fungsi`, `kelas`, `piwulang`                       |
| Built-in      | `nyerat`, `moco`, `anyar`                                       |
| Constants     | `bener`, `salah`, `kosong`                                      |
| Logical       | `lan`, `utawa`, `ora`                                           |

## Snippets

| Prefix       | Description                    |
| ------------ | ------------------------------ |
| `variabel`   | Variable declaration           |
| `nyerat`     | Print statement                |
| `moco`       | Read input                     |
| `nek`        | If statement                   |
| `nekkajaba`  | If-else statement              |
| `kanggo`     | For loop                       |
| `sawise`     | While loop                     |
| `fungsi`     | Function declaration           |
| `balekno`    | Return statement               |
| `pegat`      | Break                          |
| `terusake`   | Continue                       |

## Instalasi (Installation)

### Saka VSIX (From VSIX)

```bash
# Build VSIX
cd jametlang-vscode
npx @vscode/vsce package

# Install
code --install-extension jametlang-0.1.0.vsix
```

### Development Mode

```bash
# Kopi folder menyang extensions directory
# macOS/Linux:
cp -r jametlang-vscode ~/.vscode/extensions/jametlang

# Windows:
# Copy jametlang-vscode to %USERPROFILE%\.vscode\extensions\jametlang
```

Sawise install, restart VSCode lan mbukak file `.jmt`.

## Screenshot

```java
/* Conto JametLang karo syntax highlighting */

variabel pesan = "Halo, Dunia!";
nyerat(pesan);

fungsi tambah(a, b) {
    balekno a + b;
}

kanggo (variabel i = 0; i < 5; i = i + 1) {
    nek (i == 3) {
        pegat;
    }
    nyerat("Iterasi: " + i);
}
```
