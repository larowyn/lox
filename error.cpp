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

	return error;
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

INTERNAL Token	*getTokenInExpr(Expr *expr) {
	switch (expr->type) {
		case BINARY:
			return expr->left->value;
		case GROUPING:
			return expr->inner->value;
		case UNARY:
			return expr->op;
		case LITERAL:
			return expr->value;
		case INVALID_EXPR:
			return expr->value;
		default:
			//@todo: handle default
			ASSERT(false)
	}
}

INTERNAL Token	*getErroneousToken(Error *error) {
	switch (error->type) {
		case TOKEN_ERROR:
			return error->token;
		case EXPR_ERROR:
			return getTokenInExpr(error->expr);
		case STMT_ERROR:
			return error->stmt->inner ? getTokenInExpr(error->stmt->inner) : error->stmt->blockStart;
		default:
			//@todo: handle default
			ASSERT(false)
	}
}

void			reportError(char *source, Error *error) {
	Token		*erroneousToken = getErroneousToken(error);

	printf("\033[31m"); // red
	printf("Error: %s\n", errorMessage[error->code]);
	printf("\033[0m"); // reset
	printf("\t");
	printf("%d", erroneousToken->line);
	printf(" | ");

	uint32		startOfLine = erroneousToken->offset;

	while (startOfLine && source[startOfLine] != '\n') {
		startOfLine--;
	}

	if (source[startOfLine] == '\n') startOfLine++;

	uint32		length = erroneousToken->offset - startOfLine;

	while (
		source[startOfLine + length] != '\0'
		&& source[startOfLine + length] != '\n'
	) {
		length++;
	}

	printf("%.*s\n", length, &source[startOfLine]);

	printf("\t%*s   ", snprintf(NULL, 0, "%d", erroneousToken->line), "");
	printf("%*s^--- Here\n\n", erroneousToken->offset - startOfLine, "");
}
