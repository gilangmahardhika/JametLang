/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * Lexer - Tokenizer Implementation
 */

#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define EXIT_IF_NULL(ptr, msg) \
    if ((ptr) == NULL) { \
        fprintf(stderr, "Kesalahan: %s\n", (msg)); \
        exit(1); \
    }

/* Daftar keyword ing Bahasa Jawa */
static const Keyword keywords[] = {
    /* Keywords utama */
    {"nek", TOKEN_NEK},              /* if */
    {"kajaba", TOKEN_KAJABA},        /* else */
    {"kanggo", TOKEN_KANGGO},        /* for */
    {"sawise", TOKEN_SAWISE},        /* while */
    {"balekno", TOKEN_BALEKNO},      /* return */
    {"nyerat", TOKEN_NYERAT},        /* write/print */
    {"moco", TOKEN_MOCO},            /* read/input */
    {"fungsi", TOKEN_FUNGSI},        /* function */
    {"variabel", TOKEN_VARIABEL},    /* var/let */
    {"bener", TOKEN_BENER},          /* true */
    {"salah", TOKEN_SALAH},          /* false */
    {"kosong", TOKEN_KOSONG},        /* null */
    {"pegat", TOKEN_PEGAT},          /* break */
    {"terusake", TOKEN_TERUSAKE},    /* continue */
    {"lan", TOKEN_LAN},              /* and */
    {"utawa", TOKEN_UTAWA},          /* or */
    {"ora", TOKEN_ORA},              /* not */
    {"jancuk", TOKEN_JANCUK},        /* try */
    {"awas", TOKEN_AWAS},            /* catch */
    {"rampungke", TOKEN_RAMPUNGKE},  /* finally */
    {"uncal", TOKEN_UNCAL},          /* throw */
    {"saben", TOKEN_SABEN},          /* forEach */
    {"jupuk", TOKEN_JUPUK},          /* import */
    {"kirim", TOKEN_KIRIM},          /* export */

    /* Variasi ejaan (nganggo a/e/o) */
    {"kango", TOKEN_KANGGO},         /* for - tanpa go */
    {"sawise", TOKEN_SAWISE},        /* while */
    {"balekne", TOKEN_BALEKNO},      /* return - tanja o */
    {"tulung", TOKEN_NYERAT},        /* write - alternative */

    {NULL, TOKEN_EOF}                /* Sentinel */
};

/* Mbuat Lexer anyar */
Lexer *lexer_new(const char *source) {
    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    EXIT_IF_NULL(lexer, "Ora bisa mbenakna memori kanggo Lexer");

    lexer->source = source;
    lexer->length = strlen(source);
    lexer->start = 0;
    lexer->current = 0;
    lexer->line = 1;
    lexer->column = 1;

    return lexer;
}

/* Mbebasna Lexer */
void lexer_free(Lexer *lexer) {
    if (lexer != NULL) {
        free(lexer);
    }
}

/* Mriksa manawa wis tekan pungkasan */
int lexer_is_at_end(Lexer *lexer) {
    return lexer->current >= lexer->length;
}

/* Nemonuke karakter sabanjure */
char lexer_advance(Lexer *lexer) {
    if (lexer_is_at_end(lexer)) return '\0';
    lexer->column++;
    return lexer->source[lexer->current++];
}

/* Ndeleng karakter saiki tanpa maju */
char lexer_peek(Lexer *lexer) {
    if (lexer_is_at_end(lexer)) return '\0';
    return lexer->source[lexer->current];
}

/* Ndeleng karakter mengarepe */
char lexer_peek_next(Lexer *lexer) {
    if (lexer->current + 1 >= lexer->length) return '\0';
    return lexer->source[lexer->current + 1];
}

/* Mriksa manawa karakter cocog */
int lexer_match(Lexer *lexer, char expected) {
    if (lexer_is_at_end(lexer)) return 0;
    if (lexer->source[lexer->current] != expected) return 0;

    lexer->current++;
    lexer->column++;
    return 1;
}

/* Mlompati spasi putih lan komentar */
void lexer_skip_whitespace(Lexer *lexer) {
    while (!lexer_is_at_end(lexer)) {
        char c = lexer_peek(lexer);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                lexer_advance(lexer);
                break;
            case '\n':
                lexer->line++;
                lexer->column = 1;
                lexer_advance(lexer);
                break;
            case '/':
                if (lexer_peek_next(lexer) == '/') {
                    /* Komentar siji baris // */
                    while (!lexer_is_at_end(lexer) && lexer_peek(lexer) != '\n') {
                        lexer_advance(lexer);
                    }
                    break;
                } else if (lexer_peek_next(lexer) == '*') {
                    /* Komentar blok (block comment) */
                    lexer_advance(lexer); /* / */
                    lexer_advance(lexer); /* * */
                    while (!lexer_is_at_end(lexer)) {
                        if (lexer_peek(lexer) == '\n') {
                            lexer->line++;
                            lexer->column = 1;
                        }
                        if (lexer_peek(lexer) == '*' && lexer_peek_next(lexer) == '/') {
                            lexer_advance(lexer); /* * */
                            lexer_advance(lexer); /* / */
                            break;
                        }
                        lexer_advance(lexer);
                    }
                    break;
                }
                return;
            default:
                return;
        }
    }
}

