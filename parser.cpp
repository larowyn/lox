//
// Created by Paul Motte on 2019-05-04.
//

#include "ptb/ptb_array.h"
#include "state.h"
#include "error.h"
#include "parser.h"

GLOBAL State		*state;
GLOBAL Token		*tokens;
GLOBAL int32		current = 0;
GLOBAL ErrorCode	errorCode;

INTERNAL bool		matchType(TokenType type) {
	return tokens[current].type == type;
}

INTERNAL Token		*currentToken() {
	return &tokens[current];
}

INTERNAL void		consume() {
	current++;

	while (matchType(INVALID_TOKEN)) {
		current++;
	}
}

void				synchronize() {
	while (!matchType(LOX_EOF)) {
		switch (currentToken()->type) {
			case CLASS:
			case FUN:
			case VAR:
			case FOR:
			case IF:
			case WHILE:
			case PRINT:
			case RETURN:
				if (tokens->length > 1 && tokens[current - 1].type == SEMICOLON) {
					return;
				}
				break;
			default:
				break;
		}

		consume();
	}

	errorCode = UNTERMINATED_STATEMENT;
}

Stmt				*declaration(Array *statements, Array *expressions);
Stmt				*declarationStatement(Array *statements, Array *expressions);
Stmt				*statement(Array *statements, Array *expressions);
Expr				*expression(Array *expressions);

Expr				*primary(Array *expressions) {
	Expr			*expr = (Expr *)getNext(expressions);

	switch (currentToken()->type) {
		case LEFT_PAREN:
			expr->type = GROUPING;

			consume(); // (

			expr->inner = expression(expressions);
			consume();

			if (!matchType(RIGHT_PAREN)) {
				expr->type = INVALID_EXPR;
				expr->value = currentToken();
				errorCode = UNTERMINATED_GROUP;
			} else {
				consume(); // )
			}
			break;
		case IDENTIFIER:
			expr->type = VARIABLE;
			expr->identifier = currentToken();
			consume();
			break;
		case TRUE:
		case FALSE:
		case NIL:
		case NUMBER:
		case STRING:
			expr->type = LITERAL;
			expr->value = currentToken();
			consume();
			break;
		default:
			expr->type = INVALID_EXPR;
			expr->value = currentToken();
			errorCode = INVALID_EXPRESSION;
			consume();
			break;
	}

	return expr;
}

Expr				*unary(Array *expressions) {
	if (matchType(BANG) || matchType(MINUS)) {
		Expr		*unaryExpr = (Expr *)getNext(expressions);

		unaryExpr->type = UNARY;
		unaryExpr->op = currentToken();

		consume();
		unaryExpr->right = unary(expressions);

		return unaryExpr;
	}

	return primary(expressions);
}

// @todo: refacto all binaries expression handler into one that take the array of token to put in the while
Expr				*multiplication(Array *expressions) {
	Expr			*expr = unary(expressions);

	while (matchType(STAR) || matchType(SLASH)) {
		Expr	*binaryExpr = (Expr *)getNext(expressions);

		binaryExpr->type = BINARY;
		binaryExpr->left = expr;

		binaryExpr->op = currentToken();
		consume();

		binaryExpr->right = unary(expressions);
		expr = binaryExpr;
	}

	return expr;
}

Expr				*addition(Array *expressions) {
	Expr			*expr = multiplication(expressions);

	while (matchType(PLUS) || matchType(MINUS)) {
		Expr	*binaryExpr = (Expr *)getNext(expressions);

		binaryExpr->type = BINARY;
		binaryExpr->left = expr;

		binaryExpr->op = currentToken();
		consume();

		binaryExpr->right = multiplication(expressions);
		expr = binaryExpr;
	}

	return expr;
}

Expr				*comparison(Array *expressions) {
	Expr			*expr = addition(expressions);

	while (
		matchType(GREATER)
		|| matchType(GREATER_EQUAL)
		|| matchType(LESS)
		|| matchType(LESS_EQUAL)
	) {
		Expr	*binaryExpr = (Expr *)getNext(expressions);

		binaryExpr->type = BINARY;
		binaryExpr->left = expr;

		binaryExpr->op = currentToken();
		consume();

		binaryExpr->right = addition(expressions);
		expr = binaryExpr;
	}

	return expr;
}

