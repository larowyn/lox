//
// Created by Paul Motte on 2019-05-04.
//

#include "ptb/ptb_array.h"
#include "state.h"
#include "error.h"
#include "parser.h"

GLOBAL State	*state;
GLOBAL Token	*tokens;
GLOBAL int32	current = 0;

INTERNAL bool	matchType(TokenType type) {
	return tokens[current].type == type;
}

INTERNAL Token	*currentToken() {
	return &tokens[current];
}

INTERNAL void	consume() {
	current++;

	while (matchType(INVALID_TOKEN)) {
		current++;
	}
}

void			reportError2(int32 line, char *message) {
	printf("\033[31m"); // red
	printf("[line: %d] Error: %s", line, message);
	printf("\033[0m\n"); // reset + \n
}

Expr			*expression(Array *expressions);

Expr			*primary(Array *expressions) {
	Expr		*expr = (Expr *)getNext(expressions);

	if (matchType(LEFT_PAREN)) {
		expr->type = GROUPING;

		consume();
		expr->inner = expression(expressions);

		if (!matchType(RIGHT_PAREN)) {
			pushError(state, UNTERMINATED_GROUP, expr);
		} else {
			consume();
		}
	} else if (matchType(IDENTIFIER)) {
		expr->type = VARIABLE;
		expr->value = currentToken();
		consume();
	} else {
		expr->type = LITERAL;
		expr->value = currentToken();
		consume();
	}

	return expr;
}

Expr			*unary(Array *expressions) {
	if (matchType(BANG) || matchType(MINUS)) {
		Expr	*unaryExpr = (Expr *)getNext(expressions);

		unaryExpr->type = UNARY;
		unaryExpr->op = currentToken();

		consume();
		unaryExpr->right = unary(expressions);

		return unaryExpr;
	}

	return primary(expressions);
}

// @todo: refacto all binaries expression handler into one that take the array of token to put in the while
Expr			*multiplication(Array *expressions) {
	Expr		*expr = unary(expressions);

	while (matchType(STAR) || matchType(SLASH)) {
		Expr	*binaryExpr = (Expr *)getNext(expressions);

		binaryExpr->type = BINARY;
		binaryExpr->left = expr;

		binaryExpr->op = currentToken();
		consume();

		binaryExpr->right = unary(expressions);
		expr = binaryExpr;
	}

	return expr;
}

Expr			*addition(Array *expressions) {
	Expr		*expr = multiplication(expressions);

	while (matchType(PLUS) || matchType(MINUS)) {
		Expr	*binaryExpr = (Expr *)getNext(expressions);

		binaryExpr->type = BINARY;
		binaryExpr->left = expr;

		binaryExpr->op = currentToken();
		consume();

		binaryExpr->right = multiplication(expressions);
		expr = binaryExpr;
	}

	return expr;
}

Expr			*comparison(Array *expressions) {
	Expr		*expr = addition(expressions);

	while (
		matchType(GREATER)
		|| matchType(GREATER_EQUAL)
		|| matchType(LESS)
		|| matchType(LESS_EQUAL)
	) {
		Expr	*binaryExpr = (Expr *)getNext(expressions);

		binaryExpr->type = BINARY;
		binaryExpr->left = expr;

		binaryExpr->op = currentToken();
		consume();

		binaryExpr->right = addition(expressions);
		expr = binaryExpr;
	}

	return expr;
}

Expr			*equality(Array *expressions) {
	Expr		*expr = comparison(expressions);

	while (matchType(EQUAL_EQUAL) || matchType(BANG_EQUAL)) {
		Expr	*binaryExpr = (Expr *)getNext(expressions);

		binaryExpr->type = BINARY;
		binaryExpr->left = expr;

		binaryExpr->op = currentToken();
		consume();

		binaryExpr->right = comparison(expressions);
		expr = binaryExpr;
	}

	return expr;
}

Expr			*expression(Array *expressions) {
	return equality(expressions);
}

Stmt			*expressionStatement(Array *statements, Array *expressions) {
	Stmt		*statement = (Stmt *)getNext(statements);

	statement->type = STMT;
	statement->inner = expression(expressions);

	return statement;
}

Stmt			*printStatement(Array *statements, Array *expressions) {
	Stmt		*statement = (Stmt *)getNext(statements);

	statement->type = PRINT_STMT;

	consume();
	statement->inner = expression(expressions);

	return statement;
}

Stmt			*statement(Array *statements, Array *expressions) {
	Stmt		*stmt;

	if (matchType(PRINT)) {
		stmt = printStatement(statements, expressions);
	} else {
		stmt = expressionStatement(statements, expressions);
	}

	return stmt;
}

Stmt			*declarationStatement(Array *statements, Array *expressions) {
	Stmt		*stmt = (Stmt *)getNext(statements);

	stmt->type = DECL_STMT;

	consume(); // VAR

	if (!matchType(IDENTIFIER)) {
		stmt->type = INVALID_STMT;
		pushError(state, EXPECT_VARIABLE_NAME, stmt);
	} else {
		stmt->initializer = NULL;
		stmt->identifier = currentToken();
		consume(); // IDENTIFIER

		if (matchType(EQUAL)) {
			consume(); // EQUAL
			stmt->initializer = expression(expressions);
		}
	}

	return stmt;
}

Stmt			*declaration(Array *statements, Array *expressions) {
	Stmt		*stmt;

	if (matchType(VAR)) {
		stmt = declarationStatement(statements, expressions);
	} else {
		stmt = statement(statements, expressions);
	}

	// @todo: error synchronization should happen here if an error happen

	if (!matchType(SEMICOLON)) {
		stmt->type = INVALID_STMT;
		pushError(state, UNTERMINATED_STATEMENT, stmt);
	} else {
		consume();
	}

	return stmt;
}

void 			parse(State *interpreterState, Array *tokensArray, Array *statements, Array *expressions) {
	state = interpreterState;
	tokens = (Token *)getStart(tokensArray);

	// Skip potential starting invalid tokens
	while (matchType(INVALID_TOKEN)) {
		current++;
	}

	while (!matchType(LOX_EOF)) {
		declaration(statements, expressions);
	}
}
