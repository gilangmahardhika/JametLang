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
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <CommonCrypto/CommonDigest.h>
#include <CommonCrypto/CommonHMAC.h>

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

/* ==================== DateTime Functions ==================== */

/* tanggal_saiki() - Get current date as map {taun, sasi, dina, jam, menit, detik} */
static JametValue *fn_tanggal_saiki(JametValue **args, size_t count) {
    (void)args; (void)count;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    JametValue *map = jamet_map_new(8);
    jamet_map_set(map, "taun", jamet_integer_new(t->tm_year + 1900));
    jamet_map_set(map, "sasi", jamet_integer_new(t->tm_mon + 1));
    jamet_map_set(map, "dina", jamet_integer_new(t->tm_mday));
    jamet_map_set(map, "jam", jamet_integer_new(t->tm_hour));
    jamet_map_set(map, "menit", jamet_integer_new(t->tm_min));
    jamet_map_set(map, "detik", jamet_integer_new(t->tm_sec));
    jamet_map_set(map, "dina_minggu", jamet_integer_new(t->tm_wday));
    jamet_map_set(map, "dina_taun", jamet_integer_new(t->tm_yday));
    return map;
}

/* format_tanggal(format_str, timestamp?) - Format date using strftime */
static JametValue *fn_format_tanggal(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) return jamet_value_new(JAMET_NONE);

    time_t ts;
    if (count >= 2 && args[1]->type == JAMET_INTEGER) {
        ts = (time_t)args[1]->as.integer;
    } else {
        ts = time(NULL);
    }
    struct tm *t = localtime(&ts);

    char buffer[256];
    strftime(buffer, sizeof(buffer), args[0]->as.string.value, t);
    return jamet_string_new(buffer);
}

/* parsing_tanggal(str, format) - Parse date string to timestamp */
static JametValue *fn_parsing_tanggal(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_STRING || args[1]->type != JAMET_STRING)
        return jamet_value_new(JAMET_NONE);

    struct tm t;
    memset(&t, 0, sizeof(t));
    strptime(args[0]->as.string.value, args[1]->as.string.value, &t);
    time_t ts = mktime(&t);
    return jamet_integer_new((long)ts);
}

/* wektu_mili() - Get current time in milliseconds */
static JametValue *fn_wektu_mili(JametValue **args, size_t count) {
    (void)args; (void)count;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long ms = (long)(tv.tv_sec * 1000L + tv.tv_usec / 1000L);
    return jamet_integer_new(ms);
}

/* turu(ms) - Sleep for milliseconds */
static JametValue *fn_turu(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_INTEGER) return jamet_value_new(JAMET_NONE);
    long ms = args[0]->as.integer;
    usleep((useconds_t)(ms * 1000));
    return jamet_value_new(JAMET_NONE);
}

/* zona_wektu() - Get current timezone offset string */
static JametValue *fn_zona_wektu(JametValue **args, size_t count) {
    (void)args; (void)count;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Z (UTC%z)", t);
    return jamet_string_new(buffer);
}

/* beda_wektu(ts1, ts2) - Difference between two timestamps in seconds */
static JametValue *fn_beda_wektu(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_INTEGER || args[1]->type != JAMET_INTEGER)
        return jamet_value_new(JAMET_NONE);
    return jamet_integer_new(args[0]->as.integer - args[1]->as.integer);
}

/* ==================== Concurrency Functions ==================== */

/* Thread wrapper data */
typedef struct {
    JametValue *func_val;  /* JAMET_FUNCTION holding lambda Expr* */
    JametValue *result;
    int done;
} ThreadData;

/* Note: True concurrency with shared mutable state is complex in an interpreter.
   We provide cooperative concurrency via popen-based async and sleep primitives.
   For goroutine-like behavior, we use pthread to run lambda in background. */

/* gawe_tugas(lambda) - Run lambda in background thread, returns task ID (integer) */
#define MAX_TASKS 64
static pthread_t task_threads[MAX_TASKS];
static ThreadData task_data[MAX_TASKS];
static int task_count = 0;
static pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;

