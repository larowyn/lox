//
// Created by Paul Motte on 2019-05-04.
//

#include "ptb/ptb_stack.h"
#include "parser.h"

GLOBAL Token	*tokens;
GLOBAL int32	current = 0;

INTERNAL inline bool matchType(TokenType type) {
	return tokens[current].type == type;
}

void			reportError2(int32 line, char *message) {
	printf("\033[31m"); // red
	printf("[line: %d] Error: %s", line, message);
	printf("\033[0m\n"); // reset + \n
}

Expr			*expression(Stack *expressions);

Expr			*primary(Stack *expressions) {
	Expr		*expr = (Expr *)getNext(expressions);

	if (matchType(LEFT_PAREN)) {
		expr->type = GROUPING;

		current++;
		expr->inner = expression(expressions);

		if (!matchType(RIGHT_PAREN)) {
			reportError2(tokens[current].line, "Expect ')' After Expression"); // @todo: error handling
		} else {
			current++;
		}
	} else {
		expr->type = LITERAL;
		expr->value = &tokens[current];
		current++;
	}

	return expr;
}

Expr			*unary(Stack *expressions) {
	if (matchType(BANG) || matchType(MINUS)) {
		Expr	*unaryExpr = (Expr *)getNext(expressions);

		unaryExpr->type = UNARY;
		unaryExpr->op = &tokens[current];

		current++;
		unaryExpr->right = unary(expressions);

		return unaryExpr;
	}

	return primary(expressions);
}

// @todo: refacto all binaries expression handler into one that take the array of token to put in the while
Expr			*multiplication(Stack *expressions) {
	Expr		*expr = unary(expressions);

	while (matchType(STAR) || matchType(SLASH)) {
		Expr	*binaryExpr = (Expr *)getNext(expressions);

		binaryExpr->type = BINARY;
		binaryExpr->left = expr;

		binaryExpr->op = &tokens[current];
		current++;

		binaryExpr->right = unary(expressions);
		expr = binaryExpr;
	}

	return expr;
}

Expr			*addition(Stack *expressions) {
	Expr		*expr = multiplication(expressions);

	while (matchType(PLUS) || matchType(MINUS)) {
		Expr	*binaryExpr = (Expr *)getNext(expressions);

		binaryExpr->type = BINARY;
		binaryExpr->left = expr;

		binaryExpr->op = &tokens[current];
		current++;

		binaryExpr->right = multiplication(expressions);
		expr = binaryExpr;
	}

	return expr;
}

Expr			*comparison(Stack *expressions) {
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

		binaryExpr->op = &tokens[current];
		current++;

		binaryExpr->right = addition(expressions);
		expr = binaryExpr;
	}

	return expr;
}

Expr			*equality(Stack *expressions) {
	Expr		*expr = comparison(expressions);

	while (matchType(EQUAL_EQUAL) || matchType(BANG_EQUAL)) {
		Expr	*binaryExpr = (Expr *)getNext(expressions);

		binaryExpr->type = BINARY;
		binaryExpr->left = expr;

		binaryExpr->op = &tokens[current];
		current++;

		binaryExpr->right = comparison(expressions);
		expr = binaryExpr;
	}

	return expr;
}

Expr			*expression(Stack *expressions) {
	return equality(expressions);
}

Stmt			*expressionStatement(Stack *statements, Stack *expressions) {
	Stmt		*statement = (Stmt *)getNext(statements);

	statement->type = STMT;
	statement->inner = expression(expressions);

	return statement;
}

Stmt			*printStatement(Stack *statements, Stack *expressions) {
	Stmt		*statement = (Stmt *)getNext(statements);

	statement->type = PRINT_STMT;

	current++;
	statement->inner = expression(expressions);

	return statement;
}

Stmt			*statement(Stack *statements, Stack *expressions) {
	Stmt		*statement;

	if (matchType(PRINT)) {
		statement = printStatement(statements, expressions);
	} else {
		statement = expressionStatement(statements, expressions);
	}

	if (!matchType(SEMICOLON)) {
		// @todo: error handling
		reportError2(tokens[current].line, "Expect ';' At The End Of A Statement");
		statement->type = INVALID_STMT;
	} else {
		current++;
	}

	return statement;
}

void 			parse(Stack *tokensStack, Stack *statements, Stack *expressions) {
	tokens = (Token *)getStart(tokensStack);

	while (!matchType(LOX_EOF)) {
		statement(statements, expressions);
	}
}
