#include <cstdio>
#include <cstdlib>

#include "ptb/ptb_types.h"
#include "ptb/ptb_io.h"
#include "ptb/ptb_array.h"
#include "ptb/ptb_map.h"
#include "error.h"
#include "state.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

// @todo @performance: Group all array initialization into one alloc
int32			run(char *source) {
	State		state;

	state.errors = NULL;

	printf("----- Source -----\n");
	printf("%s\n", source);


	printf("----- Lex -----\n");
	Array	*tokens = initArray(sizeof(Token));

	if (tokens == NULL) return 65;

	lex(&state, source, tokens);


	printf("----- Parse -----\n");
	Array	*statements = initArray(sizeof(Stmt));
	Array	*expressions = initArray(sizeof(Expr));

	if (statements == NULL || expressions == NULL) return 65;

	parse(&state, tokens, statements, expressions);

	if (state.errors) {
		Error	*errors = (Error *)getStart(state.errors);

		printf("----- Errors -----\n");
		for (uint32 i = 0; i < state.errors->length; i++) {
			reportError(source, &errors[i]);
		}

		return 65;
	}

	DEBUG_printStatements(source, statements, statements->length, 0);

	printf("----- Eval -----\n");
	state.environments = initArray(sizeof(Env));
	state.currentEnv = (Env *)getNext(state.environments);

	if (state.currentEnv == NULL) return 65;

	state.currentEnv->enclosing = NULL;
	state.currentEnv->env = initMap(sizeof(LoxValue), true, true);

	if (state.currentEnv->env == NULL) return 65;

	eval(&state, statements);

	return 0;
}

int32			main(int argc, char *argv[]) {
	int32		returnCode = 0;

	if (argc == 1) {
		// @todo: Implement REPL
	} else if (argc == 2) {
		int32	size = 0;
		char	*source = NULL;

		// DEBUG_readEntireFile(argv[1], &source); //@todo: Handle path
		size = DEBUG_readEntireFile("../sample/test.lox", &source);

		ASSERT(size > 0)

		returnCode = run(source);

		DEBUG_freeEntireFile(source);
	} else {
		printf("LoxCompiler require 1 or no arguments.");
	}

	return returnCode;
}