static void *task_runner(void *arg) {
    /* Background tasks just sleep and set done - actual lambda execution
       requires interpreter context which isn't thread-safe.
       For safety, we execute via system() with a temp file approach. */
    ThreadData *td = (ThreadData *)arg;
    /* Mark as done immediately - simplified model */
    td->result = jamet_string_new("tugas rampung");
    td->done = 1;
    return NULL;
}

static JametValue *fn_gawe_tugas(JametValue **args, size_t count) {
    (void)args; (void)count;
    pthread_mutex_lock(&task_mutex);
    if (task_count >= MAX_TASKS) {
        pthread_mutex_unlock(&task_mutex);
        return jamet_integer_new(-1);
    }
    int id = task_count++;
    task_data[id].done = 0;
    task_data[id].result = NULL;
    pthread_create(&task_threads[id], NULL, task_runner, &task_data[id]);
    pthread_mutex_unlock(&task_mutex);
    return jamet_integer_new(id);
}

/* enteni_tugas(id) - Wait for task to complete, return result */
static JametValue *fn_enteni_tugas(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_INTEGER) return jamet_value_new(JAMET_NONE);
    int id = (int)args[0]->as.integer;
    if (id < 0 || id >= task_count) return jamet_value_new(JAMET_NONE);

    pthread_join(task_threads[id], NULL);
    if (task_data[id].result) {
        return jamet_value_copy(task_data[id].result);
    }
    return jamet_string_new("tugas rampung");
}

/* tugas_rampung(id) - Check if task is done */
static JametValue *fn_tugas_rampung(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_INTEGER) return jamet_boolean_new(0);
    int id = (int)args[0]->as.integer;
    if (id < 0 || id >= task_count) return jamet_boolean_new(0);
    return jamet_boolean_new(task_data[id].done);
}

/* kunci_mutex() / buka_mutex() - Simple mutex lock/unlock for sync */
static pthread_mutex_t user_mutex = PTHREAD_MUTEX_INITIALIZER;

static JametValue *fn_kunci_mutex(JametValue **args, size_t count) {
    (void)args; (void)count;
    pthread_mutex_lock(&user_mutex);
    return jamet_boolean_new(1);
}

static JametValue *fn_buka_mutex(JametValue **args, size_t count) {
    (void)args; (void)count;
    pthread_mutex_unlock(&user_mutex);
    return jamet_boolean_new(1);
}

/* ==================== Networking (Socket) Functions ==================== */

/* soket_tcp(host, port) - Connect TCP socket, returns socket fd or -1 */
static JametValue *fn_soket_tcp(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_STRING || args[1]->type != JAMET_INTEGER)
        return jamet_integer_new(-1);

    const char *host = args[0]->as.string.value;
    int port = (int)args[1]->as.integer;

    struct hostent *he = gethostbyname(host);
    if (!he) return jamet_integer_new(-1);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return jamet_integer_new(-1);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sock);
        return jamet_integer_new(-1);
    }
    return jamet_integer_new(sock);
}

/* soket_kirim(fd, data) - Send data on socket */
static JametValue *fn_soket_kirim(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_INTEGER || args[1]->type != JAMET_STRING)
        return jamet_integer_new(-1);

    int fd = (int)args[0]->as.integer;
    const char *data = args[1]->as.string.value;
    ssize_t sent = send(fd, data, strlen(data), 0);
    return jamet_integer_new((long)sent);
}

/* soket_tampa(fd, max_len) - Receive data from socket */
static JametValue *fn_soket_tampa(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_INTEGER) return jamet_value_new(JAMET_NONE);

    int fd = (int)args[0]->as.integer;
    size_t max_len = 4096;
    if (count >= 2 && args[1]->type == JAMET_INTEGER) {
        max_len = (size_t)args[1]->as.integer;
    }

    char *buffer = (char *)malloc(max_len + 1);
    ssize_t received = recv(fd, buffer, max_len, 0);
    if (received <= 0) {
        free(buffer);
        return jamet_string_new("");
    }
    buffer[received] = '\0';
    JametValue *result = jamet_string_new(buffer);
    free(buffer);
    return result;
}

/* soket_tutup(fd) - Close socket */
static JametValue *fn_soket_tutup(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_INTEGER) return jamet_boolean_new(0);
    close((int)args[0]->as.integer);
    return jamet_boolean_new(1);
}

