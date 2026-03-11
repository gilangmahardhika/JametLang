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
    } else if (args[0]->type == JAMET_MAP) {
        return jamet_integer_new((long)args[0]->as.map.count);
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
        case JAMET_MAP:      return jamet_string_new("map");
        case JAMET_FUNCTION: return jamet_string_new("fungsi");
        default:             return jamet_string_new("ora dikenal");
    }
}

/* ==================== Array Functions ==================== */

/* tambah(arr, elem) - Push element to array, returns the array */
static JametValue *fn_tambah(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_ARRAY) {
        return jamet_value_new(JAMET_NONE);
    }

    /* We return a new array with the element appended */
    JametValue *result = jamet_value_copy(args[0]);
    jamet_array_push(result, jamet_value_copy(args[1]));
    return result;
}

/* hapus(arr, index) - Remove element at index, returns new array */
static JametValue *fn_hapus(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_ARRAY || args[1]->type != JAMET_INTEGER) {
        return jamet_value_new(JAMET_NONE);
    }

    long idx = args[1]->as.integer;
    size_t arr_count = args[0]->as.array.count;
    if (idx < 0) idx = (long)arr_count + idx;
    if (idx < 0 || (size_t)idx >= arr_count) {
        return jamet_value_copy(args[0]);
    }

    JametValue *result = jamet_array_new(arr_count > 1 ? arr_count - 1 : 1);
    for (size_t i = 0; i < arr_count; i++) {
        if (i != (size_t)idx) {
            jamet_array_push(result, jamet_value_copy(args[0]->as.array.elements[i]));
        }
    }
    return result;
}

/* gabung(arr1, arr2) - Concatenate two arrays */
static JametValue *fn_gabung(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_ARRAY || args[1]->type != JAMET_ARRAY) {
        return jamet_value_new(JAMET_NONE);
    }

    size_t total = args[0]->as.array.count + args[1]->as.array.count;
    JametValue *result = jamet_array_new(total > 0 ? total : 8);
    for (size_t i = 0; i < args[0]->as.array.count; i++) {
        jamet_array_push(result, jamet_value_copy(args[0]->as.array.elements[i]));
    }
    for (size_t i = 0; i < args[1]->as.array.count; i++) {
        jamet_array_push(result, jamet_value_copy(args[1]->as.array.elements[i]));
    }
    return result;
}

/* balik(arr) - Reverse array */
static JametValue *fn_balik(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_ARRAY) {
        return jamet_value_new(JAMET_NONE);
    }

    size_t n = args[0]->as.array.count;
    JametValue *result = jamet_array_new(n > 0 ? n : 8);
    for (size_t i = 0; i < n; i++) {
        jamet_array_push(result, jamet_value_copy(args[0]->as.array.elements[n - 1 - i]));
    }
    return result;
}

/* Helper comparison for sorting */
static int jamet_compare(const void *a, const void *b) {
    JametValue *va = *(JametValue **)a;
    JametValue *vb = *(JametValue **)b;

    /* Compare integers */
    if (va->type == JAMET_INTEGER && vb->type == JAMET_INTEGER) {
        if (va->as.integer < vb->as.integer) return -1;
        if (va->as.integer > vb->as.integer) return 1;
        return 0;
    }
    /* Compare floats or mixed numeric */
    if ((va->type == JAMET_INTEGER || va->type == JAMET_FLOAT) &&
        (vb->type == JAMET_INTEGER || vb->type == JAMET_FLOAT)) {
        double da = va->type == JAMET_FLOAT ? va->as.float_val : (double)va->as.integer;
        double db = vb->type == JAMET_FLOAT ? vb->as.float_val : (double)vb->as.integer;
        if (da < db) return -1;
        if (da > db) return 1;
        return 0;
    }
    /* Compare strings */
    if (va->type == JAMET_STRING && vb->type == JAMET_STRING) {
        return strcmp(va->as.string.value, vb->as.string.value);
    }
    return 0;
}

