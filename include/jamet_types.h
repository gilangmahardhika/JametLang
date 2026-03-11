/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * Basic Data Types Header
 */

#ifndef JAMET_TYPES_H
#define JAMET_TYPES_H

#include <stddef.h>

/* JametLang Value Types - Jinis Data ing JametLang */
typedef enum {
    JAMET_NONE,      /* Kosong - None/Null */
    JAMET_INTEGER,   /* Integer - Nomor bulat */
    JAMET_FLOAT,     /* Float - Nomor desimal */
    JAMET_STRING,    /* String - Teks */
    JAMET_BOOLEAN,   /* Boolean - Bener/Salah */
    JAMET_ARRAY,     /* Array - Kumpulan data */
    JAMET_MAP,       /* Map/Dictionary - Kamus */
    JAMET_OBJECT,    /* Object - Objek */
    JAMET_FUNCTION   /* Function - Fungsi */
} JametType;

/* Forward declaration kanggo penggunaan ing array */
typedef struct JametValue JametValue;

/* JametLang Value - Nilai ing JametLang */
struct JametValue {
    JametType type;
    union {
        long integer;          /* Nilai integer */
        double float_val;       /* Nilai float */
        int boolean;            /* Nilai boolean (1 = bener, 0 = salah) */
        struct {                /* Nilai string */
            char *value;
            size_t length;
        } string;
        struct {                /* Nilai array */
            JametValue **elements;
            size_t count;
            size_t capacity;
        } array;
        struct {                /* Nilai map/dictionary */
            char **keys;
            JametValue **values;
            size_t count;
            size_t capacity;
        } map;
        void *ptr;              /* Pointer kanggo object/function */
    } as;
};

/* Token Types - Jinis Token ing Lexer */
typedef enum {
    /* Token Khusus */
    TOKEN_EOF,
    TOKEN_ERROR,

    /* Identifier & Literal */
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_STRING,

    /* Keywords - Tembung Kunci ing Bahasa Jawa */
    TOKEN_NEK,         /* if */
    TOKEN_KAJABA,      /* else */
    TOKEN_KANGGO,      /* for */
    TOKEN_SAWISE,      /* while */
    TOKEN_BALEKNO,     /* return */
    TOKEN_NYERAT,      /* write/print */
    TOKEN_MOCO,        /* read/input */
    TOKEN_FUNGSI,      /* function */
    TOKEN_VARIABEL,    /* var/let */
    TOKEN_BENER,       /* true */
    TOKEN_SALAH,       /* false */
    TOKEN_KOSONG,      /* null */
    TOKEN_PEGAT,       /* break */
    TOKEN_TERUSAKE,    /* continue */
    TOKEN_LAN,         /* and */
    TOKEN_UTAWA,       /* or */
    TOKEN_ORA,         /* not */
    TOKEN_JANCUK,      /* try */
    TOKEN_AWAS,        /* catch */
    TOKEN_RAMPUNGKE,   /* finally */
    TOKEN_UNCAL,       /* throw */
    TOKEN_SABEN,       /* forEach */
    TOKEN_JUPUK,       /* import */
    TOKEN_KIRIM,       /* export */

    /* Operators & Symbols */
    TOKEN_PLUS,        /* + */
    TOKEN_MINUS,       /* - */
    TOKEN_KALI,        /* * */
    TOKEN_BAGI,        /* / */
    TOKEN_MODULO,      /* % */
    TOKEN_SAMADENGAN,  /* == */
    TOKEN_BEDA,        /* != */
    TOKEN_LEBIH_BESIK, /* > */
    TOKEN_LEBIH_CIYUT, /* < */
    TOKEN_LEBIH_SAMA,  /* >= */
    TOKEN_KURANG_SAMA, /* <= */
    TOKEN_PLUS_SAMA,   /* += */
    TOKEN_MINUS_SAMA,  /* -= */
    TOKEN_PLUS_PLUS,   /* ++ */
    TOKEN_MINUS_MINUS, /* -- */
    TOKEN_LPAREN,      /* ( */
    TOKEN_RPAREN,      /* ) */
    TOKEN_LBRACE,      /* { */
    TOKEN_RBRACE,      /* } */
    TOKEN_LBRACKET,    /* [ */
    TOKEN_RBRACKET,    /* ] */
    TOKEN_COMMA,       /* , */
    TOKEN_DOT,         /* . */
    TOKEN_COLON,       /* : */
    TOKEN_SEMICOLON,   /* ; */
    TOKEN_ASSIGN       /* = */
} TokenType;

/* Token - Token sing diasilke dening Lexer */
typedef struct {
    TokenType type;
    char *lexeme;
    int line;
    int column;
} Token;

/* Helper Functions - Fungsi Bantu */

/* Mbuat JametValue anyar */
JametValue *jamet_value_new(JametType type);

/* Mbebasna memori JametValue */
void jamet_value_free(JametValue *value);

/* Nyalin JametValue */
JametValue *jamet_value_copy(JametValue *value);

/* Mbuat integer anyar */
JametValue *jamet_integer_new(long value);

/* Mbuat float anyar */
JametValue *jamet_float_new(double value);

/* Mbuat string anyar */
JametValue *jamet_string_new(const char *value);

/* Mbuat boolean anyar */
JametValue *jamet_boolean_new(int value);

/* Mbuat array anyar */
JametValue *jamet_array_new(size_t capacity);

/* Ngonversi JametValue dadi string */
char *jamet_value_to_string(JametValue *value);

/* Mbandingake loro JametValue */
int jamet_value_equals(JametValue *a, JametValue *b);

/* Nambah elemen ing array */
void jamet_array_push(JametValue *array, JametValue *element);

/* Mbuat map anyar */
JametValue *jamet_map_new(size_t capacity);

/* Set map entry */
void jamet_map_set(JametValue *map, const char *key, JametValue *value);

/* Get map entry */
JametValue *jamet_map_get(JametValue *map, const char *key);

#endif /* JAMET_TYPES_H */