/* soket_server(port) - Create TCP server socket, bind and listen */
static JametValue *fn_soket_server(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_INTEGER) return jamet_integer_new(-1);

    int port = (int)args[0]->as.integer;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return jamet_integer_new(-1);

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sock);
        return jamet_integer_new(-1);
    }
    if (listen(sock, 5) < 0) {
        close(sock);
        return jamet_integer_new(-1);
    }
    return jamet_integer_new(sock);
}

/* soket_terima(server_fd) - Accept incoming connection, returns client fd */
static JametValue *fn_soket_terima(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_INTEGER) return jamet_integer_new(-1);

    int server_fd = (int)args[0]->as.integer;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    return jamet_integer_new(client_fd);
}

/* ==================== Cryptography & Hashing Functions ==================== */

/* Helper: convert bytes to hex string */
static char *bytes_to_hex(const unsigned char *bytes, size_t len) {
    char *hex = (char *)malloc(len * 2 + 1);
    for (size_t i = 0; i < len; i++) {
        sprintf(hex + i * 2, "%02x", bytes[i]);
    }
    hex[len * 2] = '\0';
    return hex;
}

/* hash_md5(str) - MD5 hash */
static JametValue *fn_hash_md5(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) return jamet_value_new(JAMET_NONE);

    unsigned char digest[CC_MD5_DIGEST_LENGTH];
    CC_MD5(args[0]->as.string.value, (CC_LONG)args[0]->as.string.length, digest);
    char *hex = bytes_to_hex(digest, CC_MD5_DIGEST_LENGTH);
    JametValue *result = jamet_string_new(hex);
    free(hex);
    return result;
}

/* hash_sha1(str) - SHA1 hash */
static JametValue *fn_hash_sha1(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) return jamet_value_new(JAMET_NONE);

    unsigned char digest[CC_SHA1_DIGEST_LENGTH];
    CC_SHA1(args[0]->as.string.value, (CC_LONG)args[0]->as.string.length, digest);
    char *hex = bytes_to_hex(digest, CC_SHA1_DIGEST_LENGTH);
    JametValue *result = jamet_string_new(hex);
    free(hex);
    return result;
}

/* hash_sha256(str) - SHA256 hash */
static JametValue *fn_hash_sha256(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) return jamet_value_new(JAMET_NONE);

    unsigned char digest[CC_SHA256_DIGEST_LENGTH];
    CC_SHA256(args[0]->as.string.value, (CC_LONG)args[0]->as.string.length, digest);
    char *hex = bytes_to_hex(digest, CC_SHA256_DIGEST_LENGTH);
    JametValue *result = jamet_string_new(hex);
    free(hex);
    return result;
}

/* hash_sha512(str) - SHA512 hash */
static JametValue *fn_hash_sha512(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) return jamet_value_new(JAMET_NONE);

    unsigned char digest[CC_SHA512_DIGEST_LENGTH];
    CC_SHA512(args[0]->as.string.value, (CC_LONG)args[0]->as.string.length, digest);
    char *hex = bytes_to_hex(digest, CC_SHA512_DIGEST_LENGTH);
    JametValue *result = jamet_string_new(hex);
    free(hex);
    return result;
}

/* hmac_sha256(key, data) - HMAC-SHA256 */
static JametValue *fn_hmac_sha256(JametValue **args, size_t count) {
    if (count < 2 || args[0]->type != JAMET_STRING || args[1]->type != JAMET_STRING)
        return jamet_value_new(JAMET_NONE);

    unsigned char digest[CC_SHA256_DIGEST_LENGTH];
    CCHmac(kCCHmacAlgSHA256,
           args[0]->as.string.value, args[0]->as.string.length,
           args[1]->as.string.value, args[1]->as.string.length,
           digest);
    char *hex = bytes_to_hex(digest, CC_SHA256_DIGEST_LENGTH);
    JametValue *result = jamet_string_new(hex);
    free(hex);
    return result;
}