/* Mriksa keyword */
TokenType lexer_check_keyword(Lexer *lexer, const char *rest, TokenType type) {
    size_t rest_len = strlen(rest);
    if (lexer->current - lexer->start == rest_len) {
        /* Bandingake karakter saka start + 1 (amarga karakter pertama wis dicek) */
        if (strncmp(lexer->source + lexer->start + 1, rest, rest_len) == 0) {
            return type;
        }
    }
    return TOKEN_IDENTIFIER;
}

/* Mriksa identifier type (keyword utawa identifier biasa) */
TokenType lexer_identifier_type(Lexer *lexer) {
    /* Ambil lexeme lengkap */
    size_t len = lexer->current - lexer->start;
    char *lexeme = (char *)malloc(len + 1);
    EXIT_IF_NULL(lexeme, "Ora bisa mbenakna memori kanggo identifier");
    strncpy(lexeme, lexer->source + lexer->start, len);
    lexeme[len] = '\0';

    /* Mriksa menawa ana ing daftar keyword */
    for (int i = 0; keywords[i].keyword != NULL; i++) {
        if (strcmp(lexeme, keywords[i].keyword) == 0) {
            free(lexeme);
            return keywords[i].type;
        }
    }

    free(lexeme);
    return TOKEN_IDENTIFIER;
}

/* Mriksa string */
Token lexer_string(Lexer *lexer) {
    /* Tanda petik buka wis di-advance dening lexer_scan_token */

    while (!lexer_is_at_end(lexer) && lexer_peek(lexer) != '"') {
        if (lexer_peek(lexer) == '\\') {
            /* Skip escaped character */
            lexer_advance(lexer);
            if (!lexer_is_at_end(lexer)) {
                lexer_advance(lexer);
                continue;
            }
        }
        if (lexer_peek(lexer) == '\n') {
            lexer->line++;
            lexer->column = 1;
        }
        lexer_advance(lexer);
    }

    if (lexer_is_at_end(lexer)) {
        return error_token(lexer, "String ora ditutup");
    }

    /* Mlompati tanda petik nutup */
    lexer_advance(lexer);

    return make_token(lexer, TOKEN_STRING);
}

/* Mriksa nomer */
Token lexer_number(Lexer *lexer) {
    int is_float = 0;

    /* Baca digit sadurunge titik */
    while (isdigit(lexer_peek(lexer))) {
        lexer_advance(lexer);
    }

    /* Mriksa titik desimal */
    if (lexer_peek(lexer) == '.' && isdigit(lexer_peek_next(lexer))) {
        is_float = 1;
        lexer_advance(lexer); /* Titik */

        /* Baca digit sawise titik */
        while (isdigit(lexer_peek(lexer))) {
            lexer_advance(lexer);
        }
    }

    return make_token(lexer, is_float ? TOKEN_FLOAT : TOKEN_INTEGER);
}

/* Mriksa identifier */
Token lexer_identifier(Lexer *lexer) {
    while (isalnum(lexer_peek(lexer)) || lexer_peek(lexer) == '_') {
        lexer_advance(lexer);
    }

    return make_token(lexer, lexer_identifier_type(lexer));
}

/* Mbuat token */
Token make_token(Lexer *lexer, TokenType type) {
    Token token;
    token.type = type;
    token.line = lexer->line;
    token.column = lexer->column - (lexer->current - lexer->start);

    size_t length = lexer->current - lexer->start;
    token.lexeme = (char *)malloc(length + 1);
    EXIT_IF_NULL(token.lexeme, "Ora bisa mbenakna memori kanggo token lexeme");
    strncpy(token.lexeme, lexer->source + lexer->start, length);
    token.lexeme[length] = '\0';

    return token;
}

/* Mbuat token error */
Token error_token(Lexer *lexer, const char *message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.line = lexer->line;
    token.column = lexer->column;

    token.lexeme = (char *)malloc(strlen(message) + 1);
    EXIT_IF_NULL(token.lexeme, "Ora bisa mbenakna memori kanggo error message");
    strcpy(token.lexeme, message);

    return token;
}

