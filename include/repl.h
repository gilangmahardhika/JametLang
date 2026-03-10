/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * REPL - Read-Eval-Print Loop Header
 */

#ifndef REPL_H
#define REPL_H

#include "jamet_types.h"
#include "lexer.h"

/* Run REPL */
void repl_run(void);

/* Fallback REPL without linenoise dependency */
void repl_run_fallback(void);

/* Print prompt */
void repl_print_prompt(void);

/* Print welcome message */
void repl_print_welcome(void);

/* Execute single line of code */
void repl_execute_line(const char *line);

/* Print token stream for debugging */
void repl_print_tokens(Token *tokens, size_t count);

#endif /* REPL_H */