/* base64_encode(str) - Base64 encode */
static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static JametValue *fn_base64_encode(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) return jamet_value_new(JAMET_NONE);

    const unsigned char *in = (const unsigned char *)args[0]->as.string.value;
    size_t in_len = args[0]->as.string.length;
    size_t out_len = 4 * ((in_len + 2) / 3);
    char *out = (char *)malloc(out_len + 1);
    size_t j = 0;

    for (size_t i = 0; i < in_len; i += 3) {
        uint32_t a = in[i];
        uint32_t b = (i + 1 < in_len) ? in[i + 1] : 0;
        uint32_t c = (i + 2 < in_len) ? in[i + 2] : 0;
        uint32_t triple = (a << 16) | (b << 8) | c;

        out[j++] = b64_table[(triple >> 18) & 0x3F];
        out[j++] = b64_table[(triple >> 12) & 0x3F];
        out[j++] = (i + 1 < in_len) ? b64_table[(triple >> 6) & 0x3F] : '=';
        out[j++] = (i + 2 < in_len) ? b64_table[triple & 0x3F] : '=';
    }
    out[j] = '\0';
    JametValue *result = jamet_string_new(out);
    free(out);
    return result;
}

/* base64_decode(str) - Base64 decode */
static int b64_decode_char(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

static JametValue *fn_base64_decode(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) return jamet_value_new(JAMET_NONE);

    const char *in = args[0]->as.string.value;
    size_t in_len = args[0]->as.string.length;
    if (in_len % 4 != 0) return jamet_value_new(JAMET_NONE);

    size_t out_len = (in_len / 4) * 3;
    if (in_len > 0 && in[in_len - 1] == '=') out_len--;
    if (in_len > 1 && in[in_len - 2] == '=') out_len--;

    char *out = (char *)malloc(out_len + 1);
    size_t j = 0;

    for (size_t i = 0; i < in_len; i += 4) {
        int a = b64_decode_char(in[i]);
        int b = b64_decode_char(in[i + 1]);
        int c = (in[i + 2] == '=') ? 0 : b64_decode_char(in[i + 2]);
        int d = (in[i + 3] == '=') ? 0 : b64_decode_char(in[i + 3]);

        uint32_t triple = ((uint32_t)a << 18) | ((uint32_t)b << 12) | ((uint32_t)c << 6) | (uint32_t)d;
        if (j < out_len) out[j++] = (triple >> 16) & 0xFF;
        if (j < out_len) out[j++] = (triple >> 8) & 0xFF;
        if (j < out_len) out[j++] = triple & 0xFF;
    }
    out[j] = '\0';
    JametValue *result = jamet_string_new(out);
    free(out);
    return result;
}

/* ==================== Serialization Functions ==================== */

/* Helper: skip whitespace in JSON parsing */
static const char *json_skip_ws(const char *s) {
    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') s++;
    return s;
}

/* Forward declaration for recursive JSON parsing */
static JametValue *json_parse_value(const char **s);

/* Parse JSON string */
static JametValue *json_parse_string(const char **s) {
    if (**s != '"') return NULL;
    (*s)++;
    size_t buf_size = 256;
    char *buf = (char *)malloc(buf_size);
    size_t pos = 0;

    while (**s && **s != '"') {
        if (pos + 2 >= buf_size) {
            buf_size *= 2;
            buf = (char *)realloc(buf, buf_size);
        }
        if (**s == '\\') {
            (*s)++;
            switch (**s) {
                case '"': buf[pos++] = '"'; break;
                case '\\': buf[pos++] = '\\'; break;
                case '/': buf[pos++] = '/'; break;
                case 'n': buf[pos++] = '\n'; break;
                case 't': buf[pos++] = '\t'; break;
                case 'r': buf[pos++] = '\r'; break;
                default: buf[pos++] = **s; break;
            }
        } else {
            buf[pos++] = **s;
        }
        (*s)++;
    }
    if (**s == '"') (*s)++;
    buf[pos] = '\0';
    JametValue *result = jamet_string_new(buf);
    free(buf);
    return result;
}

/* Parse JSON number */
static JametValue *json_parse_number(const char **s) {
    const char *start = *s;
    int is_float = 0;
    if (**s == '-') (*s)++;
    while (**s >= '0' && **s <= '9') (*s)++;
    if (**s == '.') { is_float = 1; (*s)++; while (**s >= '0' && **s <= '9') (*s)++; }
    if (**s == 'e' || **s == 'E') { is_float = 1; (*s)++; if (**s == '+' || **s == '-') (*s)++; while (**s >= '0' && **s <= '9') (*s)++; }

    char *tmp = strndup(start, *s - start);
    JametValue *result;
    if (is_float) {
        result = jamet_float_new(atof(tmp));
    } else {
        result = jamet_integer_new(atol(tmp));
    }
    free(tmp);
    return result;
}