Expr				*equality(Array *expressions) {
	Expr			*expr = comparison(expressions);

	while (matchType(EQUAL_EQUAL) || matchType(BANG_EQUAL)) {
		Expr	*binaryExpr = (Expr *)getNext(expressions);

		binaryExpr->type = BINARY;
		binaryExpr->left = expr;

		binaryExpr->op = currentToken();
		consume();

		binaryExpr->right = comparison(expressions);
		expr = binaryExpr;
	}

	return expr;
}

Expr				*logicAnd(Array *expressions) {
	Expr			*expr = equality(expressions);

	if (matchType(AND)) {
		Expr	*logicAndExpr = (Expr *)getNext(expressions);

		logicAndExpr->type = LOGIC_OR;
		logicAndExpr->left = expr;

		logicAndExpr->op = currentToken();
		consume(); // ||

		logicAndExpr->right = equality(expressions);
		expr = logicAndExpr;
	}

	return expr;
}

Expr				*logicOr(Array *expressions) {
	Expr			*expr = logicAnd(expressions);

	if (matchType(OR)) {
		Expr	*logicOrExpr = (Expr *)getNext(expressions);

		logicOrExpr->type = LOGIC_OR;
		logicOrExpr->left = expr;

		logicOrExpr->op = currentToken();
		consume(); // ||

		logicOrExpr->right = logicAnd(expressions);
		expr = logicOrExpr;
	}

	return expr;
}

Expr				*assignment(Array *expressions) {
	Expr			*expr = logicOr(expressions);

	if (matchType(EQUAL)) {
		if (expr->type != VARIABLE) {
			expr->type = INVALID_EXPR;
			expr->value = currentToken();
			errorCode = INVALID_ASSIGNEMENT;
			return expr;
		}

		expr->type = ASSIGNMENT;

		consume(); // =
		expr->right = assignment(expressions);

		return expr;
	}

	return expr;
}

Expr				*expression(Array *expressions) {
	return assignment(expressions);
}

Stmt				*expressionStatement(Array *statements, Array *expressions) {
	Stmt			*stmt = (Stmt *)getNext(statements);

	stmt->type = STMT;
	stmt->inner = expression(expressions);

	return stmt;
}

Stmt				*printStatement(Array *statements, Array *expressions) {
	Stmt			*stmt = (Stmt *)getNext(statements);

	stmt->type = PRINT_STMT;

	consume(); // print
	stmt->inner = expression(expressions);

	return stmt;
}

Stmt				*blockStatement(Array *statements, Array *expressions) {
	Stmt			*blockStmt = (Stmt *)getNext(statements);

	blockStmt->type = BLOCK_STMT;
	blockStmt->stmtStart = currentToken();

	consume(); // {

	blockStmt->statements.dataSize = sizeof(Stmt *);
	blockStmt->statements.start = NULL;
	blockStmt->statements.length = 0;

	while (!matchType(LOX_EOF) && !matchType(RIGHT_BRACE)) {
		Stmt		*stmt = declaration(statements, expressions);

		if (blockStmt->statements.length == 0) {
			blockStmt->statements.start = (byte *)stmt;
		}

		blockStmt->statements.length += stmt->type == BLOCK_STMT ? (stmt->statements.length + 1) : 1;

		errorCode = INVALID_ERROR_CODE; // Reset error code between statements
	}

	if (!matchType(RIGHT_BRACE)) {
		blockStmt->type = INVALID_STMT;
		blockStmt->inner->type = INVALID_EXPR;
		blockStmt->inner->value = currentToken();
		errorCode = UNTERMINATED_BLOCK;
		return blockStmt;
	}

	consume(); // }

	return blockStmt;
}

