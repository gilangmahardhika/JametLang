/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * Parser - AST Builder Implementation
 */

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simple symbol table for variables */
#define MAX_VARS 100
typedef struct {
    char *names[MAX_VARS];
    JametValue *values[MAX_VARS];
    int count;
} SymbolTable;

static SymbolTable globals = {0};

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
    if (match(parser, TOKEN_IDENTIFIER)) {
        Token name = *previous(parser);
        
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
            return make_call_expr(make_variable_expr(name), paren, args, arg_count);
        }
        
        return make_variable_expr(name);
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
        Token equals = *previous(parser);
        Expr *value = parse_assignment(parser);

        if (expr->type == EXPR_VARIABLE) {
            Expr *assign = make_assign_expr(expr->as.variable.name, value);
            expr_free(expr); /* Free the variable expr, we saved the name */
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

/* Parse statement */
static Stmt *parse_statement(Parser *parser) {
    if (match(parser, TOKEN_VARIABEL)) {
        return parse_var_declaration(parser);
    }
    if (match(parser, TOKEN_NEK)) {
        return parse_if_statement(parser);
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
                    char *str = jamet_value_to_string(arg);
                    printf("%s\n", str);
                    free(str);
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
            }
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
            } else if (left->type == JAMET_FLOAT || right->type == JAMET_FLOAT) {
                double l = left->type == JAMET_FLOAT ? left->as.float_val : left->as.integer;
                double r = right->type == JAMET_FLOAT ? right->as.float_val : right->as.integer;
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
            }

            jamet_value_free(left);
            jamet_value_free(right);
            return result;
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
            if (expr->type != EXPR_CALL) {
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
            char *str = jamet_value_to_string(value);
            printf("=> %s = %s\n", stmt->as.var_decl.name.lexeme, str);
            free(str);
            break;
        }
        case STMT_BLOCK: {
            for (size_t i = 0; i < stmt->as.block.count; i++) {
                exec_stmt(stmt->as.block.statements[i]);
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
        default:
            break;
    }
}
