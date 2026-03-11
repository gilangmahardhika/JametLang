/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * Parser - AST Builder Implementation
 */

#include "parser.h"
#include "stdlib_jamet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

/* Control flow flags for break/continue */
static int break_flag = 0;
static int continue_flag = 0;
static JametValue *return_value = NULL;
static int quiet_mode = 0;

/* Exception handling state */
#define MAX_TRY_DEPTH 32
static jmp_buf try_stack[MAX_TRY_DEPTH];
static int try_depth = 0;
static int throw_flag = 0;
static JametValue *thrown_value = NULL;

/* Set quiet mode (suppress => output in file execution) */
void parser_set_quiet(int quiet) {
    quiet_mode = quiet;
}

/* Function representation */
typedef struct {
    char *name;
    Stmt *declaration;  /* STMT_FUNC_DECL */
} Function;

/* Simple symbol table for variables */
#define MAX_VARS 100
typedef struct {
    char *names[MAX_VARS];
    JametValue *values[MAX_VARS];
    int count;
} SymbolTable;

/* Function table */
#define MAX_FUNCS 50
typedef struct {
    Function functions[MAX_FUNCS];
    int count;
} FunctionTable;

static SymbolTable globals = {0};
static FunctionTable functions = {0};

/* Find variable index */
static int find_var(const char *name) {
    for (int i = globals.count - 1; i >= 0; i--) {
        if (strcmp(globals.names[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

/* Get variable value */
JametValue *get_var(const char *name) {
    int idx = find_var(name);
    if (idx >= 0) {
        return globals.values[idx];
    }
    return NULL;
}

/* Set variable value */
void set_var(const char *name, JametValue *value) {
    int idx = find_var(name);
    if (idx >= 0) {
        jamet_value_free(globals.values[idx]);
        globals.values[idx] = value;
    } else if (globals.count < MAX_VARS) {
        globals.names[globals.count] = strdup(name);
        globals.values[globals.count] = value;
        globals.count++;
    }
}

/* Find function by name */
static Function *find_func(const char *name) {
    for (int i = 0; i < functions.count; i++) {
        if (strcmp(functions.functions[i].name, name) == 0) {
            return &functions.functions[i];
        }
    }
    return NULL;
}

/* Register function */
static void register_func(const char *name, Stmt *declaration) {
    if (functions.count < MAX_FUNCS) {
        functions.functions[functions.count].name = strdup(name);
        functions.functions[functions.count].declaration = declaration;
        functions.count++;
    } else {
        fprintf(stderr, "Kesalahan: Function table penuh\n");
    }
}

/* Execute function call */
static JametValue *exec_function_call(Function *func, Expr **arguments, size_t arg_count) {
    Stmt *decl = func->declaration;
    size_t param_count = decl->as.func_decl.param_count;

    /* Save current variable state */
    char *saved_names[MAX_VARS];
    JametValue *saved_values[MAX_VARS];
    int saved_count = globals.count;

    for (int i = 0; i < saved_count; i++) {
        saved_names[i] = globals.names[i];
        saved_values[i] = globals.values[i];
    }

    /* Clear globals for function scope */
    globals.count = 0;

    /* Bind parameters to arguments */
    for (size_t i = 0; i < param_count && i < arg_count; i++) {
        Token *param = decl->as.func_decl.params[i];
        JametValue *arg_value = eval_expr(arguments[i]);
        set_var(param->lexeme, arg_value);
    }

    /* Execute function body */
    JametValue *func_return = NULL;
    break_flag = 0;
    return_value = NULL;

    if (decl->as.func_decl.body->type == STMT_BLOCK) {
        for (size_t i = 0; i < decl->as.func_decl.body->as.block.count; i++) {
            exec_stmt(decl->as.func_decl.body->as.block.statements[i]);
            if (break_flag) {
                /* Return statement was hit */
                break_flag = 0;
                break;
            }
        }
    } else {
        exec_stmt(decl->as.func_decl.body);
    }

    /* Capture return value */
    if (return_value) {
        func_return = return_value;
        return_value = NULL;
    } else {
        func_return = jamet_value_new(JAMET_NONE);
    }

    /* Restore variable state */
    for (int i = 0; i < globals.count; i++) {
        free(globals.names[i]);
        jamet_value_free(globals.values[i]);
    }

    globals.count = saved_count;
    for (int i = 0; i < saved_count; i++) {
        globals.names[i] = saved_names[i];
        globals.values[i] = saved_values[i];
    }

    return func_return;
}

/* Create parser */
Parser *parser_new(Token *tokens, size_t count) {
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    parser->tokens = tokens;
    parser->token_count = count;
    parser->current = 0;
    return parser;
}

/* Free parser */
void parser_free(Parser *parser) {
    free(parser);
}

/* Check if at end */
static int is_at_end(Parser *parser) {
    return parser->tokens[parser->current].type == TOKEN_EOF;
}

/* Get current token */
static Token *peek(Parser *parser) {
    return &parser->tokens[parser->current];
}

/* Get previous token */
static Token *previous(Parser *parser) {
    return &parser->tokens[parser->current - 1];
}

/* Advance and return previous token */
static Token *advance(Parser *parser) {
    if (!is_at_end(parser)) parser->current++;
    return previous(parser);
}

/* Check if current token is of type */
static int check(Parser *parser, TokenType type) {
    if (is_at_end(parser)) return 0;
    return peek(parser)->type == type;
}

/* Match and consume if types match */
static int match(Parser *parser, TokenType type) {
    if (check(parser, type)) {
        advance(parser);
        return 1;
    }
    return 0;
}

/* Consume or error */
static int consume(Parser *parser, TokenType type, const char *message) {
    if (check(parser, type)) {
        advance(parser);
        return 1;
    }
    fprintf(stderr, "Kesalahan: %s\n", message);
    return 0;
}

/* Error recovery - synchronize to next statement boundary */
static void synchronize(Parser *parser) {
    advance(parser);

    while (!is_at_end(parser)) {
        if (previous(parser)->type == TOKEN_SEMICOLON) return;

        switch (peek(parser)->type) {
            case TOKEN_VARIABEL:
            case TOKEN_NEK:
            case TOKEN_KANGGO:
            case TOKEN_SAWISE:
            case TOKEN_BALEKNO:
            case TOKEN_NYERAT:
            case TOKEN_FUNGSI:
            case TOKEN_PEGAT:
            case TOKEN_TERUSAKE:
            case TOKEN_JANCUK:
            case TOKEN_UNCAL:
                return;
            default:
                break;
        }

        advance(parser);
    }
}

/* Create literal expression */
static Expr *make_literal_expr(JametValue *value) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = EXPR_LITERAL;
    expr->as.literal = value;
    return expr;
}

/* Create binary expression */
static Expr *make_binary_expr(Expr *left, Token op, Expr *right) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = EXPR_BINARY;
    expr->as.binary.left = left;
    expr->as.binary.op = op;
    expr->as.binary.right = right;
    return expr;
}

/* Create unary expression */
static Expr *make_unary_expr(Token op, Expr *right) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = EXPR_UNARY;
    expr->as.unary.op = op;
    expr->as.unary.right = right;
    return expr;
}

/* Create grouping expression */
static Expr *make_grouping_expr(Expr *expression) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = EXPR_GROUPING;
    expr->as.grouping.expression = expression;
    return expr;
}

/* Create variable expression */
static Expr *make_variable_expr(Token name) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = EXPR_VARIABLE;
    expr->as.variable.name = name;
    return expr;
}

