//
// Created by Paul Motte on 2019-05-05.
//

#include "ptb/ptb_array.h"
#include "state.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

GLOBAL State	*state;

INTERNAL Map	*currentEnvMap(State *state) {
	return state->currentEnv->env;
}

INTERNAL void	reportError3(int32 line, char *message) {
	printf("\033[31m"); // red
	printf("[line: %d]: Error: %s", line, message);
	printf("\033[0m\n"); // reset + \n
}

INTERNAL bool32	isTruthy(LoxValue value) {
	switch (value.type) {
		case LOX_NUMBER:
			return 1;
		case LOX_STRING:
			return 1;
		case LOX_BOOLEAN:
			return value.boolean;
		case LOX_NIL:
			return 0;
		default:
			// @todo: Error handling
			return 0;
	}
}

uint32			execStatement(Stmt *statement);
LoxValue		evalExpr(Expr *expr);

LoxValue		evalLiteral(Expr *expr) {
	LoxValue	result;

	switch (expr->value->type) {
		case NUMBER:
			result.type = LOX_NUMBER;
			result.number = expr->value->literal.number;
			break;
		case STRING:
			result.type = LOX_STRING;
			result.string = &expr->value->literal.string;
			break;
		case TRUE:
			result.type = LOX_BOOLEAN;
			result.boolean = 1;
			break;
		case FALSE:
			result.type = LOX_BOOLEAN;
			result.boolean = 0;
			break;
		case NIL:
			result.type = LOX_NIL;
			break;
		default:
			// @todo: Error handling
			result.type = INVALID_VALUE;
			reportError3(expr->value->line, "Invalid Value");
			break;
	}

	return  result;
}

LoxValue		evalUnary(Expr *expr) {
	LoxValue	result;
	LoxValue	right = evalExpr(expr->right);

	switch (expr->op->type) {
		case MINUS:
			if (right.type != LOX_NUMBER) {
				// @todo: Error handling
				result.type = INVALID_VALUE;
				reportError3(expr->op->line, "Not a number");
				break;
			}

			result.type = LOX_NUMBER;
			result.number = -right.number;
			break;
		case BANG:
			result.type = LOX_BOOLEAN;
			result.boolean = isTruthy(right);
			break;
		default:
			// @todo: Error handling
			result.type = INVALID_VALUE;
			reportError3(expr->value->line, "Invalid Value");
			break;
	}

	return result;
}

