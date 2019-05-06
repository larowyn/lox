//
// Created by Paul Motte on 2019-05-05.
//

#include "ptb/ptb_stack.h"
#include "state.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

void			reportError3(int32 line, char *message) {
	printf("\033[31m"); // red
	printf("[line: %d] Error: %s", line, message);
	printf("\033[0m\n"); // reset + \n
}

bool32			isTruthy(LoxValue value) {
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
			printf("[line: %d]: ", statement->inner->value->line);
			break;
	}
	printLoxValue(&result);
	printf("\n");
}

void			execStatement(Stmt *statement) {
	switch (statement->type) {
		case STMT:
			execExpressionStatement(statement);
			break;
		case PRINT_STMT:
			execPrintStatement(statement);
			break;
		default:
			// @todo: Error handling
			reportError3(statement->inner->value->line, "Invalid Statement");
	}
}

void 			eval(State *state, Stack *statements) {
	Stmt		*statement = (Stmt *)getStart(statements);

	while (statement - (Stmt *)statements->content <= statements->length) {
		execStatement(statement);

		statement++;
	}
}