/* Create assign expression */
static Expr *make_assign_expr(Token name, Expr *value) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = EXPR_ASSIGN;
    expr->as.assign.name = name;
    expr->as.assign.value = value;
    return expr;
}

/* Create expression statement */
static Stmt *make_expr_stmt(Expr *expression) {
    Stmt *stmt = (Stmt *)malloc(sizeof(Stmt));
    stmt->type = STMT_EXPR;
    stmt->as.expr.expression = expression;
    return stmt;
}

/* Create var declaration statement */
static Stmt *make_var_decl_stmt(Token name, Expr *initializer) {
    Stmt *stmt = (Stmt *)malloc(sizeof(Stmt));
    stmt->type = STMT_VAR_DECL;
    stmt->as.var_decl.name = name;
    stmt->as.var_decl.initializer = initializer;
    return stmt;
}

/* Create block statement */
static Stmt *make_block_stmt(Stmt **statements, size_t count) {
    Stmt *stmt = (Stmt *)malloc(sizeof(Stmt));
    stmt->type = STMT_BLOCK;
    stmt->as.block.statements = statements;
    stmt->as.block.count = count;
    return stmt;
}

/* Create if statement */
static Stmt *make_if_stmt(Expr *condition, Stmt *then_branch, Stmt *else_branch) {
    Stmt *stmt = (Stmt *)malloc(sizeof(Stmt));
    stmt->type = STMT_IF;
    stmt->as.if_stmt.condition = condition;
    stmt->as.if_stmt.then_branch = then_branch;
    stmt->as.if_stmt.else_branch = else_branch;
    return stmt;
}

/* Create while statement */
static Stmt *make_while_stmt(Expr *condition, Stmt *body) {
    Stmt *stmt = (Stmt *)malloc(sizeof(Stmt));
    stmt->type = STMT_WHILE;
    stmt->as.while_stmt.condition = condition;
    stmt->as.while_stmt.body = body;
    return stmt;
}

/* Create for statement */
static Stmt *make_for_stmt(Stmt *initializer, Expr *condition, Expr *increment, Stmt *body) {
    Stmt *stmt = (Stmt *)malloc(sizeof(Stmt));
    stmt->type = STMT_FOR;
    stmt->as.for_stmt.initializer = initializer;
    stmt->as.for_stmt.condition = condition;
    stmt->as.for_stmt.increment = increment;
    stmt->as.for_stmt.body = body;
    return stmt;
}

/* Create function declaration statement */
static Stmt *make_func_decl_stmt(Token name, Token **params, size_t param_count, Stmt *body) {
    Stmt *stmt = (Stmt *)malloc(sizeof(Stmt));
    stmt->type = STMT_FUNC_DECL;
    stmt->as.func_decl.name = name;
    stmt->as.func_decl.params = params;
    stmt->as.func_decl.param_count = param_count;
    stmt->as.func_decl.body = body;
    return stmt;
}

/* Create return statement */
static Stmt *make_return_stmt(Token keyword, Expr *value) {
    Stmt *stmt = (Stmt *)malloc(sizeof(Stmt));
    stmt->type = STMT_RETURN;
    stmt->as.return_stmt.keyword = keyword;
    stmt->as.return_stmt.value = value;
    return stmt;
}

/* Create break statement */
static Stmt *make_break_stmt(void) {
    Stmt *stmt = (Stmt *)malloc(sizeof(Stmt));
    stmt->type = STMT_BREAK;
    return stmt;
}

/* Create continue statement */
static Stmt *make_continue_stmt(void) {
    Stmt *stmt = (Stmt *)malloc(sizeof(Stmt));
    stmt->type = STMT_CONTINUE;
    return stmt;
}

/* Create try/catch/finally statement */
static Stmt *make_try_stmt(Stmt *try_block, Token catch_param, Stmt *catch_block, Stmt *finally_block) {
    Stmt *stmt = (Stmt *)malloc(sizeof(Stmt));
    stmt->type = STMT_TRY;
    stmt->as.try_stmt.try_block = try_block;
    stmt->as.try_stmt.catch_param = catch_param;
    stmt->as.try_stmt.catch_block = catch_block;
    stmt->as.try_stmt.finally_block = finally_block;
    return stmt;
}

/* Create throw statement */
static Stmt *make_throw_stmt(Expr *value) {
    Stmt *stmt = (Stmt *)malloc(sizeof(Stmt));
    stmt->type = STMT_THROW;
    stmt->as.throw_stmt.value = value;
    return stmt;
}

/* Create call expression */
static Expr *make_call_expr(Expr *callee, Token paren, Expr **arguments, size_t count) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = EXPR_CALL;
    expr->as.call.callee = callee;
    expr->as.call.paren = paren;
    expr->as.call.arguments = arguments;
    expr->as.call.count = count;
    return expr;
}

/* Create array literal expression */
static Expr *make_array_expr(Expr **elements, size_t count) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = EXPR_ARRAY;
    expr->as.array.elements = elements;
    expr->as.array.count = count;
    return expr;
}

/* Create index access expression */
static Expr *make_index_expr(Expr *object, Expr *index) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = EXPR_INDEX;
    expr->as.index_access.object = object;
    expr->as.index_access.index = index;
    return expr;
}

/* Create index assignment expression */
static Expr *make_index_assign_expr(Expr *object, Expr *index, Expr *value) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = EXPR_INDEX_ASSIGN;
    expr->as.index_assign.object = object;
    expr->as.index_assign.index = index;
    expr->as.index_assign.value = value;
    return expr;
}