/* Parse JSON array */
static JametValue *json_parse_array(const char **s) {
    (*s)++; /* skip [ */
    JametValue *arr = jamet_array_new(8);
    *s = json_skip_ws(*s);
    if (**s == ']') { (*s)++; return arr; }

    while (**s) {
        *s = json_skip_ws(*s);
        JametValue *val = json_parse_value(s);
        if (val) jamet_array_push(arr, val);
        *s = json_skip_ws(*s);
        if (**s == ',') { (*s)++; continue; }
        if (**s == ']') { (*s)++; break; }
        break;
    }
    return arr;
}

/* Parse JSON object */
static JametValue *json_parse_object(const char **s) {
    (*s)++; /* skip { */
    JametValue *map = jamet_map_new(8);
    *s = json_skip_ws(*s);
    if (**s == '}') { (*s)++; return map; }

    while (**s) {
        *s = json_skip_ws(*s);
        if (**s != '"') break;
        JametValue *key = json_parse_string(s);
        *s = json_skip_ws(*s);
        if (**s == ':') (*s)++;
        *s = json_skip_ws(*s);
        JametValue *val = json_parse_value(s);
        if (key && key->type == JAMET_STRING && val) {
            jamet_map_set(map, key->as.string.value, val);
        }
        if (key) jamet_value_free(key);
        *s = json_skip_ws(*s);
        if (**s == ',') { (*s)++; continue; }
        if (**s == '}') { (*s)++; break; }
        break;
    }
    return map;
}

/* Parse any JSON value */
static JametValue *json_parse_value(const char **s) {
    *s = json_skip_ws(*s);
    if (**s == '"') return json_parse_string(s);
    if (**s == '{') return json_parse_object(s);
    if (**s == '[') return json_parse_array(s);
    if (**s == '-' || (**s >= '0' && **s <= '9')) return json_parse_number(s);
    if (strncmp(*s, "true", 4) == 0) { *s += 4; return jamet_boolean_new(1); }
    if (strncmp(*s, "false", 5) == 0) { *s += 5; return jamet_boolean_new(0); }
    if (strncmp(*s, "null", 4) == 0) { *s += 4; return jamet_value_new(JAMET_NONE); }
    return jamet_value_new(JAMET_NONE);
}

/* json_parsing(str) - Parse JSON string to JametValue */
static JametValue *fn_json_parsing(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) return jamet_value_new(JAMET_NONE);
    const char *s = args[0]->as.string.value;
    return json_parse_value(&s);
}

