//
// Created by Paul Motte on 2019-05-04.
//

#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include "state.h"
#include "lexer.h"

enum 		ExprType {
	INVALID_EXPR,

	BINARY, GROUPING, LITERAL, UNARY
};

struct			Expr {
	ExprType	type;

	union {
		Expr		*left;
		Expr		*inner;
	};
	Expr		*right;
	Token		*op;
	Token		*value;
};

enum 		StmtType {
	INVALID_STMT,

	STMT, PRINT_STMT
};

struct			Stmt {
	StmtType	type;

	Expr		*inner;
};

inline char		*exprTypeToString(int32 type) {
	switch (type) {
		case INVALID_EXPR:
			return "INVALID_EXPR";
		case BINARY:
			return "BINARY";
		case GROUPING:
			return "GROUPING";
		case LITERAL:
			return "LITERAL";
		case UNARY:
			return "UNARY";
		default:
			return "UNKNOWN";
	}
}

inline char		*stmtTypeToString(int32 type) {
	switch (type) {
		case INVALID_STMT:
			return "INVALID_STMT";
		case STMT:
			return "STMT";
		case PRINT_STMT:
			return "PRINT_STMT";
		default:
			return "UNKNOWN";
	}
}

inline char		*exprToString(Expr *expr, char buffers[1000][1000], int32 index) {
	char *buffer = buffers[index];

	ASSERT(index < 1000)

	switch (expr->type) {
		case INVALID_EXPR:
			return "INVALID_EXPR";
		case BINARY:
			sprintf(
				buffer,
				"(%s %s %s)",
				exprToString(expr->left, buffers, index + 1),
				tokenTypeToString(expr->op->type),
				exprToString(expr->right, buffers, index + 2)
			);
			return buffer;
		case GROUPING:
			sprintf(buffer, "(group %s)", exprToString(expr->left, buffers, index + 1));
			return buffer;
		case LITERAL:
			if (expr->value->type == NUMBER) {
				sprintf(buffer, "%f", expr->value->literal.number);
			} else if (expr->value->type == STRING) {
				sprintf(buffer, "%.*s", expr->value->literal.string.length, expr->value->literal.string.start);
			} else if (expr->value->type == IDENTIFIER) {
				sprintf(buffer, "%.*s", expr->value->lexeme.length, expr->value->lexeme.start);
			} else if (expr->value->type == TRUE) {
				sprintf(buffer, "true");
			} else if (expr->value->type == FALSE) {
				sprintf(buffer, "false");
			} else if (expr->value->type == NIL) {
				sprintf(buffer, "nil");
			}

			return buffer;
		case UNARY:
			sprintf(buffer, "(%s %s)", tokenTypeToString(expr->op->type), exprToString(expr->right, buffers, index + 1));
			return buffer;
		default:
			return "UNKNOWN";
	}
}

inline void		printExpr(Expr *expr) {
	char buffers [1000][1000]; // @todo

	printf("%s\n", exprToString(expr, buffers, 0));
}

void			parse(State *state, Array *tokensStack, Array *statements, Array *expressions);

#endif //LOX_PARSER_H