Stmt				*ifStatement(Array *statements, Array *expressions) {
	Stmt			*ifStmt = (Stmt *)getNext(statements);

	ifStmt->type = IF_STMT;
	ifStmt->stmtStart = currentToken();

	consume(); // if

	if (!matchType(LEFT_PAREN)) {
		ifStmt->type = INVALID_STMT;
		ifStmt->inner->type = INVALID_EXPR;
		ifStmt->inner->value = currentToken();
		errorCode = IF_MISSING_LEFT_PAREN;
		return ifStmt;
	}

	consume(); // (

	ifStmt->condition = expression(expressions);

	if (!matchType(RIGHT_PAREN)) {
		ifStmt->type = INVALID_STMT;
		ifStmt->inner->type = INVALID_EXPR;
		ifStmt->inner->value = currentToken();
		errorCode = CONDITION_MISSING_RIGHT_PAREN;
		return ifStmt;
	}

	consume(); // )

	ifStmt->thenBranch = statement(statements, expressions);

	ifStmt->elseBranch = NULL;
	if (matchType(ELSE)) {
		ifStmt->elseBranch = statement(statements, expressions);
	}

	return ifStmt;
}

Stmt				*whileStatement(Array *statements, Array *expressions) {
	Stmt			*whileStmt = (Stmt *)getNext(statements);

	whileStmt->type = WHILE_STMT;
	whileStmt->stmtStart = currentToken(); // @todo: set stmtStart for all statement

	consume(); // while

	if (!matchType(LEFT_PAREN)) {
		whileStmt->type = INVALID_STMT;
		whileStmt->inner->type = INVALID_EXPR;
		whileStmt->inner->value = currentToken();
		errorCode = WHILE_MISSING_LEFT_PAREN;
		return whileStmt;
	}

	consume(); // (

	whileStmt->condition = expression(expressions);

	if (!matchType(RIGHT_PAREN)) {
		whileStmt->type = INVALID_STMT;
		whileStmt->inner->type = INVALID_EXPR;
		whileStmt->inner->value = currentToken();
		errorCode = CONDITION_MISSING_RIGHT_PAREN;
		return whileStmt;
	}

	consume(); // )

	whileStmt->body = statement(statements, expressions);

	return whileStmt;
}

// Implemented by 'desugaring' it into two blocks with a while
// @todo: Because we desugar the for into blocks and a while the debug/error display will be fuck, find a way to fix it
Stmt				*forStatement(Array *statements, Array *expressions) {
	Stmt			*outerStmt = (Stmt *)getNext(statements);

	outerStmt->stmtStart = currentToken();

	consume(); // for

	if (!matchType(LEFT_PAREN)) {
		outerStmt->type = INVALID_STMT;
		outerStmt->inner->type = INVALID_EXPR;
		outerStmt->inner->value = currentToken();
		errorCode = FOR_MISSING_LEFT_PAREN;
		return outerStmt;
	}

	consume(); // (

	outerStmt->statements.start = NULL;
	outerStmt->statements.length = 0;
	outerStmt->statements.dataSize = sizeof(Stmt *);

	// ----- Initializer -----
	Stmt			*initializerStmt = NULL;

	if (matchType(VAR)) {
		initializerStmt = declarationStatement(statements, expressions);
	} else if (!matchType(SEMICOLON)) {
		initializerStmt = expressionStatement(statements, expressions);
	}

	if (!matchType(SEMICOLON)) {
		outerStmt->type = INVALID_STMT;
		outerStmt->inner->type = INVALID_EXPR;
		outerStmt->inner->value = currentToken();
		pushError(state, FOR_MISSING_SEMICOLON_AFTER_INITIALIZER, outerStmt);
	} else {
		consume(); // ;
	}


	// ----- Condition -----
	Expr			*condition = NULL;

	if (!matchType(SEMICOLON)) {
		condition = expression(expressions);
	} else {
		// @bug: condition should be a LITERAL with a Token TRUE, but we dont want to alloc a new token so we will just
		// simulate it in the interpreter, it may cause may cause issue
	}

	if (!matchType(SEMICOLON)) {
		outerStmt->type = INVALID_STMT;
		outerStmt->inner->type = INVALID_EXPR;
		outerStmt->inner->value = currentToken();
		pushError(state, FOR_MISSING_SEMICOLON_AFTER_CONDITION, outerStmt);
	} else {
		consume(); // ;
	}


	// ----- Iterator -----
	Expr			*iterator = NULL;

	if (!matchType(RIGHT_PAREN)) {
		iterator = expression(expressions);
	}

	if (!matchType(RIGHT_PAREN)) {
		outerStmt->type = INVALID_STMT;
		outerStmt->inner->type = INVALID_EXPR;
		outerStmt->inner->value = currentToken();
		errorCode = FOR_MISSING_RIGHT_PAREN;
		return outerStmt;
	}

	consume(); // )

	// ----- Actual Statement -----
	Stmt			*whileStmt = initializerStmt != NULL ? (Stmt *)getNext(statements) : outerStmt;


	whileStmt->stmtStart = currentToken(); // @todo: condition token if condition, else find out something
	whileStmt->type = WHILE_STMT;
	whileStmt->condition = condition;

	 // If there is an initializer we wrap the whole thing in a block
	if (initializerStmt != NULL) {
		outerStmt->type = BLOCK_STMT;

		outerStmt->statements.start = (byte *)initializerStmt;
		outerStmt->statements.length = 2; // initializer + while
	}

	if (iterator != NULL) { // We wrap the body in a block
		whileStmt->body = (Stmt *)getNext(statements);

		whileStmt->body->type = BLOCK_STMT;
		whileStmt->body->statements.start = (byte *)statement(statements, expressions);
		whileStmt->body->stmtStart = ((Stmt *)whileStmt->body->statements.start)->stmtStart;

		Stmt		*iteratorStatement = (Stmt *)getNext(statements);

		iteratorStatement->type = STMT;
		iteratorStatement->inner = iterator;

		whileStmt->body->statements.length = 2; // body + iterator
	} else {
		whileStmt->body = statement(statements, expressions);
	}

	return outerStmt;
}