/* Forward declarations for recursive parsing */
static Stmt *parse_statement(Parser *parser);
static Stmt *parse_block(Parser *parser);

/* Parse primary */
static Expr *parse_primary(Parser *parser) {
    if (match(parser, TOKEN_INTEGER)) {
        char *endptr;
        long value = strtol(previous(parser)->lexeme, &endptr, 10);
        return make_literal_expr(jamet_integer_new(value));
    }
    if (match(parser, TOKEN_FLOAT)) {
        char *endptr;
        double value = strtod(previous(parser)->lexeme, &endptr);
        return make_literal_expr(jamet_float_new(value));
    }
    if (match(parser, TOKEN_STRING)) {
        /* Remove quotes */
        char *str = previous(parser)->lexeme;
        size_t len = strlen(str) - 2;
        char *value = (char *)malloc(len + 1);
        strncpy(value, str + 1, len);
        value[len] = '\0';
        JametValue *jv = jamet_string_new(value);
        free(value);
        return make_literal_expr(jv);
    }
    if (match(parser, TOKEN_BENER)) {
        return make_literal_expr(jamet_boolean_new(1));
    }
    if (match(parser, TOKEN_SALAH)) {
        return make_literal_expr(jamet_boolean_new(0));
    }
    if (match(parser, TOKEN_KOSONG)) {
        return make_literal_expr(jamet_value_new(JAMET_NONE));
    }
    if (match(parser, TOKEN_LPAREN)) {
        Expr *expr = parser_parse_expression(parser);
        consume(parser, TOKEN_RPAREN, "Expected ')' after expression.");
        return make_grouping_expr(expr);
    }
    /* Array literal [a, b, c] */
    if (match(parser, TOKEN_LBRACKET)) {
        Expr **elements = NULL;
        size_t elem_count = 0;
        size_t elem_capacity = 8;

        if (!check(parser, TOKEN_RBRACKET)) {
            elements = (Expr **)malloc(sizeof(Expr *) * elem_capacity);
            do {
                if (elem_count >= elem_capacity) {
                    elem_capacity *= 2;
                    elements = (Expr **)realloc(elements, sizeof(Expr *) * elem_capacity);
                }
                elements[elem_count++] = parser_parse_expression(parser);
            } while (match(parser, TOKEN_COMMA));
        }

        consume(parser, TOKEN_RBRACKET, "Expected ']' after array elements.");
        Expr *arr = make_array_expr(elements, elem_count);

        /* Check for immediate index access [1,2,3][0] */
        while (match(parser, TOKEN_LBRACKET)) {
            Expr *index = parser_parse_expression(parser);
            consume(parser, TOKEN_RBRACKET, "Expected ']' after index.");
            arr = make_index_expr(arr, index);
        }

        return arr;
    }

    if (match(parser, TOKEN_IDENTIFIER)) {
        Token name = *previous(parser);
        Expr *expr = NULL;
        
        /* Check for function call */
        if (match(parser, TOKEN_LPAREN)) {
            Token paren = *previous(parser);
            Expr **args = NULL;
            size_t arg_count = 0;
            size_t arg_capacity = 4;
            
            if (!check(parser, TOKEN_RPAREN)) {
                args = (Expr **)malloc(sizeof(Expr *) * arg_capacity);
                do {
                    if (arg_count >= arg_capacity) {
                        arg_capacity *= 2;
                        args = (Expr **)realloc(args, sizeof(Expr *) * arg_capacity);
                    }
                    args[arg_count++] = parser_parse_expression(parser);
                } while (match(parser, TOKEN_COMMA));
            }
            
            consume(parser, TOKEN_RPAREN, "Expected ')' after arguments.");
            expr = make_call_expr(make_variable_expr(name), paren, args, arg_count);
        } else {
            expr = make_variable_expr(name);
        }

        /* Check for index access: arr[i] or func()[i] */
        while (match(parser, TOKEN_LBRACKET)) {
            Expr *index = parser_parse_expression(parser);
            consume(parser, TOKEN_RBRACKET, "Expected ']' after index.");
            expr = make_index_expr(expr, index);
        }
        
        return expr;
    }

    fprintf(stderr, "Kesalahan: Token ora dikenal ing expression\n");
    return make_literal_expr(jamet_value_new(JAMET_NONE));
}

/* Parse unary */
static Expr *parse_unary(Parser *parser) {
    if (match(parser, TOKEN_ORA) || match(parser, TOKEN_MINUS) ||
        match(parser, TOKEN_PLUS)) {
        Token op = *previous(parser);
        Expr *right = parse_unary(parser);
        return make_unary_expr(op, right);
    }
    return parse_primary(parser);
}

/* Parse multiplication/division */
static Expr *parse_factor(Parser *parser) {
    Expr *expr = parse_unary(parser);

    while (match(parser, TOKEN_KALI) || match(parser, TOKEN_BAGI) ||
           match(parser, TOKEN_MODULO)) {
        Token op = *previous(parser);
        Expr *right = parse_unary(parser);
        expr = make_binary_expr(expr, op, right);
    }

    return expr;
}

/* Parse addition/subtraction */
static Expr *parse_term(Parser *parser) {
    Expr *expr = parse_factor(parser);

    while (match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS)) {
        Token op = *previous(parser);
        Expr *right = parse_factor(parser);
        expr = make_binary_expr(expr, op, right);
    }

    return expr;
}

/* Parse comparison */
static Expr *parse_comparison(Parser *parser) {
    Expr *expr = parse_term(parser);

    while (match(parser, TOKEN_LEBIH_BESIK) || match(parser, TOKEN_LEBIH_CIYUT) ||
           match(parser, TOKEN_SAMADENGAN) ||
           match(parser, TOKEN_BEDA)) {
        Token op = *previous(parser);
        Expr *right = parse_term(parser);
        expr = make_binary_expr(expr, op, right);
    }

    return expr;
}

/* Parse logical AND */
static Expr *parse_logical_and(Parser *parser) {
    Expr *expr = parse_comparison(parser);

    while (match(parser, TOKEN_LAN)) {
        Token op = *previous(parser);
        Expr *right = parse_comparison(parser);
        expr = make_binary_expr(expr, op, right);
    }

    return expr;
}

/* Parse logical OR */
static Expr *parse_logical_or(Parser *parser) {
    Expr *expr = parse_logical_and(parser);

    while (match(parser, TOKEN_UTAWA)) {
        Token op = *previous(parser);
        Expr *right = parse_logical_and(parser);
        expr = make_binary_expr(expr, op, right);
    }

    return expr;
}

