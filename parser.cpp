//
// Created by Paul Motte on 2019-05-04.
//

#include "ptb/ptb_array.h"
#include "state.h"
#include "error.h"
#include "parser.h"

GLOBAL State		*state;
GLOBAL Token		*tokens;
GLOBAL int32		current = 0;
GLOBAL ErrorCode	errorCode;

INTERNAL bool		matchType(TokenType type) {
	return tokens[current].type == type;
}

INTERNAL Token		*currentToken() {
	return &tokens[current];
}

INTERNAL void		consume() {
	current++;

	while (matchType(INVALID_TOKEN)) {
		current++;
	}
}

void				synchronize() {
	while (!matchType(LOX_EOF)) {
		switch (currentToken()->type) {
			case CLASS:
			case FUN:
			case VAR:
			case FOR:
			case IF:
			case WHILE:
			case PRINT:
			case RETURN:
				if (tokens->length > 1 && tokens[current - 1].type == SEMICOLON) {
					return;
				}
				break;
			default:
				break;
		}

		consume();
	}

	errorCode = UNTERMINATED_STATEMENT;
}

Expr				*expression(Array *expressions);

Expr				*primary(Array *expressions) {
	Expr			*expr = (Expr *)getNext(expressions);

	switch (currentToken()->type) {
		case LEFT_PAREN:
			expr->type = GROUPING;

			consume(); // (

			expr->inner = expression(expressions);
			consume();

			if (!matchType(RIGHT_PAREN)) {
				expr->type = INVALID_EXPR;
				expr->value = currentToken();
				errorCode = UNTERMINATED_GROUP;
			} else {
				consume(); // )
			}
			break;
		case IDENTIFIER:
			expr->type = VARIABLE;
			expr->identifier = currentToken();
			consume();
			break;
		case TRUE:
		case FALSE:
		case NIL:
		case NUMBER:
		case STRING:
			expr->type = LITERAL;
			expr->value = currentToken();
			consume();
			break;
		default:
			expr->type = INVALID_EXPR;
			expr->value = currentToken();
			errorCode = INVALID_EXPRESSION;
			consume();
			break;
	}

	return expr;
}

Expr				*unary(Array *expressions) {
	if (matchType(BANG) || matchType(MINUS)) {
		Expr		*unaryExpr = (Expr *)getNext(expressions);

		unaryExpr->type = UNARY;
		unaryExpr->op = currentToken();

		consume();
		unaryExpr->right = unary(expressions);

		return unaryExpr;
	}

	return primary(expressions);
}

// @todo: refacto all binaries expression handler into one that take the array of token to put in the while
Expr				*multiplication(Array *expressions) {
	Expr			*expr = unary(expressions);

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

Expr				*addition(Array *expressions) {
	Expr			*expr = multiplication(expressions);

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

Expr				*comparison(Array *expressions) {
	Expr			*expr = addition(expressions);

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

Expr				*equality(Array *expressions) {
	Expr			*expr = comparison(expressions);

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

Expr				*assignment(Array *expressions) {
	Expr			*expr = equality(expressions);

	if (matchType(EQUAL)) {
		if (expr->type != VARIABLE) {
			expr->type = INVALID_EXPR;
			expr->value = currentToken();
			errorCode = INVALID_ASSIGNEMENT;
			return expr;
		}

		expr->type = ASSIGNMENT;

		consume(); // =
		expr->right = assignment(expressions);

		return expr;
	}

	return expr;
}

Expr				*expression(Array *expressions) {
	return assignment(expressions);
}

Stmt				*expressionStatement(Array *statements, Array *expressions) {
	Stmt			*statement = (Stmt *)getNext(statements);

	statement->type = STMT;
	statement->inner = expression(expressions);

	return statement;
}

Stmt				*printStatement(Array *statements, Array *expressions) {
	Stmt			*statement = (Stmt *)getNext(statements);

	statement->type = PRINT_STMT;

	consume();
	statement->inner = expression(expressions);

	return statement;
}

Stmt				*statement(Array *statements, Array *expressions) {
	Stmt			*stmt;

	if (matchType(PRINT)) {
		stmt = printStatement(statements, expressions);
	} else {
		stmt = expressionStatement(statements, expressions);
	}

	return stmt;
}

Stmt				*declarationStatement(Array *statements, Array *expressions) {
	Stmt			*stmt = (Stmt *)getNext(statements);

	stmt->type = DECL_STMT;

	consume(); // VAR

	if (!matchType(IDENTIFIER)) {
		stmt->type = INVALID_STMT;
		stmt->inner->type = INVALID_EXPR;
		stmt->inner->value = currentToken();
		errorCode = UNTERMINATED_GROUP;
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

Stmt				*declaration(Array *statements, Array *expressions) {
	Stmt			*stmt;

	if (matchType(VAR)) {
		stmt = declarationStatement(statements, expressions);
	} else {
		stmt = statement(statements, expressions);
	}

	if (stmt->type == INVALID_STMT || stmt->inner->type == INVALID_EXPR) {
		if (stmt->type == INVALID_STMT) {
			pushError(state, errorCode, stmt);
		} else {
			pushError(state, errorCode, stmt->inner);
		}

		errorCode = INVALID_ERROR_CODE;

		stmt->type = INVALID_STMT;
		stmt->inner->type = INVALID_EXPR;

		// Synchronize
		synchronize();
	} else {
		if (!matchType(SEMICOLON)) {
			stmt->type = INVALID_STMT;
			pushError(state, UNTERMINATED_STATEMENT, stmt);
		} else {
			consume();
		}
	}

	return stmt;
}

void 				parse(State *interpreterState, Array *tokensArray, Array *statements, Array *expressions) {
	state = interpreterState;
	tokens = (Token *)getStart(tokensArray);

	// Skip potential starting invalid tokens
	while (matchType(INVALID_TOKEN)) {
		current++;
	}

	while (!matchType(LOX_EOF)) {
		declaration(statements, expressions);
		errorCode = INVALID_ERROR_CODE;
	}
}
