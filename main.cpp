#include <cstdio>
#include <cstdlib>

#include "ptb/ptb_types.h"
#include "ptb/ptb_io.h"
#include "ptb/ptb_array.h"
#include "error.h"
#include "state.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

// @todo @performance: Group all stack initialization into one alloc
void		run(char *source) {
	State	state;

	state.errors = NULL;

	printf("----- Source -----\n");
	printf("%s\n", source);


	printf("----- Lex -----\n");
	Array	*tokens = initArray(sizeof(Token));

	lex(&state, source, tokens);


	printf("----- Parse -----\n");
	Array	*statements = initArray(sizeof(Stmt));
	Array	*expressions = initArray(sizeof(Expr));

	parse(&state, tokens, statements, expressions);


	printf("----- Eval -----\n");
	eval(&state, statements);
}

int32		main(int argc, char *argv[]) {
	if (argc == 1) {
		// @todo: Implement REPL
	} else if (argc == 2) {
		int32	size = 0;
		char	*source = NULL;

		// DEBUG_readEntireFile(argv[1], &source); //@todo: Handle path
		size = DEBUG_readEntireFile("../sample/test.lox", &source);

		ASSERT(size > 0)

		run(source);

		DEBUG_freeEntireFile(source);
	} else {
		printf("LoxCompiler require 1 or no arguments.");
	}

	return 0;
}