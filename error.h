//
// Created by Paul Motte on 2019-05-06.
//

#ifndef LOX_ERROR_H
#define LOX_ERROR_H

#include "state.h"
#include "lexer.h"
#include "parser.h"

enum 				ErrorType {
	INVALID_ERROR_TYPE,

	TOKEN_ERROR, EXPR_ERROR, STMT_ERROR
};

enum 				ErrorCode {
	INVALID_ERROR_CODE,

	// Lexer errors
	UNTERMINATED_STRING,
	NUMBER_TOO_BIG,
	NUMBER_TOO_PRECISE,
	ERROR_INVALID_TOKEN,

	// Parser errors
	UNTERMINATED_GROUP,
	UNTERMINATED_STATEMENT,
	EXPECT_VARIABLE_NAME,
	INVALID_EXPRESSION,
	INVALID_ASSIGNEMENT,

	ERROR_CODE_LENGTH
};

const char			errorMessage[ERROR_CODE_LENGTH][100] = {
	"Unknown error",

	// Lexer errors
	"Unterminated string",
	"Number literal too big",
	"Number literal too precise",
	"Unexpected character",

	// Parser errors
	"Expect ')' after expression",
	"Expect ';' at the end of a statement",
	"Expect variable name",
	"Invalid expression",
	"Invalid assignment target"
};

struct				Error {
	ErrorType		type;
	ErrorCode		code;

	union {
		Token		*token;
		Expr		*expr;
		Stmt		*stmt;
	};
};

void 				pushError(State *state, ErrorCode code, Token *token);
void 				pushError(State *state, ErrorCode code, Expr *expr);
void 				pushError(State *state, ErrorCode code, Stmt *stmt);
void				reportError(char *source, Error *error);


#endif //LOX_ERROR_H
