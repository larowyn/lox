//
// Created by Paul Motte on 2019-05-06.
//

#include "ptb/ptb_stack.h"
#include "error.h"
#include "state.h"

void 		pushError(State *state, ErrorType type, ErrorCode code) {
	Error	*error;

	if (state->errors == NULL) {
		state->errors = initStack(sizeof(Error));
	}

	error = (Error *)getNext(state->errors);

	error->type = type;
	error->code = code;
}

void		reportError(int32 line, char *message) {
	printf("\033[31m"); // red
	printf("[line: %d] Error: %s", line, message);
	printf("\033[0m\n"); // reset + \n
}