/* Parse assignment */
static Expr *parse_assignment(Parser *parser) {
    Expr *expr = parse_logical_or(parser);

    if (match(parser, TOKEN_ASSIGN)) {
        (void)previous(parser); /* Consume the equals token */
        Expr *value = parse_assignment(parser);

        if (expr->type == EXPR_VARIABLE) {
            Expr *assign = make_assign_expr(expr->as.variable.name, value);
            expr_free(expr); /* Free the variable expr, we saved the name */
            return assign;
        }

        /* Array index assignment: arr[i] = val */
        if (expr->type == EXPR_INDEX) {
            Expr *obj = expr->as.index_access.object;
            Expr *idx = expr->as.index_access.index;
            Expr *assign = make_index_assign_expr(obj, idx, value);
            /* Free only the wrapper, not children (they are reused) */
            free(expr);
            return assign;
        }
    }

    return expr;
}

/* Parse expression */
Expr *parser_parse_expression(Parser *parser) {
    return parse_assignment(parser);
}

/* Parse expression statement */
static Stmt *parse_expr_statement(Parser *parser) {
    Expr *expr = parser_parse_expression(parser);
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after expression.");
    return make_expr_stmt(expr);
}

/* Parse var declaration */
static Stmt *parse_var_declaration(Parser *parser) {
    Token name = *advance(parser);

    Expr *initializer = NULL;
    if (match(parser, TOKEN_ASSIGN)) {
        initializer = parser_parse_expression(parser);
    }

    consume(parser, TOKEN_SEMICOLON, "Expected ';' after variable declaration.");
    return make_var_decl_stmt(name, initializer);
}

/* Parse block */
static Stmt *parse_block(Parser *parser) {
    size_t capacity = 8;
    Stmt **statements = (Stmt **)malloc(sizeof(Stmt *) * capacity);
    size_t count = 0;
    
    while (!check(parser, TOKEN_RBRACE) && !is_at_end(parser)) {
        if (count >= capacity) {
            capacity *= 2;
            statements = (Stmt **)realloc(statements, sizeof(Stmt *) * capacity);
        }
        statements[count++] = parse_statement(parser);
    }
    
    consume(parser, TOKEN_RBRACE, "Expected '}' after block.");
    return make_block_stmt(statements, count);
}

/* Parse if statement (nek) */
static Stmt *parse_if_statement(Parser *parser) {
    consume(parser, TOKEN_LPAREN, "Expected '(' after 'nek'.");
    Expr *condition = parser_parse_expression(parser);
    consume(parser, TOKEN_RPAREN, "Expected ')' after condition.");
    
    Stmt *then_branch = parse_statement(parser);
    Stmt *else_branch = NULL;
    
    if (match(parser, TOKEN_KAJABA)) {
        else_branch = parse_statement(parser);
    }
    
    return make_if_stmt(condition, then_branch, else_branch);
}

/* Parse print statement (nyerat) */
static Stmt *parse_print_statement(Parser *parser) {
    consume(parser, TOKEN_LPAREN, "Expected '(' after 'nyerat'.");
    Expr *value = parser_parse_expression(parser);
    consume(parser, TOKEN_RPAREN, "Expected ')' after value.");
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after nyerat statement.");

    /* Create a call expression for nyerat */
    Token name_token = {TOKEN_IDENTIFIER, "nyerat", 1, 0};
    Expr **args = (Expr **)malloc(sizeof(Expr *));
    args[0] = value;
    Expr *call = make_call_expr(make_variable_expr(name_token), name_token, args, 1);
    return make_expr_stmt(call);
}

/* Parse while statement (sawise) */
static Stmt *parse_while_statement(Parser *parser) {
    consume(parser, TOKEN_LPAREN, "Expected '(' after 'sawise'.");
    Expr *condition = parser_parse_expression(parser);
    consume(parser, TOKEN_RPAREN, "Expected ')' after condition.");
    Stmt *body = parse_statement(parser);
    return make_while_stmt(condition, body);
}

/* Parse for statement (kanggo) */
static Stmt *parse_for_statement(Parser *parser) {
    consume(parser, TOKEN_LPAREN, "Expected '(' after 'kanggo'.");

    /* Initializer */
    Stmt *initializer = NULL;
    if (match(parser, TOKEN_SEMICOLON)) {
        initializer = NULL;
    } else if (match(parser, TOKEN_VARIABEL)) {
        initializer = parse_var_declaration(parser);
    } else {
        initializer = parse_expr_statement(parser);
    }

    /* Condition */
    Expr *condition = NULL;
    if (!check(parser, TOKEN_SEMICOLON)) {
        condition = parser_parse_expression(parser);
    }
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after loop condition.");

    /* Increment */
    Expr *increment = NULL;
    if (!check(parser, TOKEN_RPAREN)) {
        increment = parser_parse_expression(parser);
    }
    consume(parser, TOKEN_RPAREN, "Expected ')' after for clauses.");

    /* Body */
    Stmt *body = parse_statement(parser);

    return make_for_stmt(initializer, condition, increment, body);
}

/* Parse function declaration (fungsi) */
static Stmt *parse_function_declaration(Parser *parser) {
    Token name = *advance(parser);

    consume(parser, TOKEN_LPAREN, "Expected '(' after function name.");

    /* Parse parameters */
    Token **params = NULL;
    size_t param_count = 0;
    size_t param_capacity = 4;

    if (!check(parser, TOKEN_RPAREN)) {
        params = (Token **)malloc(sizeof(Token *) * param_capacity);

        do {
            if (param_count >= param_capacity) {
                param_capacity *= 2;
                params = (Token **)realloc(params, sizeof(Token *) * param_capacity);
            }

            if (check(parser, TOKEN_IDENTIFIER)) {
                Token *param = (Token *)malloc(sizeof(Token));
                *param = *advance(parser);
                params[param_count++] = param;
            } else {
                fprintf(stderr, "Kesalahan: Expected parameter name.\n");
                free(params);
                return NULL;
            }
        } while (match(parser, TOKEN_COMMA));
    }

    consume(parser, TOKEN_RPAREN, "Expected ')' after parameters.");

    /* Body */
    Stmt *body = parse_statement(parser);

    return make_func_decl_stmt(name, params, param_count, body);
}