LoxValue		evalBinary(Expr *expr) {
	LoxValue	result;
	LoxValue	left = evalExpr(expr->left);
	LoxValue	right = evalExpr(expr->right);

	switch (expr->op->type) {
		// Numeric operations
		// @todo: refacto numeric operations
		case MINUS:
			if (left.type != LOX_NUMBER || right.type != LOX_NUMBER) {
				// @todo: Error handling
				result.type = INVALID_VALUE;
				reportError3(expr->op->line, "Invalid Expression");
				break;
			}

			result.type = LOX_NUMBER;
			result.number = left.number - right.number;
			break;
		case STAR:
			if (left.type != LOX_NUMBER || right.type != LOX_NUMBER) {
				// @todo: Error handling
				result.type = INVALID_VALUE;
				reportError3(expr->op->line, "Invalid Expression");
				break;
			}

			result.type = LOX_NUMBER;
			result.number = left.number * right.number;
			break;
		case SLASH:
			if (left.type != LOX_NUMBER || right.type != LOX_NUMBER) {
				// @todo: Error handling
				result.type = INVALID_VALUE;
				reportError3(expr->op->line, "Invalid Expression");
				break;
			}

			ASSERT(right.number != 0) // @todo: Error Handling

			result.type = LOX_NUMBER;
			result.number = left.number / right.number;
			break;

		// Comparison operations
		case GREATER:
			if (left.type != LOX_NUMBER || right.type != LOX_NUMBER) {
				// @todo: Error handling
				result.type = INVALID_VALUE;
				reportError3(expr->op->line, "Invalid Expression");
				break;
			}

			result.type = LOX_NUMBER;
			result.number = left.number > right.number;
			break;
		case GREATER_EQUAL:
			if (left.type != LOX_NUMBER || right.type != LOX_NUMBER) {
				// @todo: Error handling
				result.type = INVALID_VALUE;
				reportError3(expr->op->line, "Invalid Expression");
				break;
			}

			result.type = LOX_NUMBER;
			result.number = left.number >= right.number;
			break;
		case LESS:
			if (left.type != LOX_NUMBER || right.type != LOX_NUMBER) {
				// @todo: Error handling
				result.type = INVALID_VALUE;
				reportError3(expr->op->line, "Invalid Expression");
				break;
			}

			result.type = LOX_NUMBER;
			result.number = left.number < right.number;
			break;
		case LESS_EQUAL:
			if (left.type != LOX_NUMBER || right.type != LOX_NUMBER) {
				// @todo: Error handling
				result.type = INVALID_VALUE;
				reportError3(expr->op->line, "Invalid Expression");
				break;
			}

			result.type = LOX_NUMBER;
			result.number = left.number <= right.number;
			break;


		// Hybrid operations
		case PLUS:
			if (left.type == LOX_NUMBER || right.type == LOX_NUMBER) {
				result.type = LOX_NUMBER;
				result.number = left.number + right.number;
				break;
			} else if (left.type == LOX_STRING || right.type == LOX_STRING) {
				// @todo: String concatenation
				result.type = INVALID_VALUE;
				reportError3(expr->op->line, "String Concatenation Not Yet Implemented");
				break;
			}

			// @todo: Error handling
			result.type = INVALID_VALUE;
			reportError3(expr->op->line, "Invalid Expression");
			break;
		case EQUAL_EQUAL:
			if (left.type != right.type) {
				result.type = LOX_BOOLEAN;
				result.boolean = 0;
			} else {
				switch (left.type) {
					case LOX_NUMBER:
						result.type = LOX_BOOLEAN;
						result.boolean = left.number == right.number;
						break;
					case LOX_STRING:
						result.type = LOX_BOOLEAN;
						result.boolean = substrEqual(left.string, right.string);
						break;
					case LOX_BOOLEAN:
						result.type = LOX_BOOLEAN;
						result.boolean = left.boolean == right.boolean;
						break;
					case LOX_NIL:
						result.type = LOX_BOOLEAN;
						result.boolean = 1;
						break;
					default:
						// @todo: Error handling
						result.type = INVALID_VALUE;
						reportError3(expr->op->line, "Invalid Expression");
						break;
				}
			}
		case BANG_EQUAL:
			if (left.type != right.type) {
				result.type = LOX_BOOLEAN;
				result.boolean = 1;
			} else {
				switch (left.type) {
					case LOX_NUMBER:
						result.type = LOX_BOOLEAN;
						result.boolean = left.number != right.number;
						break;
					case LOX_STRING:
						result.type = LOX_BOOLEAN;
						result.boolean = !substrEqual(left.string, right.string);
						break;
					case LOX_BOOLEAN:
						result.type = LOX_BOOLEAN;
						result.boolean = left.boolean != right.boolean;
						break;
					case LOX_NIL:
						result.type = LOX_BOOLEAN;
						result.boolean = 0;
						break;
					default:
						// @todo: Error handling
						result.type = INVALID_VALUE;
						reportError3(expr->op->line, "Invalid Expression");
						break;
				}
			}


		default:
			// @todo: Error handling
			result.type = INVALID_VALUE;
			reportError3(expr->value->line, "Invalid Expression");
			break;
	}

	return result;
}

LoxValue		evalVariable(Expr *expr) {
	LoxValue	*value = NULL;
	Env			*env = state->currentEnv;

	while (value == NULL && env != NULL) {
		value = (LoxValue *)get(env->env, &expr->value->lexeme);
		env = env->enclosing;
	}

	if (value != NULL) {
		return *value;
	}

	// @todo: Error handling
	LoxValue	result = {
		.type = INVALID_VALUE
	};
	reportError3(expr->value->line, "Undefined variable"); // @todo: add variable lexeme in the error message

	return  result;
}

