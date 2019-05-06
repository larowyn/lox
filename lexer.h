//
// Created by Paul Motte on 2019-05-04.
//

#ifndef LOX_LEXER_H
#define LOX_LEXER_H

#include <cstdio>

#include "ptb/ptb_types.h"
#include "ptb/ptb_stack.h"
#include "ptb/ptb_substring.h"
#include "state.h"

enum 			TokenType {
	INVALID_TOKEN,

	// Single-character tokens.
	LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
	COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

	// One or two character tokens.
	BANG, BANG_EQUAL,
	EQUAL, EQUAL_EQUAL,
	GREATER, GREATER_EQUAL,
	LESS, LESS_EQUAL,

	// Literals.
	IDENTIFIER, STRING, NUMBER,

	// Keywords.
	AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
	PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

	LOX_EOF
};

inline char 	*tokenTypeToString(int32 type) {
	switch (type) {
		case INVALID_TOKEN:
			return "INVALID_TOKEN";
		case LEFT_PAREN:
			return "LEFT_PAREN";
		case RIGHT_PAREN:
			return "RIGHT_PAREN";
		case LEFT_BRACE:
			return "LEFT_BRACE";
		case RIGHT_BRACE:
			return "RIGHT_BRACE";
		case COMMA:
			return "COMMA";
		case DOT:
			return "DOT";
		case MINUS:
			return "MINUS";
		case PLUS:
			return "PLUS";
		case SEMICOLON:
			return "SEMICOLON";
		case SLASH:
			return "SLASH";
		case STAR:
			return "STAR";
		case BANG:
			return "BANG";
		case BANG_EQUAL:
			return "BANG_EQUAL";
		case EQUAL:
			return "EQUAL";
		case EQUAL_EQUAL:
			return "EQUAL_EQUAL";
		case GREATER:
			return "GREATER";
		case GREATER_EQUAL:
			return "GREATER_EQUAL";
		case LESS:
			return "LESS";
		case LESS_EQUAL:
			return "LESS_EQUAL";
		case IDENTIFIER:
			return "IDENTIFIER";
		case STRING:
			return "STRING";
		case NUMBER:
			return "NUMBER";
		case AND:
			return "AND";
		case CLASS:
			return "CLASS";
		case ELSE:
			return "ELSE";
		case FALSE:
			return "FALSE";
		case FUN:
			return "FUN";
		case FOR:
			return "FOR";
		case IF:
			return "IF";
		case NIL:
			return "NIL";
		case OR:
			return "OR";
		case PRINT:
			return "PRINT";
		case RETURN:
			return "RETURN";
		case SUPER:
			return "SUPER";
		case THIS:
			return "THIS";
		case TRUE:
			return "TRUE";
		case VAR:
			return "VAR";
		case WHILE:
			return "WHILE";
		case LOX_EOF:
			return "LOX_EOF";
		default:
			return "UNKNOWN";
	}
}

struct 			Token {
	TokenType	type;
	Substring	lexeme;
	union {
		Substring	string;
		real64		number;
	} literal;

	int32 		line;
	int32 		offset; // From the start of the file
	int32 		length; // Length of the lexeme
};

inline void		printToken(Token *token) {
	printf("----- Token -----\n");
	printf("Lexeme: ");
	printSubstring(&token->lexeme);
	printf("\n");
	printf("Type: %s | Line: %d | Offset: %d | Length: %d\n", tokenTypeToString(token->type), token->line, token->offset, token->length);

	if (token->type == STRING) {
		printf("Literal value: ");
		printSubstring(&token->literal.string);
		printf("\n");
	}

	if (token->type == NUMBER) {
		printf("Literal value: %f\n", token->literal.number);
	}
}

void 			lex(State *state, char *source, Stack *tokens);

#endif //LOX_LEXER_H
