/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * Standard Library Header
 */

#ifndef STDLIB_JAMET_H
#define STDLIB_JAMET_H

#include "jamet_types.h"

/*
 * Try to call a standard library function by name.
 * Returns the result JametValue* if the function is found and executed.
 * Returns NULL if the function name is not a stdlib function.
 *
 * Caller is responsible for freeing the returned JametValue.
 */
JametValue *stdlib_call(const char *name, JametValue **args, size_t arg_count);

#endif /* STDLIB_JAMET_H */