/* Parse return statement (balekno) */
static Stmt *parse_return_statement(Parser *parser) {
    Token keyword = *previous(parser);

    Expr *value = NULL;
    if (!check(parser, TOKEN_SEMICOLON)) {
        value = parser_parse_expression(parser);
    }

    consume(parser, TOKEN_SEMICOLON, "Expected ';' after return value.");
    return make_return_stmt(keyword, value);
}

/* Parse break statement (pegat) */
static Stmt *parse_break_statement(Parser *parser) {
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after 'pegat'.");
    return make_break_stmt();
}

/* Parse continue statement (terusake) */
static Stmt *parse_continue_statement(Parser *parser) {
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after 'terusake'.");
    return make_continue_stmt();
}

/* Parse try/catch/finally statement (jancuk/awas/rampungke) */
static Stmt *parse_try_statement(Parser *parser) {
    /* jancuk { ... } */
    consume(parser, TOKEN_LBRACE, "Expected '{' after 'jancuk'.");
    Stmt *try_block = parse_block(parser);

    /* awas (err) { ... } */
    consume(parser, TOKEN_AWAS, "Expected 'awas' after jancuk block.");
    consume(parser, TOKEN_LPAREN, "Expected '(' after 'awas'.");
    consume(parser, TOKEN_IDENTIFIER, "Expected variable name in 'awas'.");
    Token catch_param = *previous(parser);
    consume(parser, TOKEN_RPAREN, "Expected ')' after awas variable.");
    consume(parser, TOKEN_LBRACE, "Expected '{' after 'awas(...)'.");
    Stmt *catch_block = parse_block(parser);

    /* rampungke { ... } (optional) */
    Stmt *finally_block = NULL;
    if (match(parser, TOKEN_RAMPUNGKE)) {
        consume(parser, TOKEN_LBRACE, "Expected '{' after 'rampungke'.");
        finally_block = parse_block(parser);
    }

    return make_try_stmt(try_block, catch_param, catch_block, finally_block);
}

/* Parse throw statement (uncal) */
static Stmt *parse_throw_statement(Parser *parser) {
    Expr *value = parser_parse_expression(parser);
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after 'uncal' value.");
    return make_throw_stmt(value);
}

/* Parse statement */
static Stmt *parse_statement(Parser *parser) {
    if (match(parser, TOKEN_VARIABEL)) {
        return parse_var_declaration(parser);
    }
    if (match(parser, TOKEN_FUNGSI)) {
        return parse_function_declaration(parser);
    }
    if (match(parser, TOKEN_NEK)) {
        return parse_if_statement(parser);
    }
    if (match(parser, TOKEN_SAWISE)) {
        return parse_while_statement(parser);
    }
    if (match(parser, TOKEN_KANGGO)) {
        return parse_for_statement(parser);
    }
    if (match(parser, TOKEN_BALEKNO)) {
        return parse_return_statement(parser);
    }
    if (match(parser, TOKEN_PEGAT)) {
        return parse_break_statement(parser);
    }
    if (match(parser, TOKEN_TERUSAKE)) {
        return parse_continue_statement(parser);
    }
    if (match(parser, TOKEN_JANCUK)) {
        return parse_try_statement(parser);
    }
    if (match(parser, TOKEN_UNCAL)) {
        return parse_throw_statement(parser);
    }
    if (match(parser, TOKEN_NYERAT)) {
        return parse_print_statement(parser);
    }
    if (match(parser, TOKEN_LBRACE)) {
        return parse_block(parser);
    }
    return parse_expr_statement(parser);
}

/* Public parse statement function */
Stmt *parser_parse_statement(Parser *parser) {
    return parse_statement(parser);
}

/* Parse program */
Stmt **parser_parse_program(Parser *parser, size_t *count) {
    size_t capacity = 16;
    Stmt **statements = (Stmt **)malloc(sizeof(Stmt *) * capacity);
    *count = 0;

    while (!is_at_end(parser)) {
        if (*count >= capacity) {
            capacity *= 2;
            statements = (Stmt **)realloc(statements, sizeof(Stmt *) * capacity);
        }
        statements[(*count)++] = parser_parse_statement(parser);
    }

    return statements;
}

/* Free expression */
void expr_free(Expr *expr) {
    if (!expr) return;

    switch (expr->type) {
        case EXPR_LITERAL:
            jamet_value_free(expr->as.literal);
            break;
        case EXPR_BINARY:
            expr_free(expr->as.binary.left);
            expr_free(expr->as.binary.right);
            /* Don't free op.lexeme - owned by lexer tokens */
            break;
        case EXPR_UNARY:
            expr_free(expr->as.unary.right);
            /* Don't free op.lexeme - owned by lexer tokens */
            break;
        case EXPR_GROUPING:
            expr_free(expr->as.grouping.expression);
            break;
        case EXPR_VARIABLE:
            /* Don't free name.lexeme - owned by lexer tokens */
            break;
        case EXPR_ASSIGN:
            /* Don't free name.lexeme - owned by lexer tokens */
            expr_free(expr->as.assign.value);
            break;
        case EXPR_CALL:
            expr_free(expr->as.call.callee);
            for (size_t i = 0; i < expr->as.call.count; i++) {
                expr_free(expr->as.call.arguments[i]);
            }
            free(expr->as.call.arguments);
            break;
        case EXPR_ARRAY:
            for (size_t i = 0; i < expr->as.array.count; i++) {
                expr_free(expr->as.array.elements[i]);
            }
            free(expr->as.array.elements);
            break;
        case EXPR_INDEX:
            expr_free(expr->as.index_access.object);
            expr_free(expr->as.index_access.index);
            break;
        case EXPR_INDEX_ASSIGN:
            expr_free(expr->as.index_assign.object);
            expr_free(expr->as.index_assign.index);
            expr_free(expr->as.index_assign.value);
            break;
        default:
            break;
    }
    free(expr);
}

