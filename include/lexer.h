/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * Lexer - Tokenizer Header
 */

#ifndef LEXER_H
#define LEXER_H

#include "jamet_types.h"

/* Lexer - Ngeparse kode dadi token */
typedef struct {
    const char *source;      /* Kode sumber sing arep di-scan */
    size_t length;           /* Dawa kode sumber */
    size_t start;            /* Awal token saiki */
    size_t current;          /* Posisi saiki */
    int line;                /* Baris saiki */
    int column;              /* Kolom saiki */
} Lexer;

/* Keywords - Tembung kunci ing Bahasa Jawa */
typedef struct {
    const char *keyword;
    TokenType type;
} Keyword;

/* Mbuat Lexer anyar */
Lexer *lexer_new(const char *source);

/* Mbebasna Lexer */
void lexer_free(Lexer *lexer);

/* Ngetoken kelanjutan */
Token lexer_scan_token(Lexer *lexer);

/* Ngetoken kabeh */
Token *lexer_scan_all(Lexer *lexer, size_t *count);

/* Mbebasna token list */
void lexer_free_tokens(Token *tokens, size_t count);

/* Mbuat token */
Token make_token(Lexer *lexer, TokenType type);

/* Mbuat token error */
Token error_token(Lexer *lexer, const char *message);

/* Mriksa manawa wis tekan pungkasan */
int lexer_is_at_end(Lexer *lexer);

/* Nemonuke karakter sabanjure */
char lexer_advance(Lexer *lexer);

/* Ndeleng karakter saiki tanpa maju */
char lexer_peek(Lexer *lexer);

/* Ndeleng karakter mengarepe */
char lexer_peek_next(Lexer *lexer);

/* Mriksa manawa karakter cocog */
int lexer_match(Lexer *lexer, char expected);

/* Mlompati spasi putih */
void lexer_skip_whitespace(Lexer *lexer);

/* Mriksa keyword */
TokenType lexer_check_keyword(Lexer *lexer, const char *rest, TokenType type);

/* Mriksa identifier */
TokenType lexer_identifier_type(Lexer *lexer);

/* Mriksa string */
Token lexer_string(Lexer *lexer);

/* Mriksa nomer */
Token lexer_number(Lexer *lexer);

/* Mriksa identifier */
Token lexer_identifier(Lexer *lexer);

#endif /* LEXER_H */
