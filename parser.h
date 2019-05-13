//
// Created by Paul Motte on 2019-05-04.
//

#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include "state.h"
#include "lexer.h"

enum 				ExprType {
	INVALID_EXPR,

	BINARY,
	GROUPING,
	LITERAL,
	UNARY,
	VARIABLE,
	ASSIGNMENT,
	LOGIC_OR,
	LOGIC_AND
};

// @todo: figure out which property is used by what (like Stmt)
struct				Expr {
	ExprType		type;

	union {
		Expr		*left; // LOGIC_OR, LOGIC_AND
		Expr		*inner; // GROUPING
	};

	Expr			*right; // LOGIC_OR, LOGIC_AND

	union {
		Token		*op; // LOGIC_OR, LOGIC_AND
		Token		*value;
		Token		*identifier;
	};
};

enum 				StmtType {
	INVALID_STMT,

	STMT,
	PRINT_STMT,
	DECL_STMT,
	BLOCK_STMT,
	IF_STMT,
	WHILE_STMT
};

// @todo: figure out which property is used by what
struct				Stmt {
	StmtType		type;

	union {
		Expr		*inner; // STMT, GROUP_STMT
		Expr		*initializer; // DECL_STMT
		Expr		*condition; // IF_STMT, WHILE_STMT
	};

	SubArray		statements; // BLOCK_STMT

	union {
		Stmt		*thenBranch; // IF_STMT
		Stmt		*body; // WHILE_STMT
	};
	Stmt			*elseBranch; // IF_STMT

	union {
		Token		*identifier;
		Token		*stmtStart; // BLOCK_STMT, IF_STMT, WHILE_STMT
	};
};

inline char			*exprTypeToString(int32 type) {
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
		case LOGIC_OR:
			return "LOGIC_OR";
		case LOGIC_AND:
			return "LOGIC_AND";
		default:
			return "UNKNOWN";
	}
}

inline char			*stmtTypeToString(int32 type) {
	switch (type) {
		case INVALID_STMT:
			return "INVALID_STMT";
		case STMT:
			return "STMT";
		case PRINT_STMT:
			return "PRINT_STMT";
		case DECL_STMT:
			return "DECL_STMT";
		case BLOCK_STMT:
			return "BLOCK_STMT";
		case IF_STMT:
			return "IF_STMT";
		case WHILE_STMT:
			return "WHILE_STMT";
		default:
			return "UNKNOWN";
	}
}

inline char			*exprToString(Expr *expr, char buffers[1000][1000], int32 index) {
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

inline void			printExpr(Expr *expr) {
	char buffers [1000][1000]; // @todo

	printf("%s\n", exprToString(expr, buffers, 0));
}

void				parse(State *state, Array *tokensArray, Array *statements, Array *expressions);
uint32				DEBUG_printStatements(char *source, Array *statements, uint32 length, int32 nesting);

#endif //LOX_PARSER_H
