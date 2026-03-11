/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * Standard Library Implementation
 */

#include "stdlib_jamet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

/* ==================== String Functions ==================== */

/* panjang(str) - Get string length */
static JametValue *fn_panjang(JametValue **args, size_t count) {
    if (count < 1) return jamet_value_new(JAMET_NONE);

    if (args[0]->type == JAMET_STRING) {
        return jamet_integer_new((long)args[0]->as.string.length);
    } else if (args[0]->type == JAMET_ARRAY) {
        return jamet_integer_new((long)args[0]->as.array.count);
    }
    return jamet_integer_new(0);
}

/* potong(str, mulai, akhir) - Substring */
static JametValue *fn_potong(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_STRING || args[1]->type != JAMET_INTEGER) {
        return jamet_value_new(JAMET_NONE);
    }

    const char *str = args[0]->as.string.value;
    size_t len = args[0]->as.string.length;
    long start = args[1]->as.integer;
    long end = (count >= 3 && args[2]->type == JAMET_INTEGER) ? args[2]->as.integer : (long)len;

    if (start < 0) start = 0;
    if (end > (long)len) end = (long)len;
    if (start >= end) return jamet_string_new("");

    size_t sub_len = (size_t)(end - start);
    char *result = (char *)malloc(sub_len + 1);
    strncpy(result, str + start, sub_len);
    result[sub_len] = '\0';

    JametValue *val = jamet_string_new(result);
    free(result);
    return val;
}

/* huruf_gedhe(str) - Convert to uppercase */
static JametValue *fn_huruf_gedhe(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) {
        return jamet_value_new(JAMET_NONE);
    }

    size_t len = args[0]->as.string.length;
    char *result = (char *)malloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        result[i] = (char)toupper((unsigned char)args[0]->as.string.value[i]);
    }
    result[len] = '\0';

    JametValue *val = jamet_string_new(result);
    free(result);
    return val;
}

/* huruf_cilik(str) - Convert to lowercase */
static JametValue *fn_huruf_cilik(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) {
        return jamet_value_new(JAMET_NONE);
    }

    size_t len = args[0]->as.string.length;
    char *result = (char *)malloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        result[i] = (char)tolower((unsigned char)args[0]->as.string.value[i]);
    }
    result[len] = '\0';

    JametValue *val = jamet_string_new(result);
    free(result);
    return val;
}

/* ganti(str, lama, anyar) - Replace substring */
static JametValue *fn_ganti(JametValue **args, size_t count) {
    if (count < 3 || args[0]->type != JAMET_STRING ||
        args[1]->type != JAMET_STRING || args[2]->type != JAMET_STRING) {
        return jamet_value_new(JAMET_NONE);
    }

    const char *haystack = args[0]->as.string.value;
    const char *needle = args[1]->as.string.value;
    const char *replacement = args[2]->as.string.value;
    size_t needle_len = strlen(needle);
    size_t replacement_len = strlen(replacement);

    if (needle_len == 0) return jamet_value_copy(args[0]);

    /* Count occurrences */
    int occ = 0;
    const char *tmp = haystack;
    while ((tmp = strstr(tmp, needle)) != NULL) {
        occ++;
        tmp += needle_len;
    }

    size_t result_len = strlen(haystack) + occ * (replacement_len - needle_len);
    char *result = (char *)malloc(result_len + 1);
    char *dest = result;

    while (*haystack) {
        const char *found = strstr(haystack, needle);
        if (found) {
            size_t prefix_len = (size_t)(found - haystack);
            memcpy(dest, haystack, prefix_len);
            dest += prefix_len;
            memcpy(dest, replacement, replacement_len);
            dest += replacement_len;
            haystack = found + needle_len;
        } else {
            strcpy(dest, haystack);
            dest += strlen(haystack);
            break;
        }
    }
    *dest = '\0';

    JametValue *val = jamet_string_new(result);
    free(result);
    return val;
}

/* ngandhut(str, sub) - Check if string contains substring */
static JametValue *fn_ngandhut(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_STRING || args[1]->type != JAMET_STRING) {
        return jamet_boolean_new(0);
    }
    return jamet_boolean_new(strstr(args[0]->as.string.value, args[1]->as.string.value) != NULL);
}

/* pisah(str, delimiter) - Split string into array */
static JametValue *fn_pisah(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_STRING || args[1]->type != JAMET_STRING) {
        return jamet_value_new(JAMET_NONE);
    }

    const char *str = args[0]->as.string.value;
    const char *delim = args[1]->as.string.value;
    size_t delim_len = strlen(delim);

    JametValue *arr = jamet_array_new(8);

    if (delim_len == 0) {
        /* Split into individual characters */
        for (size_t i = 0; i < args[0]->as.string.length; i++) {
            char ch[2] = { str[i], '\0' };
            jamet_array_push(arr, jamet_string_new(ch));
        }
        return arr;
    }

    const char *start = str;
    const char *found;
    while ((found = strstr(start, delim)) != NULL) {
        size_t part_len = (size_t)(found - start);
        char *part = (char *)malloc(part_len + 1);
        strncpy(part, start, part_len);
        part[part_len] = '\0';
        jamet_array_push(arr, jamet_string_new(part));
        free(part);
        start = found + delim_len;
    }
    jamet_array_push(arr, jamet_string_new(start));

    return arr;
}