/* Ngetoken kelanjutan */
Token lexer_scan_token(Lexer *lexer) {
    lexer_skip_whitespace(lexer);

    lexer->start = lexer->current;

    if (lexer_is_at_end(lexer)) {
        return make_token(lexer, TOKEN_EOF);
    }

    char c = lexer_advance(lexer);

    /* Mriksa identifier utawa keyword */
    if (isalpha(c) || c == '_') {
        return lexer_identifier(lexer);
    }

    /* Mriksa nomer */
    if (isdigit(c)) {
        /* Mulangno amarga digit wis di-consume */
        lexer->current--;
        lexer->column--;
        return lexer_number(lexer);
    }

    /* Mriksa string */
    if (c == '"') {
        return lexer_string(lexer);
    }

    /* Operators lan Symbols */
    switch (c) {
        case '+':
            if (lexer_match(lexer, '=')) {
                return make_token(lexer, TOKEN_PLUS_SAMA);
            }
            if (lexer_match(lexer, '+')) {
                return make_token(lexer, TOKEN_PLUS_PLUS);
            }
            return make_token(lexer, TOKEN_PLUS);
        case '-':
            if (lexer_match(lexer, '=')) {
                return make_token(lexer, TOKEN_MINUS_SAMA);
            }
            if (lexer_match(lexer, '-')) {
                return make_token(lexer, TOKEN_MINUS_MINUS);
            }
            return make_token(lexer, TOKEN_MINUS);
        case '*':
            return make_token(lexer, TOKEN_KALI);
        case '/':
            return make_token(lexer, TOKEN_BAGI);
        case '%':
            return make_token(lexer, TOKEN_MODULO);
        case '=':
            if (lexer_match(lexer, '=')) {
                return make_token(lexer, TOKEN_SAMADENGAN);
            }
            return make_token(lexer, TOKEN_ASSIGN);
        case '!':
            if (lexer_match(lexer, '=')) {
                return make_token(lexer, TOKEN_BEDA);
            }
            /* Single ! bisa dadi ORA (not) */
            return make_token(lexer, TOKEN_ORA);
        case '>':
            if (lexer_match(lexer, '=')) {
                return make_token(lexer, TOKEN_LEBIH_SAMA);
            }
            return make_token(lexer, TOKEN_LEBIH_BESIK);
        case '<':
            if (lexer_match(lexer, '=')) {
                return make_token(lexer, TOKEN_KURANG_SAMA);
            }
            return make_token(lexer, TOKEN_LEBIH_CIYUT);
        case '(':
            return make_token(lexer, TOKEN_LPAREN);
        case ')':
            return make_token(lexer, TOKEN_RPAREN);
        case '{':
            return make_token(lexer, TOKEN_LBRACE);
        case '}':
            return make_token(lexer, TOKEN_RBRACE);
        case '[':
            return make_token(lexer, TOKEN_LBRACKET);
        case ']':
            return make_token(lexer, TOKEN_RBRACKET);
        case ',':
            return make_token(lexer, TOKEN_COMMA);
        case '.':
            return make_token(lexer, TOKEN_DOT);
        case ':':
            return make_token(lexer, TOKEN_COLON);
        case ';':
            return make_token(lexer, TOKEN_SEMICOLON);
        default:
            return error_token(lexer, "Karakter ora dikenal");
    }
}

/* Ngetoken kabeh */
Token *lexer_scan_all(Lexer *lexer, size_t *count) {
    size_t capacity = 64;
    Token *tokens = (Token *)malloc(sizeof(Token) * capacity);
    EXIT_IF_NULL(tokens, "Ora bisa mbenakna memori kanggo tokens");
    *count = 0;

    Token token = lexer_scan_token(lexer);
    while (token.type != TOKEN_EOF && token.type != TOKEN_ERROR) {
        /* Nambah kapasitas yen perlu */
        if (*count >= capacity) {
            capacity *= 2;
            tokens = (Token *)realloc(tokens, sizeof(Token) * capacity);
            EXIT_IF_NULL(tokens, "Ora bisa nambah kapasitas tokens");
        }

        tokens[(*count)++] = token;
        token = lexer_scan_token(lexer);
    }

    /* Tambah token EOF utawa ERROR */
    if (*count >= capacity) {
        capacity *= 2;
        tokens = (Token *)realloc(tokens, sizeof(Token) * capacity);
        EXIT_IF_NULL(tokens, "Ora bisa nambah kapasitas tokens");
    }
    tokens[(*count)++] = token;

    return tokens;
}

/* Mbebasna token list */
void lexer_free_tokens(Token *tokens, size_t count) {
    if (tokens == NULL) return;

    for (size_t i = 0; i < count; i++) {
        if (tokens[i].lexeme != NULL) {
            free(tokens[i].lexeme);
        }
    }

    free(tokens);
}