Stmt				*statement(Array *statements, Array *expressions) {
	Stmt			*stmt;

	switch (currentToken()->type) {
		case PRINT:
			stmt = printStatement(statements, expressions);
			break;
		case LEFT_BRACE:
			stmt = blockStatement(statements, expressions);
			break;
		case IF:
			stmt = ifStatement(statements, expressions);
			break;
		case WHILE:
			stmt = whileStatement(statements, expressions);
			break;
		case FOR:
			stmt = forStatement(statements, expressions);
			break;
		default:
			stmt = expressionStatement(statements, expressions);
			break;
	}

	return stmt;
}

Stmt				*declarationStatement(Array *statements, Array *expressions) {
	Stmt			*stmt = (Stmt *)getNext(statements);

	stmt->type = DECL_STMT;

	consume(); // VAR

	if (!matchType(IDENTIFIER)) {
		stmt->type = INVALID_STMT;
		stmt->inner->type = INVALID_EXPR;
		stmt->inner->value = currentToken();
		errorCode = UNTERMINATED_GROUP;
	} else {
		stmt->initializer = NULL;
		stmt->identifier = currentToken();
		consume(); // IDENTIFIER

		if (matchType(EQUAL)) {
			consume(); // EQUAL
			stmt->initializer = expression(expressions);
		}
	}

	return stmt;
}

Stmt				*declaration(Array *statements, Array *expressions) {
	Stmt			*stmt;

	if (matchType(VAR)) {
		stmt = declarationStatement(statements, expressions);
	} else {
		stmt = statement(statements, expressions);
	}

	if (stmt->type == INVALID_STMT || (stmt->inner && stmt->inner->type == INVALID_EXPR)) {
		if (stmt->type == INVALID_STMT) {
			pushError(state, errorCode, stmt);
		} else {
			pushError(state, errorCode, stmt->inner);
		}

		errorCode = INVALID_ERROR_CODE;

		stmt->type = INVALID_STMT;
		stmt->inner->type = INVALID_EXPR;

		// Synchronize
		synchronize();
	} else {
		if ( // Skip ';' check
			stmt->type == BLOCK_STMT
			|| stmt->type == IF_STMT
			|| stmt->type == WHILE_STMT
		) {
			return stmt;
		}

		if (!matchType(SEMICOLON)) {
			stmt->type = INVALID_STMT;
			pushError(state, UNTERMINATED_STATEMENT, stmt);
		} else {
			consume(); // ;
		}
	}

	return stmt;
}