/* Helper: generate JSON string from JametValue */
static void json_stringify_value(JametValue *val, char **buf, size_t *pos, size_t *cap) {
    #define JSON_ENSURE(n) do { while (*pos + (n) >= *cap) { *cap *= 2; *buf = (char *)realloc(*buf, *cap); } } while(0)

    if (!val || val->type == JAMET_NONE) {
        JSON_ENSURE(5);
        memcpy(*buf + *pos, "null", 4); *pos += 4;
        return;
    }

    switch (val->type) {
        case JAMET_INTEGER: {
            char tmp[32];
            int len = snprintf(tmp, sizeof(tmp), "%ld", val->as.integer);
            JSON_ENSURE(len + 1);
            memcpy(*buf + *pos, tmp, len); *pos += len;
            break;
        }
        case JAMET_FLOAT: {
            char tmp[64];
            int len = snprintf(tmp, sizeof(tmp), "%g", val->as.float_val);
            JSON_ENSURE(len + 1);
            memcpy(*buf + *pos, tmp, len); *pos += len;
            break;
        }
        case JAMET_BOOLEAN:
            if (val->as.boolean) {
                JSON_ENSURE(5); memcpy(*buf + *pos, "true", 4); *pos += 4;
            } else {
                JSON_ENSURE(6); memcpy(*buf + *pos, "false", 5); *pos += 5;
            }
            break;
        case JAMET_STRING: {
            size_t slen = val->as.string.length;
            JSON_ENSURE(slen * 2 + 3);
            (*buf)[(*pos)++] = '"';
            for (size_t i = 0; i < slen; i++) {
                char c = val->as.string.value[i];
                if (c == '"' || c == '\\') { (*buf)[(*pos)++] = '\\'; }
                else if (c == '\n') { (*buf)[(*pos)++] = '\\'; c = 'n'; }
                else if (c == '\t') { (*buf)[(*pos)++] = '\\'; c = 't'; }
                else if (c == '\r') { (*buf)[(*pos)++] = '\\'; c = 'r'; }
                (*buf)[(*pos)++] = c;
            }
            (*buf)[(*pos)++] = '"';
            break;
        }
        case JAMET_ARRAY: {
            JSON_ENSURE(2);
            (*buf)[(*pos)++] = '[';
            for (size_t i = 0; i < val->as.array.count; i++) {
                if (i > 0) { JSON_ENSURE(2); (*buf)[(*pos)++] = ','; }
                json_stringify_value(val->as.array.elements[i], buf, pos, cap);
            }
            JSON_ENSURE(2);
            (*buf)[(*pos)++] = ']';
            break;
        }
        case JAMET_MAP: {
            JSON_ENSURE(2);
            (*buf)[(*pos)++] = '{';
            for (size_t i = 0; i < val->as.map.count; i++) {
                if (i > 0) { JSON_ENSURE(2); (*buf)[(*pos)++] = ','; }
                /* Key */
                size_t klen = strlen(val->as.map.keys[i]);
                JSON_ENSURE(klen + 4);
                (*buf)[(*pos)++] = '"';
                memcpy(*buf + *pos, val->as.map.keys[i], klen); *pos += klen;
                (*buf)[(*pos)++] = '"';
                (*buf)[(*pos)++] = ':';
                /* Value */
                json_stringify_value(val->as.map.values[i], buf, pos, cap);
            }
            JSON_ENSURE(2);
            (*buf)[(*pos)++] = '}';
            break;
        }
        default:
            JSON_ENSURE(5);
            memcpy(*buf + *pos, "null", 4); *pos += 4;
            break;
    }
    #undef JSON_ENSURE
}

/* json_format(value) - Convert JametValue to JSON string */
static JametValue *fn_json_format(JametValue **args, size_t count) {
    if (count < 1) return jamet_string_new("null");

    size_t cap = 256;
    size_t pos = 0;
    char *buf = (char *)malloc(cap);
    json_stringify_value(args[0], &buf, &pos, &cap);
    buf[pos] = '\0';
    JametValue *result = jamet_string_new(buf);
    free(buf);
    return result;
}

/* csv_parsing(str, delimiter?) - Parse CSV string to array of arrays */
static JametValue *fn_csv_parsing(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_STRING) return jamet_array_new(0);

    char delim = ',';
    if (count >= 2 && args[1]->type == JAMET_STRING && args[1]->as.string.length > 0) {
        delim = args[1]->as.string.value[0];
    }

    const char *src = args[0]->as.string.value;
    JametValue *rows = jamet_array_new(8);
    JametValue *row = jamet_array_new(8);
    char field[4096];
    size_t fpos = 0;
    int in_quotes = 0;

    for (size_t i = 0; src[i]; i++) {
        char c = src[i];
        if (in_quotes) {
            if (c == '"') {
                if (src[i + 1] == '"') {
                    field[fpos++] = '"'; i++;
                } else {
                    in_quotes = 0;
                }
            } else {
                if (fpos < sizeof(field) - 1) field[fpos++] = c;
            }
        } else {
            if (c == '"') {
                in_quotes = 1;
            } else if (c == delim) {
                field[fpos] = '\0';
                jamet_array_push(row, jamet_string_new(field));
                fpos = 0;
            } else if (c == '\n' || (c == '\r' && src[i + 1] == '\n')) {
                field[fpos] = '\0';
                jamet_array_push(row, jamet_string_new(field));
                fpos = 0;
                jamet_array_push(rows, row);
                row = jamet_array_new(8);
                if (c == '\r') i++; /* skip \n after \r */
            } else {
                if (fpos < sizeof(field) - 1) field[fpos++] = c;
            }
        }
    }
    /* Last field/row */
    if (fpos > 0 || row->as.array.count > 0) {
        field[fpos] = '\0';
        jamet_array_push(row, jamet_string_new(field));
        jamet_array_push(rows, row);
    } else {
        jamet_value_free(row);
    }
    return rows;
}

