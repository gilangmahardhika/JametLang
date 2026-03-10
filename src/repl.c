/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * REPL - Read-Eval-Print Loop Implementation
 */

#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Try to use linenoise if available, otherwise use fallback */
#ifdef USE_LINENOISE
    #include <linenoise.h>
    #define HAS_LINENOISE 1
#else
    #define HAS_LINENOISE 0
#endif

/* Token type names */
static const char *token_type_names[] = {
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

/* Print welcome message */
void repl_print_welcome(void) {
    printf("\n");
    printf("    _    _    __  __ _____ _____ \n");
    printf("   | |  / \\  |  \\/  | ____|_   _|\n");
    printf("_  | | / _ \\ | |\\/| |  _|   | |  \n");
    printf("| |_| |/ ___ \\| |  | | |___  | |  \n");
    printf("\\___//_/   \\_\\_|  |_|_____| |_|  \n");
    printf("                                          \n");
    printf("    Bahasa Pemrograman Basa Jawa          \n\n");
    printf("  Ketik '.help' kanggo bantuan\n");
    printf("  Ketik '.exit' utawa tekan Ctrl-D kanggo metu\n\n");
}

/* Print prompt */
void repl_print_prompt(void) {
    printf("jamet> ");
    fflush(stdout);
}

/* Print token stream for debugging */
void repl_print_tokens(Token *tokens, size_t count) {
    printf("\n--- Token Stream ---\n");
    for (size_t i = 0; i < count; i++) {
        Token t = tokens[i];
        if (t.type == TOKEN_EOF) {
            printf("[EOF]\n");
            break;
        } else if (t.type == TOKEN_ERROR) {
            printf("[ERROR] %s\n", t.lexeme);
        } else {
            const char *type_name = (t.type < sizeof(token_type_names) / sizeof(token_type_names[0]))
                ? token_type_names[t.type]
                : "UNKNOWN";
            printf("[%s] '%s'\n", type_name, t.lexeme);
        }
    }
    printf("--------------------\n");
}

/* Execute single line of code */
void repl_execute_line(const char *line) {
    /* Check for commands */
    if (line[0] == '.') {
        if (strcmp(line, ".exit") == 0) {
            printf("Matur nuwun! Sampun jumpa!\n");
            exit(0);
        } else if (strcmp(line, ".help") == 0) {
            printf("\n--- Perintah JametLang REPL ---\n");
            printf("  .help    - Nampilake bantuan iki\n");
            printf("  .exit    - Metu saka REPL\n");
            printf("  .tokens  - Nampilake token stream\n");
            printf("  .clear   - Bersihna layar\n");
            printf("\n--- Conto Perintah ---\n");
            printf("  variabel x = 10;\n");
            printf("  nek (x > 5) { nyerat(\"X gedhe\"); }\n");
            printf("  kanggo (i = 0; i < 10; i = i + 1) { }\n\n");
            return;
        } else if (strcmp(line, ".clear") == 0) {
            printf("\033[2J\033[H");
            repl_print_welcome();
            return;
        } else if (strcmp(line, ".tokens") == 0) {
            printf("Mode token debug aktif. Gunakake .notokens kanggo mateni.\n");
            return;
        } else if (strcmp(line, ".notokens") == 0) {
            printf("Mode token debug mati.\n");
            return;
        } else {
            printf("Perintah ora dikenal: %s\n", line);
            printf("Ketik '.help' kanggo ndeleng perintah sing tersedia.\n");
            return;
        }
    }

    /* Empty line */
    if (strlen(line) == 0) {
        return;
    }

    /* Tokenize the line */
    Lexer *lexer = lexer_new(line);
    size_t count;
    Token *tokens = lexer_scan_all(lexer, &count);

    /* Check for errors */
    int has_error = 0;
    for (size_t i = 0; i < count; i++) {
        if (tokens[i].type == TOKEN_ERROR) {
            printf("Kesalahan: %s\n", tokens[i].lexeme);
            has_error = 1;
            break;
        }
    }

    if (!has_error) {
        /* TODO: Implement parsing and evaluation */
        printf("=> Sukses tokenizing (%zu token)\n", count - 1); /* -1 for EOF */
    }

    lexer_free_tokens(tokens, count);
    lexer_free(lexer);
}

/* Run REPL */
void repl_run(void) {
#if HAS_LINENOISE
    repl_print_welcome();

    char *line;

    /* Set history file */
    linenoiseHistoryLoad("jametlang_history.txt");

    while ((line = linenoise("jamet> ")) != NULL) {
        /* Execute the line */
        repl_execute_line(line);

        /* Add to history */
        if (strlen(line) > 0) {
            linenoiseHistoryAdd(line);
            linenoiseHistorySave("jametlang_history.txt");
        }

        /* Free the line */
        linenoiseFree(line);
    }

    printf("\nMatur nuwun! Sampun jumpa!\n");
#else
    /* Use fallback implementation */
    repl_run_fallback();
#endif
}

/* Fallback REPL without linenoise */
void repl_run_fallback(void) {
    repl_print_welcome();

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while (1) {
        repl_print_prompt();

        read = getline(&line, &len, stdin);

        if (read == -1) {
            /* EOF or error */
            printf("\nMatur nuwun! Sampun jumpa!\n");
            break;
        }

        /* Remove trailing newline */
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }

        repl_execute_line(line);
    }

    if (line) {
        free(line);
    }
}