/* Free statement */
void stmt_free(Stmt *stmt) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_EXPR:
            expr_free(stmt->as.expr.expression);
            break;
        case STMT_VAR_DECL:
            /* Don't free name.lexeme - owned by lexer tokens */
            expr_free(stmt->as.var_decl.initializer);
            break;
        case STMT_BLOCK:
            for (size_t i = 0; i < stmt->as.block.count; i++) {
                stmt_free(stmt->as.block.statements[i]);
            }
            free(stmt->as.block.statements);
            break;
        case STMT_IF:
            expr_free(stmt->as.if_stmt.condition);
            stmt_free(stmt->as.if_stmt.then_branch);
            stmt_free(stmt->as.if_stmt.else_branch);
            break;
        case STMT_WHILE:
            expr_free(stmt->as.while_stmt.condition);
            stmt_free(stmt->as.while_stmt.body);
            break;
        case STMT_FOR:
            stmt_free(stmt->as.for_stmt.initializer);
            expr_free(stmt->as.for_stmt.condition);
            expr_free(stmt->as.for_stmt.increment);
            stmt_free(stmt->as.for_stmt.body);
            break;
        case STMT_FUNC_DECL:
            /* Don't free name.lexeme - owned by lexer tokens */
            for (size_t i = 0; i < stmt->as.func_decl.param_count; i++) {
                free(stmt->as.func_decl.params[i]); /* Free the allocated Token */
            }
            free(stmt->as.func_decl.params);
            stmt_free(stmt->as.func_decl.body);
            break;
        case STMT_RETURN:
            /* Don't free keyword.lexeme - owned by lexer tokens */
            expr_free(stmt->as.return_stmt.value);
            break;
        case STMT_BREAK:
        case STMT_CONTINUE:
            /* Nothing to free */
            break;
        case STMT_TRY:
            stmt_free(stmt->as.try_stmt.try_block);
            /* Don't free catch_param.lexeme - owned by lexer tokens */
            stmt_free(stmt->as.try_stmt.catch_block);
            stmt_free(stmt->as.try_stmt.finally_block);
            break;
        case STMT_THROW:
            expr_free(stmt->as.throw_stmt.value);
            break;
        default:
            break;
    }
    free(stmt);
}

