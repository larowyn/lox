//
// Created by Paul Motte on 2019-05-06.
//

#ifndef LOX_ERROR_H
#define LOX_ERROR_H

#include "state.h"

enum 			ErrorType {
	INVALID_ERROR_TYPE,

	SYNTAX_ERROR, PARSING_ERROR, RUNTIME_ERROR
};

enum 			ErrorCode {
	INVALID_ERROR_CODE,

	UNTERMINATED_STRING
};

const char		errorMessage[10][100] = {
	"INVALID ERROR CODE",

	"Unterminated string"
};

struct			Error {
	ErrorType	type;
	ErrorCode	code;
};

void 			pushError(State *state, ErrorType type, ErrorCode code);


#endif //LOX_ERROR_H