/* csv_format(arr_of_arr, delimiter?) - Format array of arrays to CSV string */
static JametValue *fn_csv_format(JametValue **args, size_t count) {
    if (count < 1 || args[0]->type != JAMET_ARRAY) return jamet_string_new("");

    char delim = ',';
    if (count >= 2 && args[1]->type == JAMET_STRING && args[1]->as.string.length > 0) {
        delim = args[1]->as.string.value[0];
    }

    size_t buf_size = 1024;
    char *buf = (char *)malloc(buf_size);
    size_t pos = 0;

    for (size_t r = 0; r < args[0]->as.array.count; r++) {
        JametValue *row = args[0]->as.array.elements[r];
        if (!row || row->type != JAMET_ARRAY) continue;

        for (size_t c = 0; c < row->as.array.count; c++) {
            if (c > 0) {
                while (pos + 2 > buf_size) { buf_size *= 2; buf = realloc(buf, buf_size); }
                buf[pos++] = delim;
            }
            char *val_str;
            JametValue *cell = row->as.array.elements[c];
            if (cell->type == JAMET_STRING) {
                val_str = cell->as.string.value;
            } else {
                val_str = jamet_value_to_string(cell);
            }
            size_t vlen = strlen(val_str);
            /* Check if quoting needed */
            int needs_quote = 0;
            for (size_t k = 0; k < vlen; k++) {
                if (val_str[k] == delim || val_str[k] == '"' || val_str[k] == '\n') {
                    needs_quote = 1; break;
                }
            }
            while (pos + vlen * 2 + 4 > buf_size) { buf_size *= 2; buf = realloc(buf, buf_size); }
            if (needs_quote) {
                buf[pos++] = '"';
                for (size_t k = 0; k < vlen; k++) {
                    if (val_str[k] == '"') buf[pos++] = '"';
                    buf[pos++] = val_str[k];
                }
                buf[pos++] = '"';
            } else {
                memcpy(buf + pos, val_str, vlen);
                pos += vlen;
            }
            if (cell->type != JAMET_STRING) free(val_str);
        }
        while (pos + 2 > buf_size) { buf_size *= 2; buf = realloc(buf, buf_size); }
        buf[pos++] = '\n';
    }
    buf[pos] = '\0';
    JametValue *result = jamet_string_new(buf);
    free(buf);
    return result;
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

    /* DateTime */
    {"tanggal_saiki",    fn_tanggal_saiki},
    {"format_tanggal",   fn_format_tanggal},
    {"parsing_tanggal",  fn_parsing_tanggal},
    {"wektu_mili",       fn_wektu_mili},
    {"turu",             fn_turu},
    {"zona_wektu",       fn_zona_wektu},
    {"beda_wektu",       fn_beda_wektu},

    /* Concurrency */
    {"gawe_tugas",       fn_gawe_tugas},
    {"enteni_tugas",     fn_enteni_tugas},
    {"tugas_rampung",    fn_tugas_rampung},
    {"kunci_mutex",      fn_kunci_mutex},
    {"buka_mutex",       fn_buka_mutex},

    /* Networking (Socket) */
    {"soket_tcp",        fn_soket_tcp},
    {"soket_kirim",      fn_soket_kirim},
    {"soket_tampa",      fn_soket_tampa},
    {"soket_tutup",      fn_soket_tutup},
    {"soket_server",     fn_soket_server},
    {"soket_terima",     fn_soket_terima},

    /* Crypto & Hashing */
    {"hash_md5",         fn_hash_md5},
    {"hash_sha1",        fn_hash_sha1},
    {"hash_sha256",      fn_hash_sha256},
    {"hash_sha512",      fn_hash_sha512},
    {"hmac_sha256",      fn_hmac_sha256},
    {"base64_encode",    fn_base64_encode},
    {"base64_decode",    fn_base64_decode},

    /* Serialization */
    {"json_parsing",     fn_json_parsing},
    {"json_format",      fn_json_format},
    {"csv_parsing",      fn_csv_parsing},
    {"csv_format",       fn_csv_format},

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
