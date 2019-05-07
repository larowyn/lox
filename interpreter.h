//
// Created by Paul Motte on 2019-05-05.
//

#ifndef LOX_INTERPRETER_H
#define LOX_INTERPRETER_H

#include "ptb/ptb_array.h"
#include "ptb/ptb_substring.h"
#include "state.h"
#include "parser.h"

enum 				LoxValueType {
	INVALID_VALUE,

	LOX_NUMBER,
	LOX_STRING,
	LOX_BOOLEAN,
	LOX_NIL
};

struct				LoxValue {
	LoxValueType	type;

	union {
		r64			number;
		Substring	*string;
		bool32		boolean;
	};
};

inline void			printLoxValue(LoxValue *value) {
	switch (value->type) {
		case LOX_NUMBER:
			printf("%f", value->number);
			break;
		case LOX_STRING:
			printSubstring(value->string);
			break;
		case LOX_BOOLEAN:
			printf("%s", value->boolean ? "true" : "false");
			break;
		case LOX_NIL:
			printf("nil");
			break;
		default:
			printf("INVALID_VALUE");
	}
}

void 				eval(State *state, Array *statements);

#endif //LOX_INTERPRETER_H
