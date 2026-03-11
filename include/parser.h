/*
 * jametLang - Programming Language with Bahasa Jawa Syntax
 * Parser - AST Builder Header
 */

#ifndef PARSER_H
#define PARSER_H

#include "jamet_types.h"
#include "lexer.h"

/* Expression types */
typedef enum {
    EXPR_LITERAL,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_GROUPING,
    EXPR_VARIABLE,
    EXPR_ASSIGN,
    EXPR_CALL
} ExprType;

/* Statement types */
typedef enum {
    STMT_EXPR,
    STMT_VAR_DECL,
    STMT_BLOCK,
    STMT_IF,
    STMT_WHILE,
    STMT_FOR,
    STMT_FUNC_DECL,
    STMT_RETURN,
    STMT_BREAK,
    STMT_CONTINUE
} StmtType;

/* Forward declarations */
typedef struct Expr Expr;
typedef struct Stmt Stmt;

/* Expression base */
struct Expr {
    ExprType type;
    union {
        JametValue *literal;
        struct {
            Expr *left;
            Token op;
            Expr *right;
        } binary;
        struct {
            Token op;
            Expr *right;
        } unary;
        struct {
            Expr *expression;
        } grouping;
        struct {
            Token name;
        } variable;
        struct {
            Token name;
            Expr *value;
        } assign;
        struct {
            Expr *callee;
            Token paren;
            Expr **arguments;
            size_t count;
        } call;
    } as;
};

/* Statement base */
struct Stmt {
    StmtType type;
    union {
        struct {
            Expr *expression;
        } expr;
        struct {
            Token name;
            Expr *initializer;
        } var_decl;
        struct {
            Stmt **statements;
            size_t count;
        } block;
        struct {
            Expr *condition;
            Stmt *then_branch;
            Stmt *else_branch;
        } if_stmt;
        struct {
            Expr *condition;
            Stmt *body;
        } while_stmt;
        struct {
            Stmt *initializer;
            Expr *condition;
            Expr *increment;
            Stmt *body;
        } for_stmt;
        struct {
            Token name;
            Token **params;
            size_t param_count;
            Stmt *body;
        } func_decl;
        struct {
            Token keyword;
            Expr *value;
        } return_stmt;
    } as;
};

/* Parser */
typedef struct {
    Token *tokens;
    size_t token_count;
    size_t current;
} Parser;

/* Set quiet mode (suppress => output in file execution) */
void parser_set_quiet(int quiet);

/* Create parser */
Parser *parser_new(Token *tokens, size_t count);

/* Free parser */
void parser_free(Parser *parser);

/* Parse expressions */
Expr *parser_parse_expression(Parser *parser);

/* Parse statements */
Stmt *parser_parse_statement(Parser *parser);

/* Parse program (list of statements) */
Stmt **parser_parse_program(Parser *parser, size_t *count);

/* Free expression */
void expr_free(Expr *expr);

/* Free statement */
void stmt_free(Stmt *stmt);

/* Evaluate expression */
JametValue *eval_expr(Expr *expr);

/* Execute statement */
void exec_stmt(Stmt *stmt);

#endif /* PARSER_H */