/* urutke(arr) - Sort array (ascending) */
static JametValue *fn_urutke(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_ARRAY) {
        return jamet_value_new(JAMET_NONE);
    }

    JametValue *result = jamet_value_copy(args[0]);
    if (result->as.array.count > 1) {
        qsort(result->as.array.elements, result->as.array.count,
              sizeof(JametValue *), jamet_compare);
    }
    return result;
}

/* irisan(arr, mulai, akhir) - Slice array */
static JametValue *fn_irisan(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_ARRAY || args[1]->type != JAMET_INTEGER) {
        return jamet_value_new(JAMET_NONE);
    }

    size_t arr_count = args[0]->as.array.count;
    long start = args[1]->as.integer;
    long end = (count >= 3 && args[2]->type == JAMET_INTEGER) ? args[2]->as.integer : (long)arr_count;

    if (start < 0) start = (long)arr_count + start;
    if (end < 0) end = (long)arr_count + end;
    if (start < 0) start = 0;
    if (end > (long)arr_count) end = (long)arr_count;
    if (start >= end) return jamet_array_new(1);

    JametValue *result = jamet_array_new((size_t)(end - start));
    for (long i = start; i < end; i++) {
        jamet_array_push(result, jamet_value_copy(args[0]->as.array.elements[i]));
    }
    return result;
}

/* indeks(arr, val) - Find index of value in array, returns -1 if not found */
static JametValue *fn_indeks(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_ARRAY) {
        return jamet_integer_new(-1);
    }

    for (size_t i = 0; i < args[0]->as.array.count; i++) {
        if (jamet_value_equals(args[0]->as.array.elements[i], args[1])) {
            return jamet_integer_new((long)i);
        }
    }
    return jamet_integer_new(-1);
}

/* ==================== Utility Functions ==================== */

/* wektu() - Get current time in seconds since epoch */
static JametValue *fn_wektu(JametValue **args, size_t count) {
    (void)args;
    (void)count;
    return jamet_integer_new((long)time(NULL));
}

/* ==================== File I/O Functions ==================== */

/* baca_file(path) - Read entire file as string */
static JametValue *fn_baca_file(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) return jamet_value_new(JAMET_NONE);

    FILE *f = fopen(args[0]->as.string.value, "r");
    if (!f) return jamet_value_new(JAMET_NONE);

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *content = (char *)malloc(fsize + 1);
    fread(content, 1, fsize, f);
    content[fsize] = '\0';
    fclose(f);

    JametValue *result = jamet_string_new(content);
    free(content);
    return result;
}

/* tulis_file(path, content) - Write string to file (overwrite) */
static JametValue *fn_tulis_file(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_STRING || args[1]->type != JAMET_STRING)
        return jamet_boolean_new(0);

    FILE *f = fopen(args[0]->as.string.value, "w");
    if (!f) return jamet_boolean_new(0);

    fprintf(f, "%s", args[1]->as.string.value);
    fclose(f);
    return jamet_boolean_new(1);
}

/* tambah_file(path, content) - Append string to file */
static JametValue *fn_tambah_file(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_STRING || args[1]->type != JAMET_STRING)
        return jamet_boolean_new(0);

    FILE *f = fopen(args[0]->as.string.value, "a");
    if (!f) return jamet_boolean_new(0);

    fprintf(f, "%s", args[1]->as.string.value);
    fclose(f);
    return jamet_boolean_new(1);
}

/* ana_file(path) - Check if file exists */
static JametValue *fn_ana_file(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) return jamet_boolean_new(0);

    FILE *f = fopen(args[0]->as.string.value, "r");
    if (f) {
        fclose(f);
        return jamet_boolean_new(1);
    }
    return jamet_boolean_new(0);
}

/* ==================== HTTP Functions ==================== */

