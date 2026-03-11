/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * Basic Data Types Implementation
 */

#include "jamet_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Mbuang memori yen kesalahan */
#define EXIT_IF_NULL(ptr, msg) \
    if ((ptr) == NULL) { \
        fprintf(stderr, "Kesalahan: %s\n", (msg)); \
        exit(1); \
    }

/* Mbuat JametValue anyar */
JametValue *jamet_value_new(JametType type) {
    JametValue *value = (JametValue *)malloc(sizeof(JametValue));
    EXIT_IF_NULL(value, "Ora bisa mbenakna memori kanggo JametValue");

    value->type = type;

    /* Inisialisasi default gumantung jinise */
    switch (type) {
        case JAMET_NONE:
            value->as.ptr = NULL;
            break;
        case JAMET_INTEGER:
            value->as.integer = 0;
            break;
        case JAMET_FLOAT:
            value->as.float_val = 0.0;
            break;
        case JAMET_BOOLEAN:
            value->as.boolean = 0;
            break;
        case JAMET_STRING:
            value->as.string.value = NULL;
            value->as.string.length = 0;
            break;
        case JAMET_ARRAY:
            value->as.array.elements = NULL;
            value->as.array.count = 0;
            value->as.array.capacity = 0;
            break;
        default:
            value->as.ptr = NULL;
            break;
    }

    return value;
}

/* Mbebasna memori JametValue */
void jamet_value_free(JametValue *value) {
    if (value == NULL) return;

    switch (value->type) {
        case JAMET_STRING:
            if (value->as.string.value != NULL) {
                free(value->as.string.value);
            }
            break;
        case JAMET_ARRAY:
            if (value->as.array.elements != NULL) {
                /* Mbebasna kabeh elemen array */
                for (size_t i = 0; i < value->as.array.count; i++) {
                    jamet_value_free(value->as.array.elements[i]);
                }
                free(value->as.array.elements);
            }
            break;
        default:
            break;
    }

    free(value);
}

/* Nyalin JametValue */
JametValue *jamet_value_copy(JametValue *value) {
    if (value == NULL) return NULL;

    JametValue *copy = jamet_value_new(value->type);

    switch (value->type) {
        case JAMET_NONE:
            copy->as.ptr = NULL;
            break;
        case JAMET_INTEGER:
            copy->as.integer = value->as.integer;
            break;
        case JAMET_FLOAT:
            copy->as.float_val = value->as.float_val;
            break;
        case JAMET_BOOLEAN:
            copy->as.boolean = value->as.boolean;
            break;
        case JAMET_STRING:
            copy->as.string.length = value->as.string.length;
            copy->as.string.value = (char *)malloc(value->as.string.length + 1);
            EXIT_IF_NULL(copy->as.string.value, "Ora bisa nyalin string");
            strncpy(copy->as.string.value, value->as.string.value, value->as.string.length);
            copy->as.string.value[value->as.string.length] = '\0';
            break;
        case JAMET_ARRAY:
            copy->as.array.count = value->as.array.count;
            copy->as.array.capacity = value->as.array.capacity > 0 ? value->as.array.capacity : 8;
            copy->as.array.elements = (JametValue **)malloc(
                sizeof(JametValue *) * copy->as.array.capacity
            );
            EXIT_IF_NULL(copy->as.array.elements, "Ora bisa nyalin array");
            for (size_t i = 0; i < value->as.array.count; i++) {
                copy->as.array.elements[i] = jamet_value_copy(value->as.array.elements[i]);
            }
            break;
        default:
            copy->as.ptr = value->as.ptr;
            break;
    }

    return copy;
}

/* Mbuat integer anyar */
JametValue *jamet_integer_new(long value) {
    JametValue *v = jamet_value_new(JAMET_INTEGER);
    v->as.integer = value;
    return v;
}

/* Mbuat float anyar */
JametValue *jamet_float_new(double value) {
    JametValue *v = jamet_value_new(JAMET_FLOAT);
    v->as.float_val = value;
    return v;
}

/* Mbuat string anyar */
JametValue *jamet_string_new(const char *value) {
    JametValue *v = jamet_value_new(JAMET_STRING);
    if (value != NULL) {
        v->as.string.length = strlen(value);
        v->as.string.value = (char *)malloc(v->as.string.length + 1);
        EXIT_IF_NULL(v->as.string.value, "Ora bisa mbenakna memori string");
        strcpy(v->as.string.value, value);
    }
    return v;
}

/* Mbuat boolean anyar */
JametValue *jamet_boolean_new(int value) {
    JametValue *v = jamet_value_new(JAMET_BOOLEAN);
    v->as.boolean = value ? 1 : 0;
    return v;
}

