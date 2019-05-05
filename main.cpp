#include <cstdio>
#include <cstdlib>

#include "ptb/ptb_types.h"
#include "ptb/ptb_io.h"
#include "ptb/ptb_stack.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

void	run(char *source) {
	printf("----- Source -----\n");
	printf("%s\n", source);


	printf("----- Lex -----\n");
	Stack	*tokens = initStack(sizeof(Token));

	lex(source, tokens);


	printf("----- Parse -----\n");
	Stack	*statements = initStack(sizeof(Stmt));
	Stack	*expressions = initStack(sizeof(Expr));

	parse(tokens, statements, expressions);


	printf("----- Eval -----\n");
	eval(statements);
}

int32	main(int argc, char *argv[]) {
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