/* Helper: execute curl command and return output */
static char *exec_curl(const char *cmd) {
    FILE *fp = popen(cmd, "r");
    if (!fp) return NULL;

    size_t buf_size = 4096;
    char *buffer = (char *)malloc(buf_size);
    size_t pos = 0;
    char chunk[1024];

    while (fgets(chunk, sizeof(chunk), fp)) {
        size_t chunk_len = strlen(chunk);
        while (pos + chunk_len + 1 > buf_size) {
            buf_size *= 2;
            buffer = (char *)realloc(buffer, buf_size);
        }
        memcpy(buffer + pos, chunk, chunk_len);
        pos += chunk_len;
    }
    buffer[pos] = '\0';
    pclose(fp);
    return buffer;
}

/* http_get(url) - HTTP GET request */
static JametValue *fn_http_get(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) return jamet_value_new(JAMET_NONE);

    size_t cmd_len = strlen(args[0]->as.string.value) + 64;
    char *cmd = (char *)malloc(cmd_len);
    snprintf(cmd, cmd_len, "curl -s \"%s\"", args[0]->as.string.value);

    char *response = exec_curl(cmd);
    free(cmd);

    if (!response) return jamet_value_new(JAMET_NONE);

    JametValue *result = jamet_string_new(response);
    free(response);
    return result;
}

/* http_post(url, body, content_type) - HTTP POST request */
static JametValue *fn_http_post(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_STRING || args[1]->type != JAMET_STRING)
        return jamet_value_new(JAMET_NONE);

    const char *content_type = "application/json";
    if (count >= 3 && args[2]->type == JAMET_STRING) {
        content_type = args[2]->as.string.value;
    }

    size_t cmd_len = strlen(args[0]->as.string.value) + strlen(args[1]->as.string.value) +
                     strlen(content_type) + 128;
    char *cmd = (char *)malloc(cmd_len);
    snprintf(cmd, cmd_len, "curl -s -X POST -H \"Content-Type: %s\" -d '%s' \"%s\"",
             content_type, args[1]->as.string.value, args[0]->as.string.value);

    char *response = exec_curl(cmd);
    free(cmd);

    if (!response) return jamet_value_new(JAMET_NONE);

    JametValue *result = jamet_string_new(response);
    free(response);
    return result;
}

/* ==================== Map Helper Functions ==================== */

/* kunci_map(map) - Get all keys of a map as array */
static JametValue *fn_kunci_map(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_MAP) return jamet_array_new(0);

    JametValue *arr = jamet_array_new(args[0]->as.map.count);
    for (size_t i = 0; i < args[0]->as.map.count; i++) {
        jamet_array_push(arr, jamet_string_new(args[0]->as.map.keys[i]));
    }
    return arr;
}

/* nilai_map(map) - Get all values of a map as array */
static JametValue *fn_nilai_map(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_MAP) return jamet_array_new(0);

    JametValue *arr = jamet_array_new(args[0]->as.map.count);
    for (size_t i = 0; i < args[0]->as.map.count; i++) {
        jamet_array_push(arr, jamet_value_copy(args[0]->as.map.values[i]));
    }
    return arr;
}

/* ana_kunci(map, key) - Check if map has key */
static JametValue *fn_ana_kunci(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_MAP || args[1]->type != JAMET_STRING)
        return jamet_boolean_new(0);

    JametValue *found = jamet_map_get(args[0], args[1]->as.string.value);
    return jamet_boolean_new(found != NULL);
}

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

    /* Array functions */
    {"tambah",       fn_tambah},
    {"hapus",        fn_hapus},
    {"gabung",       fn_gabung},
    {"balik",        fn_balik},
    {"urutke",       fn_urutke},
    {"irisan",       fn_irisan},
    {"indeks",       fn_indeks},

    /* Utility */
    {"wektu",        fn_wektu},

    /* File I/O */
    {"baca_file",    fn_baca_file},
    {"tulis_file",   fn_tulis_file},
    {"tambah_file",  fn_tambah_file},
    {"ana_file",     fn_ana_file},

    /* HTTP */
    {"http_get",     fn_http_get},
    {"http_post",    fn_http_post},

    /* Map helpers */
    {"kunci_map",    fn_kunci_map},
    {"nilai_map",    fn_nilai_map},
    {"ana_kunci",    fn_ana_kunci},

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