void 				parse(State *interpreterState, Array *tokensArray, Array *statements, Array *expressions) {
	state = interpreterState;
	tokens = (Token *)getStart(tokensArray);

	// Skip potential starting invalid tokens
	while (matchType(INVALID_TOKEN)) {
		current++;
	}

	while (!matchType(LOX_EOF)) {
		declaration(statements, expressions);
		errorCode = INVALID_ERROR_CODE;
	}
}

uint32			DEBUG_printStatements(char *source, Array *statements, uint32 length, int32 nesting) {
	Stmt		*statement = (Stmt *)statements->data;

	if (nesting) {
		printf("\033[%dm", 32 + nesting);
	}

	while (statement - (Stmt *)statements->data < length) {
		printf("%*s", nesting * 8, "");
		printf("----- Statement %ld -----\n", statement - (Stmt *)statements->data);

		if (statement->type == BLOCK_STMT) {
			printf("%*s", nesting * 8, "");
			printf("%d | ", statement->stmtStart->line);
		} else {
			switch (statement->inner->type) {
				case BINARY:
					printf("%*s", nesting * 8, "");
					printf("%d | ", statement->inner->left->value->line);
					break;
				case GROUPING:
					printf("%*s", nesting * 8, "");
					printf("%d | ", statement->inner->inner->value->line);
					break;
				case UNARY:
					printf("%*s", nesting * 8, "");
					printf("%d | ", statement->inner->op->line);
					break;
				case LITERAL:
				case VARIABLE:
					printf("%*s", nesting * 8, "");
					printf("%d | ", statement->inner->value->line);
					break;
			}
		}

		Token		*token;

		if (statement->type == BLOCK_STMT) {
			token = statement->stmtStart;
		} else {
			switch (statement->inner->type) {
				case BINARY:
					token = statement->inner->left->value;
				case GROUPING:
					token = statement->inner->inner->value;
				case UNARY:
					token = statement->inner->op;
				case VARIABLE:
				case LITERAL:
					token = statement->inner->value;
				case INVALID_EXPR:
					token = statement->inner->value;
			}
		}

		uint32		startOfLine = token->offset;

		while (startOfLine && source[startOfLine] != '\n') {
			startOfLine--;
		}

		if (source[startOfLine] == '\n') startOfLine++;

		uint32		length = token->offset - startOfLine;

		while (
			source[startOfLine + length] != '\0'
			&& source[startOfLine + length] != '\n'
		) {
			length++;
		}

		printf("%*s", nesting * 8, "");
		printf("%.*s\n\n", length, &source[startOfLine]);

		printf("%*s", nesting * 8, "");
		printf("{\n");

		printf("%*s", nesting * 8, "");
		printf("\ttype: %s\n", stmtTypeToString(statement->type));

		printf("%*s", nesting * 8, "");
		printf("}\n\n");

		if (statement->type == WHILE_STMT) {
			printf("\033[%dm", 32 + nesting + 1);

			printf("%*s", (nesting + 1) * 8, "");
			printf("----- While Body: -----\n");

			DEBUG_printStatements(source, (Array *)&statement->body, 1, nesting + 1);

			statement++; // skip the body of the loop

			if (nesting) {
				printf("\033[%dm", 32 + nesting);
			} else {
				printf("\033[0m"); // reset
			}
		} else if (statement->type == BLOCK_STMT) {
			printf("\033[%dm", 32 + nesting + 1);

			printf("%*s", (nesting + 1) * 8, "");
			printf("----- Block Statements (%d): -----\n", statement->statements.length);

			DEBUG_printStatements(source, (Array *)&statement->statements.start, statement->statements.length, nesting + 1);

			statement += statement->statements.length;

			if (nesting) {
				printf("\033[%dm", 32 + nesting);
			} else {
				printf("\033[0m"); // reset
			}
		}

		statement++;
	}
}
