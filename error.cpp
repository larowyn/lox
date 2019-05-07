//
// Created by Paul Motte on 2019-05-06.
//

#include "ptb/ptb_array.h"
#include "error.h"
#include "state.h"
#include "lexer.h"
#include "parser.h"

INTERNAL Error 	*_pushError(State *state, ErrorType type, ErrorCode code) {
	Error	*error;

	if (state->errors == NULL) {
		state->errors = initArray(sizeof(Error));
	}

	error = (Error *)getNext(state->errors);

	error->type = type;
	error->code = code;
}

void 			pushError(State *state, ErrorCode code, Token *token) {
	Error	*error = _pushError(state, TOKEN_ERROR, code);

	error->token = token;
}

void 			pushError(State *state, ErrorCode code, Expr *expr) {
	Error	*error = _pushError(state, EXPR_ERROR, code);

	error->expr = expr;
}

void 			pushError(State *state, ErrorCode code, Stmt *stmt) {
	Error	*error = _pushError(state, STMT_ERROR, code);

	error->stmt = stmt;
}

INTERNAL void	_printExprLine(Expr *expr) {
	switch (expr->type) {
		case BINARY:
			printf("[line: %d]: ", expr->left->value->line);
			break;
		case GROUPING:
			printf("[line: %d]: ", expr->inner->value->line);
			break;
		case UNARY:
			printf("[line: %d]: ", expr->op->line);
			break;
		case LITERAL:
			printf("[line: %d]: ", expr->value->line);
			break;
		//@todo: handle default
	}
}

void		reportError(Error *error) {
	printf("\033[31m"); // red
	switch (error->type) {
		case TOKEN_ERROR:
			printf("[line: %d] ", error->token->line);
			break;
		case EXPR_ERROR:
			_printExprLine(error->expr);
			break;
		case STMT_ERROR:
			_printExprLine(error->stmt->inner);
			break;
		//@todo: handle default
	}
	printf("Error: %s", errorMessage[error->code]);
	printf("\033[0m\n"); // reset + \n
}

