/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * REPL - Read-Eval-Print Loop Implementation
 */

#include "repl.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Try to use readline if available */
#ifdef USE_READLINE
    #include <readline/readline.h>
    #include <readline/history.h>
    #define HAS_READLINE 1
#else
    #define HAS_READLINE 0
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
    printf("     _    _     ___  __ _____ _____ \n");
    printf("    | |  / \\   |  \\/  | ____|_   _|\n");
    printf("_   | | / _ \\  |  |\\/| |  _   |   | |  \n");
    printf("| |_| |/ ___ \\ |  |  |  | |___  | |  \n");
    printf("\\___//_/    \\_\\_|  |__|_____| |_|  \n");
    printf("                                          \n");
    printf("    Bahasa Pemrograman Basa Jawa          \n\n");
    printf("  Ketik '.tulung' kanggo bantuan\n");
    printf("  Ketik '.metu' utawa tekan Ctrl-D kanggo metu\n\n");
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

/* Count brace balance in a string, skipping string literals and comments */
static int count_brace_balance(const char *text) {
    int balance = 0;
    int in_string = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (in_string) {
            if (text[i] == '\\' && text[i + 1] != '\0') {
                i++; /* skip escaped char */
            } else if (text[i] == '"') {
                in_string = 0;
            }
            continue;
        }
        /* Skip single-line comments */
        if (text[i] == '/' && text[i + 1] == '/') {
            break;
        }
        if (text[i] == '"') {
            in_string = 1;
        } else if (text[i] == '{') {
            balance++;
        } else if (text[i] == '}') {
            balance--;
        }
    }
    return balance;
}

/* Execute single line of code */
void repl_execute_line(const char *line) {
    /* Check for commands */
    if (line[0] == '.') {
        if (strcmp(line, ".metu") == 0) {
            printf("Matur nuwun! Nyangoni Slamet!\n");
            exit(0);
        } else if (strcmp(line, ".tulung") == 0) {
            printf("\n--- Perintah JametLang REPL ---\n");
            printf("  .tulung  - Nampilake bantuan iki\n");
            printf("  .metu    - Metu saka REPL\n");
            printf("  .tokens  - Nampilake token stream\n");
            printf("  .busak   - Bersihna layar\n");
            printf("\n--- Conto Perintah ---\n");
            printf("  variabel x = 10;\n");
            printf("  nek (x > 5) { nyerat(\"X gedhe\"); }\n");
            printf("  kanggo (i = 0; i < 10; i = i + 1) { }\n\n");
            return;
        } else if (strcmp(line, ".busak") == 0) {
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
            printf("Ketik '.tulung' kanggo ndeleng perintah sing tersedia.\n");
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
        /* Parse and evaluate */
        Parser *parser = parser_new(tokens, count);
        size_t stmt_count;
        Stmt **statements = parser_parse_program(parser, &stmt_count);

        /* Check if any statement is a func decl (or export of func decl) */
        int has_func_decl = 0;
        for (size_t i = 0; i < stmt_count; i++) {
            if (statements[i]->type == STMT_FUNC_DECL ||
                statements[i]->type == STMT_EXPORT) {
                has_func_decl = 1;
                break;
            }
        }

        /* Execute statements */
        for (size_t i = 0; i < stmt_count; i++) {
            exec_stmt(statements[i]);
        }

        /* Free statements — but if func decls are present, keep everything
           alive because the function table references the AST nodes and
           their token lexemes. This is a small, acceptable leak for REPL. */
        if (!has_func_decl) {
            for (size_t i = 0; i < stmt_count; i++) {
                stmt_free(statements[i]);
            }
            free(statements);
            parser_free(parser);
            lexer_free_tokens(tokens, count);
            lexer_free(lexer);
        } else {
            /* Only free the parser struct, not the AST or tokens */
            free(statements);
            parser_free(parser);
            lexer_free(lexer);
            /* tokens and stmt ASTs are intentionally kept alive */
        }
        return;
    }

    lexer_free_tokens(tokens, count);
    lexer_free(lexer);
}

/* Run REPL */
void repl_run(void) {
#if HAS_READLINE
    repl_print_welcome();

    char *line;

    /* Load history */
    read_history(".jamet_history");

    /* Multi-line buffer */
    size_t buf_cap = 4096;
    char *buf = (char *)malloc(buf_cap);
    buf[0] = '\0';
    size_t buf_len = 0;
    int brace_depth = 0;

    while ((line = readline(brace_depth > 0 ? "...   " : "jamet> ")) != NULL) {
        size_t line_len = strlen(line);

        /* If not in a block and line is empty, skip */
        if (brace_depth == 0 && line_len == 0) {
            free(line);
            continue;
        }

        /* Grow buffer if needed: current + newline + new line + null */
        while (buf_len + line_len + 2 >= buf_cap) {
            buf_cap *= 2;
            buf = (char *)realloc(buf, buf_cap);
        }

        /* Append newline separator if already have content */
        if (buf_len > 0) {
            buf[buf_len++] = '\n';
        }
        memcpy(buf + buf_len, line, line_len);
        buf_len += line_len;
        buf[buf_len] = '\0';

        /* Update brace balance */
        brace_depth += count_brace_balance(line);
        if (brace_depth < 0) brace_depth = 0;

        free(line);

        /* If braces are balanced, execute the accumulated buffer */
        if (brace_depth == 0) {
            /* Execute the complete input */
            repl_execute_line(buf);

            /* Add to history */
            if (buf_len > 0) {
                add_history(buf);
                write_history(".jamet_history");
            }

            /* Reset buffer */
            buf[0] = '\0';
            buf_len = 0;
        }
    }

    free(buf);
    printf("\nMatur nuwun! Nyangoni Slamet!\n");
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
    ssize_t nread;

    /* Multi-line buffer */
    size_t buf_cap = 4096;
    char *buf = (char *)malloc(buf_cap);
    buf[0] = '\0';
    size_t buf_len = 0;
    int brace_depth = 0;

    while (1) {
        if (brace_depth > 0) {
            printf("...   ");
        } else {
            repl_print_prompt();
        }
        fflush(stdout);

        nread = getline(&line, &len, stdin);

        if (nread == -1) {
            printf("\nMatur nuwun! Nyangoni Slamet!\n");
            break;
        }

        /* Remove trailing newline */
        if (nread > 0 && line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
            nread--;
        }

        size_t line_len = (size_t)nread;

        /* If not in a block and line is empty, skip */
        if (brace_depth == 0 && line_len == 0) {
            continue;
        }

        /* Grow buffer if needed */
        while (buf_len + line_len + 2 >= buf_cap) {
            buf_cap *= 2;
            buf = (char *)realloc(buf, buf_cap);
        }

        if (buf_len > 0) {
            buf[buf_len++] = '\n';
        }
        memcpy(buf + buf_len, line, line_len);
        buf_len += line_len;
        buf[buf_len] = '\0';

        brace_depth += count_brace_balance(line);
        if (brace_depth < 0) brace_depth = 0;

        if (brace_depth == 0) {
            repl_execute_line(buf);
            buf[0] = '\0';
            buf_len = 0;
        }
    }

    free(buf);
    if (line) {
        free(line);
    }
}