/* Mbuat array anyar */
JametValue *jamet_array_new(size_t capacity) {
    JametValue *v = jamet_value_new(JAMET_ARRAY);
    v->as.array.capacity = capacity > 0 ? capacity : 8;
    v->as.array.count = 0;
    v->as.array.elements = (JametValue **)malloc(
        sizeof(JametValue *) * v->as.array.capacity
    );
    EXIT_IF_NULL(v->as.array.elements, "Ora bisa mbenakna memori array");
    return v;
}

/* Nambah elemen ing array */
void jamet_array_push(JametValue *array, JametValue *element) {
    if (array == NULL || array->type != JAMET_ARRAY) return;
    if (element == NULL) return;

    /* Nambah kapasitas yen perlu */
    if (array->as.array.count >= array->as.array.capacity) {
        array->as.array.capacity *= 2;
        array->as.array.elements = (JametValue **)realloc(
            array->as.array.elements,
            sizeof(JametValue *) * array->as.array.capacity
        );
        EXIT_IF_NULL(array->as.array.elements, "Ora bisa nambah kapasitas array");
    }

    array->as.array.elements[array->as.array.count++] = element;
}

/* Ngonversi JametValue dadi string */
char *jamet_value_to_string(JametValue *value) {
    if (value == NULL) {
        char *result = (char *)malloc(7);
        strcpy(result, "kosong");
        return result;
    }

    char *buffer = NULL;
    int len = 0;

    switch (value->type) {
        case JAMET_NONE:
            len = snprintf(NULL, 0, "kosong");
            buffer = (char *)malloc(len + 1);
            snprintf(buffer, len + 1, "kosong");
            break;

        case JAMET_INTEGER:
            len = snprintf(NULL, 0, "%ld", value->as.integer);
            buffer = (char *)malloc(len + 1);
            snprintf(buffer, len + 1, "%ld", value->as.integer);
            break;

        case JAMET_FLOAT:
            len = snprintf(NULL, 0, "%g", value->as.float_val);
            buffer = (char *)malloc(len + 1);
            snprintf(buffer, len + 1, "%g", value->as.float_val);
            break;

        case JAMET_BOOLEAN:
            len = snprintf(NULL, 0, "%s", value->as.boolean ? "bener" : "salah");
            buffer = (char *)malloc(len + 1);
            snprintf(buffer, len + 1, "%s", value->as.boolean ? "bener" : "salah");
            break;

        case JAMET_STRING:
            len = snprintf(NULL, 0, "\"%s\"", value->as.string.value);
            buffer = (char *)malloc(len + 1);
            snprintf(buffer, len + 1, "\"%s\"", value->as.string.value);
            break;

        case JAMET_ARRAY: {
            /* Build string like [elem1, elem2, ...] */
            size_t buf_size = 256;
            buffer = (char *)malloc(buf_size);
            buffer[0] = '[';
            size_t pos = 1;
            for (size_t i = 0; i < value->as.array.count; i++) {
                char *elem_str = jamet_value_to_string(value->as.array.elements[i]);
                size_t elem_len = strlen(elem_str);
                size_t sep_len = (i > 0) ? 2 : 0;
                /* Grow buffer if needed */
                while (pos + elem_len + sep_len + 2 > buf_size) {
                    buf_size *= 2;
                    buffer = (char *)realloc(buffer, buf_size);
                }
                if (i > 0) { buffer[pos++] = ','; buffer[pos++] = ' '; }
                memcpy(buffer + pos, elem_str, elem_len);
                pos += elem_len;
                free(elem_str);
            }
            buffer[pos++] = ']';
            buffer[pos] = '\0';
            break;
        }

        default:
            len = snprintf(NULL, 0, "(jinis ora dikenal)");
            buffer = (char *)malloc(len + 1);
            snprintf(buffer, len + 1, "(jinis ora dikenal)");
            break;
    }

    return buffer;
}

/* Mbandingake loro JametValue */
int jamet_value_equals(JametValue *a, JametValue *b) {
    if (a == NULL || b == NULL) return a == b;

    if (a->type != b->type) return 0;

    switch (a->type) {
        case JAMET_NONE:
            return 1;
        case JAMET_INTEGER:
            return a->as.integer == b->as.integer;
        case JAMET_FLOAT:
            return a->as.float_val == b->as.float_val;
        case JAMET_BOOLEAN:
            return a->as.boolean == b->as.boolean;
        case JAMET_STRING:
            if (a->as.string.length != b->as.string.length) return 0;
            return strncmp(a->as.string.value, b->as.string.value, a->as.string.length) == 0;
        default:
            return 0;
    }
}