/* Evaluate expression */
JametValue *eval_expr(Expr *expr) {
    if (!expr) return jamet_value_new(JAMET_NONE);

    switch (expr->type) {
        case EXPR_LITERAL:
            return jamet_value_copy(expr->as.literal);

        case EXPR_GROUPING:
            return eval_expr(expr->as.grouping.expression);

        case EXPR_UNARY: {
            JametValue *right = eval_expr(expr->as.unary.right);
            JametValue *result = NULL;

            if (expr->as.unary.op.type == TOKEN_MINUS) {
                if (right->type == JAMET_INTEGER) {
                    result = jamet_integer_new(-right->as.integer);
                } else if (right->type == JAMET_FLOAT) {
                    result = jamet_float_new(-right->as.float_val);
                }
            } else if (expr->as.unary.op.type == TOKEN_ORA) {
                if (right->type == JAMET_BOOLEAN) {
                    result = jamet_boolean_new(!right->as.boolean);
                }
            }

            jamet_value_free(right);
            return result ? result : jamet_value_new(JAMET_NONE);
        }

        case EXPR_VARIABLE: {
            JametValue *value = get_var(expr->as.variable.name.lexeme);
            return value ? jamet_value_copy(value) : jamet_value_new(JAMET_NONE);
        }

        case EXPR_ASSIGN: {
            JametValue *value = eval_expr(expr->as.assign.value);
            set_var(expr->as.assign.name.lexeme, jamet_value_copy(value));
            return value;
        }

        case EXPR_CALL: {
            /* Handle built-in functions */
            if (expr->as.call.callee->type == EXPR_VARIABLE) {
                const char *name = expr->as.call.callee->as.variable.name.lexeme;

                /* nyerat (print) */
                if (strcmp(name, "nyerat") == 0 && expr->as.call.count >= 1) {
                    JametValue *arg = eval_expr(expr->as.call.arguments[0]);
                    if (arg->type == JAMET_STRING) {
                        printf("%s\n", arg->as.string.value);
                    } else {
                        char *str = jamet_value_to_string(arg);
                        printf("%s\n", str);
                        free(str);
                    }
                    jamet_value_free(arg);
                    return jamet_value_new(JAMET_NONE);
                }

                /* moco (read input) - placeholder */
                if (strcmp(name, "moco") == 0) {
                    char buffer[1024];
                    if (fgets(buffer, sizeof(buffer), stdin)) {
                        size_t len = strlen(buffer);
                        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
                        return jamet_string_new(buffer);
                    }
                    return jamet_string_new("");
                }

                /* User-defined functions (checked first to allow overriding stdlib) */
                Function *func = find_func(name);
                if (func) {
                    return exec_function_call(func, expr->as.call.arguments, expr->as.call.count);
                }

                /* Standard library functions */
                {
                    size_t ac = expr->as.call.count;
                    JametValue **evaluated_args = NULL;
                    if (ac > 0) {
                        evaluated_args = (JametValue **)malloc(sizeof(JametValue *) * ac);
                        for (size_t i = 0; i < ac; i++) {
                            evaluated_args[i] = eval_expr(expr->as.call.arguments[i]);
                        }
                    }
                    JametValue *stdlib_result = stdlib_call(name, evaluated_args, ac);
                    if (stdlib_result) {
                        for (size_t i = 0; i < ac; i++) jamet_value_free(evaluated_args[i]);
                        free(evaluated_args);
                        return stdlib_result;
                    }
                    for (size_t i = 0; i < ac; i++) jamet_value_free(evaluated_args[i]);
                    free(evaluated_args);
                }
            }
            fprintf(stderr, "Kesalahan: Fungsi '%s' ora dikenal\n",
                    expr->as.call.callee->type == EXPR_VARIABLE ?
                    expr->as.call.callee->as.variable.name.lexeme : "?");
            return jamet_value_new(JAMET_NONE);
        }

        case EXPR_BINARY: {
            JametValue *left = eval_expr(expr->as.binary.left);
            JametValue *right = eval_expr(expr->as.binary.right);
            JametValue *result = jamet_value_new(JAMET_NONE);
            TokenType op = expr->as.binary.op.type;

            /* Arithmetic */
            if (left->type == JAMET_INTEGER && right->type == JAMET_INTEGER) {
                long l = left->as.integer;
                long r = right->as.integer;
                if (op == TOKEN_PLUS) result = jamet_integer_new(l + r);
                else if (op == TOKEN_MINUS) result = jamet_integer_new(l - r);
                else if (op == TOKEN_KALI) result = jamet_integer_new(l * r);
                else if (op == TOKEN_BAGI) result = jamet_integer_new(l / r);
                else if (op == TOKEN_MODULO) result = jamet_integer_new(l % r);
                else if (op == TOKEN_SAMADENGAN) result = jamet_boolean_new(l == r);
                else if (op == TOKEN_BEDA) result = jamet_boolean_new(l != r);
                else if (op == TOKEN_LEBIH_BESIK) result = jamet_boolean_new(l > r);
                else if (op == TOKEN_LEBIH_CIYUT) result = jamet_boolean_new(l < r);
                else if (op == TOKEN_LAN) result = jamet_boolean_new(l && r);
                else if (op == TOKEN_UTAWA) result = jamet_boolean_new(l || r);
            } else if ((left->type == JAMET_FLOAT || right->type == JAMET_FLOAT) &&
                       (left->type == JAMET_FLOAT || left->type == JAMET_INTEGER) &&
                       (right->type == JAMET_FLOAT || right->type == JAMET_INTEGER)) {
                double l = left->type == JAMET_FLOAT ? left->as.float_val : (double)left->as.integer;
                double r = right->type == JAMET_FLOAT ? right->as.float_val : (double)right->as.integer;
                if (op == TOKEN_PLUS) result = jamet_float_new(l + r);
                else if (op == TOKEN_MINUS) result = jamet_float_new(l - r);
                else if (op == TOKEN_KALI) result = jamet_float_new(l * r);
                else if (op == TOKEN_BAGI) result = jamet_float_new(l / r);
                else if (op == TOKEN_SAMADENGAN) result = jamet_boolean_new(l == r);
                else if (op == TOKEN_BEDA) result = jamet_boolean_new(l != r);
                else if (op == TOKEN_LEBIH_BESIK) result = jamet_boolean_new(l > r);
                else if (op == TOKEN_LEBIH_CIYUT) result = jamet_boolean_new(l < r);
                else if (op == TOKEN_LAN) result = jamet_boolean_new(l && r);
                else if (op == TOKEN_UTAWA) result = jamet_boolean_new(l || r);
            } else if (left->type == JAMET_BOOLEAN && right->type == JAMET_BOOLEAN) {
                int l = left->as.boolean;
                int r = right->as.boolean;
                if (op == TOKEN_LAN) result = jamet_boolean_new(l && r);
                else if (op == TOKEN_UTAWA) result = jamet_boolean_new(l || r);
                else if (op == TOKEN_SAMADENGAN) result = jamet_boolean_new(l == r);
                else if (op == TOKEN_BEDA) result = jamet_boolean_new(l != r);
            } else if (left->type == JAMET_STRING && right->type == JAMET_STRING) {
                if (op == TOKEN_PLUS) {
                    /* String concatenation */
                    size_t new_len = left->as.string.length + right->as.string.length;
                    char *new_str = (char *)malloc(new_len + 1);
                    strcpy(new_str, left->as.string.value);
                    strcat(new_str, right->as.string.value);
                    result = jamet_string_new(new_str);
                    free(new_str);
                } else if (op == TOKEN_SAMADENGAN) {
                    result = jamet_boolean_new(strcmp(left->as.string.value, right->as.string.value) == 0);
                } else if (op == TOKEN_BEDA) {
                    result = jamet_boolean_new(strcmp(left->as.string.value, right->as.string.value) != 0);
                }
            } else if (op == TOKEN_PLUS && (left->type == JAMET_STRING || right->type == JAMET_STRING)) {
                /* Mixed string concatenation - convert non-string to string */
                char *left_str = (left->type == JAMET_STRING) ? strdup(left->as.string.value) : jamet_value_to_string(left);
                char *right_str = (right->type == JAMET_STRING) ? strdup(right->as.string.value) : jamet_value_to_string(right);
                size_t new_len = strlen(left_str) + strlen(right_str);
                char *new_str = (char *)malloc(new_len + 1);
                strcpy(new_str, left_str);
                strcat(new_str, right_str);
                result = jamet_string_new(new_str);
                free(new_str);
                free(left_str);
                free(right_str);
            }

            jamet_value_free(left);
            jamet_value_free(right);
            return result;
        }

        case EXPR_ARRAY: {
            JametValue *arr = jamet_array_new(expr->as.array.count > 0 ? expr->as.array.count : 8);
            for (size_t i = 0; i < expr->as.array.count; i++) {
                JametValue *elem = eval_expr(expr->as.array.elements[i]);
                jamet_array_push(arr, elem);
            }
            return arr;
        }

        case EXPR_INDEX: {
            JametValue *object = eval_expr(expr->as.index_access.object);
            JametValue *idx_val = eval_expr(expr->as.index_access.index);
            JametValue *result = jamet_value_new(JAMET_NONE);

            if (object->type == JAMET_ARRAY && idx_val->type == JAMET_INTEGER) {
                long idx = idx_val->as.integer;
                if (idx < 0) idx = (long)object->as.array.count + idx;
                if (idx >= 0 && (size_t)idx < object->as.array.count) {
                    jamet_value_free(result);
                    result = jamet_value_copy(object->as.array.elements[idx]);
                }
            } else if (object->type == JAMET_STRING && idx_val->type == JAMET_INTEGER) {
                long idx = idx_val->as.integer;
                if (idx < 0) idx = (long)object->as.string.length + idx;
                if (idx >= 0 && (size_t)idx < object->as.string.length) {
                    char ch[2] = { object->as.string.value[idx], '\0' };
                    jamet_value_free(result);
                    result = jamet_string_new(ch);
                }
            }

            jamet_value_free(object);
            jamet_value_free(idx_val);
            return result;
        }

        case EXPR_INDEX_ASSIGN: {
            /* Resolve the target object (must be a variable) */
            Expr *obj_expr = expr->as.index_assign.object;
            JametValue *idx_val = eval_expr(expr->as.index_assign.index);
            JametValue *new_val = eval_expr(expr->as.index_assign.value);

            if (obj_expr->type == EXPR_VARIABLE) {
                JametValue *arr = get_var(obj_expr->as.variable.name.lexeme);
                if (arr && arr->type == JAMET_ARRAY && idx_val->type == JAMET_INTEGER) {
                    long idx = idx_val->as.integer;
                    if (idx < 0) idx = (long)arr->as.array.count + idx;
                    if (idx >= 0 && (size_t)idx < arr->as.array.count) {
                        jamet_value_free(arr->as.array.elements[idx]);
                        arr->as.array.elements[idx] = jamet_value_copy(new_val);
                    }
                }
            }

            jamet_value_free(idx_val);
            return new_val;
        }

        default:
            return jamet_value_new(JAMET_NONE);
    }
}

/* Check if condition is truthy */
static int is_truthy(JametValue *value) {
    if (!value) return 0;
    switch (value->type) {
        case JAMET_NONE: return 0;
        case JAMET_BOOLEAN: return value->as.boolean;
        case JAMET_INTEGER: return value->as.integer != 0;
        case JAMET_FLOAT: return value->as.float_val != 0.0;
        case JAMET_STRING: return value->as.string.length > 0;
        default: return 1;
    }
}

