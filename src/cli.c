/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * CLI Entry Point - REPL and File Execution
 */

#include "jamet_types.h"
#include "lexer.h"
#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Print usage help */
void print_usage(const char *program_name) {
    printf("Penggunaan: %s [OPSI] [FILE]\n\n", program_name);
    printf("Opsi:\n");
    printf("  -r, --repl     Mlebet mode REPL (default yen ora ana file)\n");
    printf("  -f, --file     Jalankan file JametLang\n");
    printf("  -h, --help     Tampilake bantuan iki\n");
    printf("  -v, --version  Tampilake versi\n\n");
    printf("Conto:\n");
    printf("  %s             - Mlebet mode REPL\n", program_name);
    printf("  %s file.jmt    - Jalankan file JametLang\n", program_name);
    printf("  %s -r file.jmt - Jalankan file karo REPL\n\n", program_name);
}

/* Print version */
void print_version(void) {
    printf("JametLang v0.1.0 - Bahasa Pemrograman Basa Jawa\n");
    printf("Dibangun nganggo GCC\n");
    printf("Copyright (c) 2025 JametLang Project\n\n");
}

/* Run a JametLang file */
int run_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Kesalahan: Ora bisa mbukak file '%s'\n", filename);
        return 1;
    }

    /* Get file size */
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    /* Read file content */
    char *source = (char *)malloc(file_size + 1);
    if (source == NULL) {
        fprintf(stderr, "Kesalahan: Ora bisa mbenakna memori\n");
        fclose(file);
        return 1;
    }

    size_t read_size = fread(source, 1, file_size, file);
    source[read_size] = '\0';
    fclose(file);

    printf("Njalankan file: %s\n\n", filename);

    /* Tokenize */
    Lexer *lexer = lexer_new(source);
    size_t count;
    Token *tokens = lexer_scan_all(lexer, &count);

    /* Check for errors */
    int has_error = 0;
    for (size_t i = 0; i < count; i++) {
        if (tokens[i].type == TOKEN_ERROR) {
            fprintf(stderr, "Kesalahan di baris %d: %s\n",
                    tokens[i].line, tokens[i].lexeme);
            has_error = 1;
        }
    }

    if (!has_error) {
        printf("Sukses! %zu token didapet.\n", count - 1);
        /* TODO: Implement parsing and evaluation */
    }

    lexer_free_tokens(tokens, count);
    lexer_free(lexer);
    free(source);

    return has_error ? 1 : 0;
}

/* Main entry point */
int main(int argc, char **argv) {
    int use_repl = 1;
    const char *file_to_run = NULL;

    /* Parse command line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            print_version();
            return 0;
        } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--repl") == 0) {
            use_repl = 1;
        } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if (i + 1 < argc) {
                file_to_run = argv[++i];
            } else {
                fprintf(stderr, "Kesalahan: -f butuh jeneng file\n");
                return 1;
            }
        } else if (argv[i][0] != '-') {
            file_to_run = argv[i];
        } else {
            fprintf(stderr, "Kesalahan: Opsi ora dikenal '%s'\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    /* Execute based on mode */
    if (file_to_run != NULL) {
        int result = run_file(file_to_run);
        if (use_repl && result == 0) {
            printf("\n--- Mlebet mode REPL ---\n\n");
            repl_run();
        }
        return result;
    } else {
        repl_run();
        return 0;
    }
}