/* ==================== Math Functions ==================== */

/* mutlak(n) - Absolute value */
static JametValue *fn_mutlak(JametValue **args, size_t count) {
    if (count < 1) return jamet_value_new(JAMET_NONE);

    if (args[0]->type == JAMET_INTEGER) {
        long val = args[0]->as.integer;
        return jamet_integer_new(val < 0 ? -val : val);
    } else if (args[0]->type == JAMET_FLOAT) {
        return jamet_float_new(fabs(args[0]->as.float_val));
    }
    return jamet_value_new(JAMET_NONE);
}

/* pangkat(base, exp) - Power */
static JametValue *fn_pangkat(JametValue **args, size_t count) {
    if (count < 2) return jamet_value_new(JAMET_NONE);

    double base = 0, exp = 0;
    if (args[0]->type == JAMET_INTEGER) base = (double)args[0]->as.integer;
    else if (args[0]->type == JAMET_FLOAT) base = args[0]->as.float_val;
    else return jamet_value_new(JAMET_NONE);

    if (args[1]->type == JAMET_INTEGER) exp = (double)args[1]->as.integer;
    else if (args[1]->type == JAMET_FLOAT) exp = args[1]->as.float_val;
    else return jamet_value_new(JAMET_NONE);

    double result = pow(base, exp);

    /* Return integer if both args are integer and result is whole */
    if (args[0]->type == JAMET_INTEGER && args[1]->type == JAMET_INTEGER &&
        result == (double)(long)result) {
        return jamet_integer_new((long)result);
    }
    return jamet_float_new(result);
}

/* akar(n) - Square root */
static JametValue *fn_akar(JametValue **args, size_t count) {
    if (count < 1) return jamet_value_new(JAMET_NONE);

    double val = 0;
    if (args[0]->type == JAMET_INTEGER) val = (double)args[0]->as.integer;
    else if (args[0]->type == JAMET_FLOAT) val = args[0]->as.float_val;
    else return jamet_value_new(JAMET_NONE);

    return jamet_float_new(sqrt(val));
}

/* acak(min, max) - Random integer between min and max (inclusive) */
static int rand_seeded = 0;
static JametValue *fn_acak(JametValue **args, size_t count) {
    if (!rand_seeded) {
        srand((unsigned int)time(NULL));
        rand_seeded = 1;
    }

    long min_val = 0, max_val = 100;
    if (count >= 1 && args[0]->type == JAMET_INTEGER) min_val = args[0]->as.integer;
    if (count >= 2 && args[1]->type == JAMET_INTEGER) max_val = args[1]->as.integer;

    if (min_val > max_val) { long tmp = min_val; min_val = max_val; max_val = tmp; }
    long range = max_val - min_val + 1;
    long result = min_val + (rand() % range);
    return jamet_integer_new(result);
}

/* bulatke(n) - Round to nearest integer */
static JametValue *fn_bulatke(JametValue **args, size_t count) {
    if (count < 1) return jamet_value_new(JAMET_NONE);

    if (args[0]->type == JAMET_FLOAT) {
        return jamet_integer_new((long)round(args[0]->as.float_val));
    } else if (args[0]->type == JAMET_INTEGER) {
        return jamet_integer_new(args[0]->as.integer);
    }
    return jamet_value_new(JAMET_NONE);
}

/* lantai(n) - Floor */
static JametValue *fn_lantai(JametValue **args, size_t count) {
    if (count < 1) return jamet_value_new(JAMET_NONE);

    if (args[0]->type == JAMET_FLOAT) {
        return jamet_integer_new((long)floor(args[0]->as.float_val));
    } else if (args[0]->type == JAMET_INTEGER) {
        return jamet_integer_new(args[0]->as.integer);
    }
    return jamet_value_new(JAMET_NONE);
}

/* langit(n) - Ceil */
static JametValue *fn_langit(JametValue **args, size_t count) {
    if (count < 1) return jamet_value_new(JAMET_NONE);

    if (args[0]->type == JAMET_FLOAT) {
        return jamet_integer_new((long)ceil(args[0]->as.float_val));
    } else if (args[0]->type == JAMET_INTEGER) {
        return jamet_integer_new(args[0]->as.integer);
    }
    return jamet_value_new(JAMET_NONE);
}

/* sisa(a, b) - Modulo for floats */
static JametValue *fn_sisa(JametValue **args, size_t count) {
    if (count < 2) return jamet_value_new(JAMET_NONE);

    double a = 0, b = 0;
    if (args[0]->type == JAMET_INTEGER) a = (double)args[0]->as.integer;
    else if (args[0]->type == JAMET_FLOAT) a = args[0]->as.float_val;
    else return jamet_value_new(JAMET_NONE);

    if (args[1]->type == JAMET_INTEGER) b = (double)args[1]->as.integer;
    else if (args[1]->type == JAMET_FLOAT) b = args[1]->as.float_val;
    else return jamet_value_new(JAMET_NONE);

    if (b == 0) return jamet_value_new(JAMET_NONE);
    return jamet_float_new(fmod(a, b));
}