LoxValue		evalAssignment(Expr *expr) {
	LoxValue	value = evalExpr(expr->right);
	Env			*env = state->currentEnv;

	while (env != NULL) {
		if (keyExist(currentEnvMap(state), &expr->identifier->lexeme)) {
			put(currentEnvMap(state), &expr->identifier->lexeme, (void *)&value);

			return value;
		}

		env = env->enclosing;
	}

	// @todo: Error handling
	LoxValue	result = {
		.type = INVALID_VALUE
	};
	reportError3(expr->value->line, "Undefined variable"); // @todo: add variable lexeme in the error message

	return  result;
}

LoxValue		evalExpr(Expr *expr) {
	switch (expr->type) {
		case LITERAL:
			return evalLiteral(expr);
		case GROUPING:
			return evalExpr(expr->inner);
		case UNARY:
			return evalUnary(expr);
		case BINARY:
			return evalBinary(expr);
		case VARIABLE:
			return evalVariable(expr);
		case ASSIGNMENT:
			return evalAssignment(expr);
		default:
			// @todo: Error handling
			LoxValue	result;

			result.type = INVALID_VALUE;
			reportError3(expr->value->line, "Invalid Expression");
			return result;
	}
}

void			execExpressionStatement(Stmt *statement) {
	evalExpr(statement->inner);
}

void			execPrintStatement(Stmt *statement) {
	LoxValue	result;

	result = evalExpr(statement->inner);

	if (result.type != INVALID_VALUE) {
		switch (statement->inner->type) {
			case BINARY:
				printf("[line: %d]: ", statement->inner->left->value->line);
				break;
			case GROUPING:
				printf("[line: %d]: ", statement->inner->inner->value->line);
				break;
			case UNARY:
				printf("[line: %d]: ", statement->inner->op->line);
				break;
			case LITERAL:
			case VARIABLE:
				printf("[line: %d]: ", statement->inner->value->line);
				break;
		}
		printLoxValue(&result);
		printf("\n");
	}
}

void			execDeclarationStatement(Stmt *statement) {
	LoxValue	value;

	value.type = LOX_NIL;

	if (statement->initializer) {
		value = evalExpr(statement->initializer);
	}

	put(currentEnvMap(state), &statement->identifier->lexeme, (void *)&value);
}

void			execBlock(Stmt *statement) {
	// If the block is empty we return immediately
	if (statement->statements.start == NULL) {
		return;
	}

	// ----- Push new env -----
	Env			*newEnv = (Env *)getNext(state->environments);

	newEnv->enclosing = state->currentEnv;
	newEnv->env = initMap(sizeof(LoxValue), true, true);

	if (newEnv->env == NULL) {
		// @todo: error handling
		ASSERT(false)
	}

	state->currentEnv = newEnv;

	// ----- Execute statements -----
	uint32		index = 0;

	while (index < statement->statements.length) {
		index += execStatement(&((Stmt *)statement->statements.start)[index]);
	}

	// ----- Pop new env -----
	// @improvements: maybe we dont free the first 4-5 env since its likely that we will need it again
	freeMap(newEnv->env);
	state->currentEnv = (Env *)pop(state->environments);
}

uint32			execStatement(Stmt *statement) {
	switch (statement->type) {
		case STMT:
			execExpressionStatement(statement);
			break;
		case PRINT_STMT:
			execPrintStatement(statement);
			break;
		case DECL_STMT:
			execDeclarationStatement(statement);
			break;
		case BLOCK:
			execBlock(statement);
			return statement->statements.length + 1; // We skip the statements executed by the block itself
		default:
			// @todo: Error handling
			if (statement->inner) {
				reportError3(statement->inner->value->line, "Invalid Statement");
			} else {
				reportError3(statement->blockStart->line, "Invalid Block");
			}
	}

	return 1;
}

void 			eval(State *interpreterState, Array *statements) {
	uint32		index = 0;

	state = interpreterState;

	while (index < statements->length) {
		index += execStatement(&((Stmt *)statements->data)[index]);
	}
}
