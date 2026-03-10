/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * Main Entry Point - Demo Basic Data Types
 */

#include "jamet_types.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

/* Token type names untuk debugging */
const char *token_type_names[] = {
    "EOF",
    "ERROR",
    "IDENTIFIER",
    "INTEGER",
    "FLOAT",
    "STRING",
    "NEK",
    "KAJABA",
    "KANGGO",
    "SAWISE",
    "BALEKNO",
    "NYERAT",
    "MOCO",
    "FUNGS",
    "VARIABEL",
    "BENER",
    "SALAH",
    "KOSONG",
    "PEGAT",
    "TERUSAKE",
    "LAN",
    "UTAWA",
    "ORA",
    "KELAS",
    "ANYAR",
    "PIWULANG",
    "PLUS",
    "MINUS",
    "KALI",
    "BAGI",
    "MODULO",
    "SAMADENGAN",
    "BEDA",
    "LEBIH_BESIK",
    "LEBIH_CIYUT",
    "PLUS_SAMA",
    "MINUS_SAMA",
    "LPAREN",
    "RPAREN",
    "LBRACE",
    "RBRACE",
    "LBRACKET",
    "RBRACKET",
    "COMMA",
    "DOT",
    "COLON",
    "SEMICOLON",
    "ASSIGN"
};

/* Demo tipe data dhasar */
void demo_data_types() {
    printf("=== JametLang - Jinis Data Dhasar ===\n\n");

    /* Integer */
    printf("1. INTEGER (Nomor Bulat):\n");
    JametValue *num = jamet_integer_new(42);
    char *str = jamet_value_to_string(num);
    printf("   Nilai: %s\n", str);
    free(str);
    jamet_value_free(num);

    /* Float */
    printf("\n2. FLOAT (Nomor Desimal):\n");
    JametValue *fnum = jamet_float_new(3.14159);
    str = jamet_value_to_string(fnum);
    printf("   Nilai: %s\n", str);
    free(str);
    jamet_value_free(fnum);

    /* String */
    printf("\n3. STRING (Teks):\n");
    JametValue *text = jamet_string_new("Halo, JametLang!");
    str = jamet_value_to_string(text);
    printf("   Nilai: %s\n", str);
    free(str);
    jamet_value_free(text);

    /* Boolean - Bener */
    printf("\n4. BOOLEAN - BENER (True):\n");
    JametValue *benar = jamet_boolean_new(1);
    str = jamet_value_to_string(benar);
    printf("   Nilai: %s\n", str);
    free(str);
    jamet_value_free(benar);

    /* Boolean - Salah */
    printf("\n5. BOOLEAN - SALAH (False):\n");
    JametValue *salah = jamet_boolean_new(0);
    str = jamet_value_to_string(salah);
    printf("   Nilai: %s\n", str);
    free(str);
    jamet_value_free(salah);

    /* Array */
    printf("\n6. ARRAY (Kumpulan Data):\n");
    JametValue *arr = jamet_array_new(4);
    jamet_array_push(arr, jamet_integer_new(1));
    jamet_array_push(arr, jamet_integer_new(2));
    jamet_array_push(arr, jamet_integer_new(3));
    str = jamet_value_to_string(arr);
    printf("   Nilai: %s\n", str);
    free(str);
    jamet_value_free(arr);

    /* None/Kosong */
    printf("\n7. KOSONG (None/Null):\n");
    JametValue *none = jamet_value_new(JAMET_NONE);
    str = jamet_value_to_string(none);
    printf("   Nilai: %s\n", str);
    free(str);
    jamet_value_free(none);

    printf("\n=== Kabeh Jinis Data Data ===\n");
}

/* Demo lexer */
void demo_lexer() {
    printf("\n\n=== JametLang - Lexer Demo ===\n\n");

    /* Conto kode JametLang */
    const char *source =
        "nek (x > 5) {\n"
        "    nyerat(\"X luwih gedhe saka 5\");\n"
        "} kajaba {\n"
        "    nyerat(\"X cilik utawa samadengan 5\");\n"
        "}\n"
        "\n"
        "kanggo (i = 0; i < 10; i = i + 1) {\n"
        "    nyerat(i);\n"
        "}\n"
        "\n"
        "variabel hasil = bener;\n"
        "variabel angka = 42.5;\n"
        "variabel nama = \"Jamet\";";

    printf("Kode Sumber:\n");
    printf("----------------------------------------\n");
    printf("%s\n", source);
    printf("----------------------------------------\n\n");

    printf("Token sing didapet:\n");
    printf("----------------------------------------\n");

    Lexer *lexer = lexer_new(source);
    size_t count;
    Token *tokens = lexer_scan_all(lexer, &count);

    for (size_t i = 0; i < count; i++) {
        Token t = tokens[i];
        if (t.type == TOKEN_EOF) {
            printf("[TOKEN_EOF]\n");
            break;
        } else if (t.type == TOKEN_ERROR) {
            printf("[ERROR] %s (baris %d, kolom %d)\n", t.lexeme, t.line, t.column);
        } else {
            const char *type_name = (t.type < sizeof(token_type_names) / sizeof(token_type_names[0]))
                ? token_type_names[t.type]
                : "UNKNOWN";
            printf("[%-15s] '%s' (baris %d, kolom %d)\n", type_name, t.lexeme, t.line, t.column);
        }
    }

    lexer_free_tokens(tokens, count);
    lexer_free(lexer);

    printf("----------------------------------------\n");
}

/* Conto program JametLang */
void demo_program_example() {
    printf("\n\n=== Conto Program JametLang ===\n\n");

    printf("/* Program Nitip Barang - Conto Bahasa JametLang */\n\n");
    printf("fungsi nitipBarang(variabel barang, variabel jumlah) {\n");
    printf("    nek (barang == kosong) {\n");
    printf("        balekno \"Ora ana barang sing arep dititipne!\";\n");
    printf("    }\n\n");
    printf("    variabel total = 0;\n");
    printf("    kanggo (variabel i = 0; i < jumlah; i = i + 1) {\n");
    printf("        total = total + 1000;\n");
    printf("        nyerat(\"Nitipanke barang nomer: \" + i);\n");
    printf("    }\n\n");
    printf("    balekno total;\n");
    printf("}\n\n");
    printf("variabel hasil = nitipBarang(\"Keripik\", 5);\n");
    printf("nyerat(\"Total biaya: \" + hasil);\n\n");
    printf("/* Logika kondisi */\n");
    printf("nek (hasil > 5000 lan hasil < 10000) {\n");
    printf("    nyerat(\"Diskon 10%%\");\n");
    printf("} kajaba nek (hasil >= 10000) {\n");
    printf("    nyerat(\"Diskon 20%%\");\n");
    printf("} kajaba {\n");
    printf("    nyerat(\"Ora ana diskon\");\n");
    printf("}\n");
}

/* Main function */
int main(int argc __attribute__((unused)), char **argv __attribute__((unused))) {
    printf("   __  __          _____   _____\n");
    printf("  |  \\/  |   /\\   |  __ \\ / ____|\n");
    printf("  | \\  / |  /  \\  | |__) | |     \n");
    printf("  | |\\/| | / /\\ \\ |  _  /| |     \n");
    printf("  | |  | |/ ____ \\| | \\ \\| |____ \n");
    printf("  |_|  |_/_/    \\_\\_|  \\_\\\\_____|\n");
    printf("  Bahasa Pemrograman Basa Jawa\n\n");

    demo_data_types();
    demo_program_example();
    demo_lexer();

    printf("\n=== Sampun ===\n");

    return 0;
}