/* ==================== Type Conversion Functions ==================== */

/* dadi_angka(val) - Convert to integer */
static JametValue *fn_dadi_angka(JametValue **args, size_t count) {
    if (count < 1) return jamet_value_new(JAMET_NONE);

    switch (args[0]->type) {
        case JAMET_INTEGER:
            return jamet_integer_new(args[0]->as.integer);
        case JAMET_FLOAT:
            return jamet_integer_new((long)args[0]->as.float_val);
        case JAMET_BOOLEAN:
            return jamet_integer_new(args[0]->as.boolean ? 1 : 0);
        case JAMET_STRING: {
            char *endptr;
            long val = strtol(args[0]->as.string.value, &endptr, 10);
            if (endptr != args[0]->as.string.value) {
                return jamet_integer_new(val);
            }
            return jamet_value_new(JAMET_NONE);
        }
        default:
            return jamet_value_new(JAMET_NONE);
    }
}

/* dadi_desimal(val) - Convert to float */
static JametValue *fn_dadi_desimal(JametValue **args, size_t count) {
    if (count < 1) return jamet_value_new(JAMET_NONE);

    switch (args[0]->type) {
        case JAMET_INTEGER:
            return jamet_float_new((double)args[0]->as.integer);
        case JAMET_FLOAT:
            return jamet_float_new(args[0]->as.float_val);
        case JAMET_BOOLEAN:
            return jamet_float_new(args[0]->as.boolean ? 1.0 : 0.0);
        case JAMET_STRING: {
            char *endptr;
            double val = strtod(args[0]->as.string.value, &endptr);
            if (endptr != args[0]->as.string.value) {
                return jamet_float_new(val);
            }
            return jamet_value_new(JAMET_NONE);
        }
        default:
            return jamet_value_new(JAMET_NONE);
    }
}

/* dadi_teks(val) - Convert to string */
static JametValue *fn_dadi_teks(JametValue **args, size_t count) {
    if (count < 1) return jamet_string_new("");

    if (args[0]->type == JAMET_STRING) {
        return jamet_value_copy(args[0]);
    }

    char *str = jamet_value_to_string(args[0]);
    JametValue *result = jamet_string_new(str);
    free(str);
    return result;
}

/* jinis(val) - Get type name as string */
static JametValue *fn_jinis(JametValue **args, size_t count) {
    if (count < 1) return jamet_string_new("kosong");

    switch (args[0]->type) {
        case JAMET_NONE:     return jamet_string_new("kosong");
        case JAMET_INTEGER:  return jamet_string_new("integer");
        case JAMET_FLOAT:    return jamet_string_new("float");
        case JAMET_STRING:   return jamet_string_new("string");
        case JAMET_BOOLEAN:  return jamet_string_new("boolean");
        case JAMET_ARRAY:    return jamet_string_new("array");
        default:             return jamet_string_new("ora dikenal");
    }
}

/* ==================== Utility Functions ==================== */

/* wektu() - Get current time in seconds since epoch */
static JametValue *fn_wektu(JametValue **args, size_t count) {
    (void)args;
    (void)count;
    return jamet_integer_new((long)time(NULL));
}

/* turu(ms) - Sleep for milliseconds (not blocking, just a placeholder) */
/* We'll skip actual sleep to avoid blocking the interpreter */

/* ==================== Dispatch Table ==================== */

typedef JametValue *(*StdlibFunc)(JametValue **args, size_t count);

typedef struct {
    const char *name;
    StdlibFunc func;
} StdlibEntry;

static const StdlibEntry stdlib_functions[] = {
    /* String functions */
    {"panjang",      fn_panjang},
    {"potong",       fn_potong},
    {"huruf_gedhe",  fn_huruf_gedhe},
    {"huruf_cilik",  fn_huruf_cilik},
    {"ganti",        fn_ganti},
    {"ngandhut",     fn_ngandhut},
    {"pisah",        fn_pisah},

    /* Math functions */
    {"mutlak",       fn_mutlak},
    {"pangkat",      fn_pangkat},
    {"akar",         fn_akar},
    {"acak",         fn_acak},
    {"bulatke",      fn_bulatke},
    {"lantai",       fn_lantai},
    {"langit",       fn_langit},
    {"sisa",         fn_sisa},

    /* Type conversion */
    {"dadi_angka",   fn_dadi_angka},
    {"dadi_desimal", fn_dadi_desimal},
    {"dadi_teks",    fn_dadi_teks},
    {"jinis",        fn_jinis},

    /* Utility */
    {"wektu",        fn_wektu},

    {NULL, NULL}
};

/* Public API: try to call a stdlib function */
JametValue *stdlib_call(const char *name, JametValue **args, size_t arg_count) {
    for (int i = 0; stdlib_functions[i].name != NULL; i++) {
        if (strcmp(name, stdlib_functions[i].name) == 0) {
            return stdlib_functions[i].func(args, arg_count);
        }
    }
    return NULL; /* Not a stdlib function */
}