/* Execute statement */
void exec_stmt(Stmt *stmt) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_EXPR: {
            Expr *expr = stmt->as.expr.expression;
            JametValue *value = eval_expr(expr);

            /* Only print result if it's not a function call (like nyerat) */
            if (!quiet_mode && expr->type != EXPR_CALL) {
                char *str = jamet_value_to_string(value);
                printf("=> %s\n", str);
                free(str);
            }
            jamet_value_free(value);
            break;
        }
        case STMT_VAR_DECL: {
            JametValue *value = NULL;
            if (stmt->as.var_decl.initializer) {
                value = eval_expr(stmt->as.var_decl.initializer);
            } else {
                value = jamet_value_new(JAMET_NONE);
            }
            set_var(stmt->as.var_decl.name.lexeme, value);
            if (!quiet_mode) {
                char *str = jamet_value_to_string(value);
                printf("=> %s = %s\n", stmt->as.var_decl.name.lexeme, str);
                free(str);
            }
            break;
        }
        case STMT_BLOCK: {
            for (size_t i = 0; i < stmt->as.block.count; i++) {
                exec_stmt(stmt->as.block.statements[i]);
                if (break_flag || continue_flag) {
                    break;
                }
            }
            break;
        }
        case STMT_IF: {
            JametValue *cond = eval_expr(stmt->as.if_stmt.condition);
            int truthy = is_truthy(cond);
            jamet_value_free(cond);

            if (truthy) {
                exec_stmt(stmt->as.if_stmt.then_branch);
            } else if (stmt->as.if_stmt.else_branch) {
                exec_stmt(stmt->as.if_stmt.else_branch);
            }
            break;
        }
        case STMT_WHILE: {
            while (1) {
                JametValue *cond = eval_expr(stmt->as.while_stmt.condition);
                int truthy = is_truthy(cond);
                jamet_value_free(cond);

                if (!truthy) break;

                exec_stmt(stmt->as.while_stmt.body);

                if (break_flag) {
                    break_flag = 0;
                    break;
                }
                if (continue_flag) {
                    continue_flag = 0;
                    continue;
                }
            }
            break;
        }
        case STMT_FOR: {
            /* Execute initializer */
            if (stmt->as.for_stmt.initializer) {
                exec_stmt(stmt->as.for_stmt.initializer);
            }

            while (1) {
                /* Check condition */
                int should_continue = 1;
                if (stmt->as.for_stmt.condition) {
                    JametValue *cond = eval_expr(stmt->as.for_stmt.condition);
                    should_continue = is_truthy(cond);
                    jamet_value_free(cond);
                }

                if (!should_continue) break;

                /* Execute body */
                exec_stmt(stmt->as.for_stmt.body);

                if (break_flag) {
                    break_flag = 0;
                    break;
                }
                if (continue_flag) {
                    continue_flag = 0;
                }

                /* Execute increment */
                if (stmt->as.for_stmt.increment) {
                    JametValue *inc_val = eval_expr(stmt->as.for_stmt.increment);
                    jamet_value_free(inc_val);
                }
            }
            break;
        }
        case STMT_FUNC_DECL: {
            /* Register the function */
            register_func(stmt->as.func_decl.name.lexeme, stmt);
            if (!quiet_mode) {
                printf("=> fungsi %s(%zu params)\n",
                       stmt->as.func_decl.name.lexeme,
                       stmt->as.func_decl.param_count);
            }
            break;
        }
        case STMT_RETURN: {
            if (return_value) {
                jamet_value_free(return_value);
                return_value = NULL;
            }
            if (stmt->as.return_stmt.value) {
                return_value = eval_expr(stmt->as.return_stmt.value);
            }
            /* Set break flag to signal return */
            break_flag = 1;
            break;
        }
        case STMT_BREAK: {
            break_flag = 1;
            break;
        }
        case STMT_CONTINUE: {
            continue_flag = 1;
            break;
        }
        case STMT_TRY: {
            if (try_depth >= MAX_TRY_DEPTH) {
                fprintf(stderr, "Kesalahan: jancuk/awas nested kedalaman kebablasen\n");
                break;
            }

            int jmp_result = setjmp(try_stack[try_depth]);
            if (jmp_result == 0) {
                /* Normal execution of try block */
                try_depth++;
                exec_stmt(stmt->as.try_stmt.try_block);
                try_depth--;

                /* If throw happened inside a nested handler that propagated, check */
                if (throw_flag) {
                    /* Exception was thrown but not caught by inner handler, catch it here */
                    throw_flag = 0;
                    set_var(stmt->as.try_stmt.catch_param.lexeme,
                            thrown_value ? jamet_value_copy(thrown_value) : jamet_string_new("unknown error"));
                    exec_stmt(stmt->as.try_stmt.catch_block);
                    if (thrown_value) {
                        jamet_value_free(thrown_value);
                        thrown_value = NULL;
                    }
                }
            } else {
                /* longjmp landed here - exception was thrown */
                try_depth--;
                throw_flag = 0;

                /* Bind caught value to catch parameter variable */
                set_var(stmt->as.try_stmt.catch_param.lexeme,
                        thrown_value ? jamet_value_copy(thrown_value) : jamet_string_new("unknown error"));

                /* Execute catch block */
                exec_stmt(stmt->as.try_stmt.catch_block);

                /* Clean up thrown value */
                if (thrown_value) {
                    jamet_value_free(thrown_value);
                    thrown_value = NULL;
                }
            }

            /* Always execute finally block */
            if (stmt->as.try_stmt.finally_block) {
                exec_stmt(stmt->as.try_stmt.finally_block);
            }
            break;
        }
        case STMT_THROW: {
            JametValue *value = eval_expr(stmt->as.throw_stmt.value);

            /* Store the thrown value */
            if (thrown_value) {
                jamet_value_free(thrown_value);
            }
            thrown_value = value;
            throw_flag = 1;

            if (try_depth > 0) {
                /* Jump to nearest try/catch handler */
                longjmp(try_stack[try_depth - 1], 1);
            } else {
                /* Unhandled exception */
                char *str = jamet_value_to_string(value);
                fprintf(stderr, "Kesalahan ora ditangkep (Unhandled Exception): %s\n", str);
                free(str);
                throw_flag = 0;
                jamet_value_free(thrown_value);
                thrown_value = NULL;
            }
            break;
        }
        default:
            break;
    }
}
