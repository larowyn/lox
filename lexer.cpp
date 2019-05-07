//
// Created by Paul Motte on 2019-05-04.
//

#include <cstdio>

#include "lexer.h"
#include "state.h"
#include "error.h"
#include "ptb/ptb_standard.h"
#include "ptb/ptb_array.h"

#define DEBUG_ENABLED 0
#define DEBUG(expression) if (DEBUG_ENABLED) {expression}

void 	lex(State *state, char *source, Array *tokens) {
	Token	*token = (Token *)getNext(tokens);
	int32 	current = 0;
	int32	length = 0;
	int32 	line = 1;

	do {
		token->line = line;
		token->offset = current;

		if (source[current] == '\0') {
			token->type = LOX_EOF;
			token->length = 1;

			// @todo @investigate: treat EOF as the other tokens ???
			DEBUG(
			printToken(token);
			printf("\n");
			)

			break;
		}

		switch (source[current]) {
			// Single-character tokens.
			case '(':
				length = 1;
				token->type = LEFT_PAREN;
				break;
			case ')':
				length = 1;
				token->type = RIGHT_PAREN;
				break;
			case '{':
				length = 1;
				token->type = LEFT_BRACE;
				break;
			case '}':
				length = 1;
				token->type = RIGHT_BRACE;
				break;
			case ',':
				length = 1;
				token->type = COMMA;
				break;
			case '.':
				length = 1;
				token->type = DOT;
				break;
			case '-':
				length = 1;
				token->type = MINUS;
				break;
			case '+':
				length = 1;
				token->type = PLUS;
				break;
			case ';':
				length = 1;
				token->type = SEMICOLON;
				break;
			case '*':
				length = 1;
				token->type = STAR;
				break;

			// One or two character tokens.
			case '!':
				length = source[current + 1] == '=' ? 2 : 1;
				token->type = source[current + 1] == '=' ? BANG_EQUAL : BANG;
				break;
			case '=':
				length = source[current + 1] == '=' ? 2 : 1;
				token->type = source[current + 1] == '=' ? EQUAL_EQUAL : EQUAL;
				break;
			case '<':
				length = source[current + 1] == '=' ? 2 : 1;
				token->type = source[current + 1] == '=' ? LESS_EQUAL : LESS;
				break;
			case '>':
				length = source[current + 1] == '=' ? 2 : 1;
				token->type = source[current + 1] == '=' ? GREATER_EQUAL : GREATER;
				break;

			// Literals
			case '"': // String
				length = 1;

				while (source[current + length] != '"') {
					if (source[current + length] == '\0') {
						token->type = INVALID_TOKEN;
						pushError(state, UNTERMINATED_STRING, token);
						break;
					}

					if (source[current + length] == '\n') {
						line++;
					}

					length++;
				}

				token->type = STRING;
				token->literal.string.start = &source[current + 1];
				token->literal.string.length = length - 1;

				length++; // Skip the finishing: "

				// @improvements: Add escape sequence support here

				break;

			case '0': // Number
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				length = 1;

				uint64	integerPart = source[current] - 48;
				uint64	floatingPart = 0;
				f32 	floatingDepth = 1;

				while (isDigit(source[current + length])) {
					integerPart *= 10;
					integerPart += source[current + length] - 48;

					if (integerPart > INT32_MAX) { // @todo: properly handle numeric literal too big, this test is garbage
						token->type = INVALID_TOKEN;
						pushError(state, NUMBER_TOO_BIG, token);
						//@todo: Consume the rest of the number without storing the value we can't store
					}

					length++;
				}

				if (source[current + length] == '.' && isDigit(source[current + length + 1])) {
					length++;

					while (isDigit(source[current + length])) {
						floatingDepth *= 0.1;
						floatingPart *= 10;
						floatingPart += source[current + length] - 48;

						if (floatingPart > INT32_MAX) { // @todo: properly handle numeric literal too precise, this test is garbage
							token->type = INVALID_TOKEN;
							pushError(state, NUMBER_TOO_PRECISE, token);
							//@todo: Consume the rest of the number without storing the value we can't store
						}

						length++;
					}

				}

				token->type = NUMBER;
				token->literal.number = integerPart + floatingPart * floatingDepth; // @todo: properly handle numeric literal

				break;
			}

			// Comments
			case '/':
				if (source[current + 1] == '/') {
					length = 2;

					while (source[current + length] != '\0' && source[current + length] != '\n') {
						length++;
					}
				} else {
					length = 1;
					token->type = SLASH;
				}
				break;

			// Ignore whitespace.
			case ' ':
			case '\r':
			case '\t':
				length = 1;
				token->type = BLANK_TOKEN;
				break;

			case '\n':
				line++;
				length = 1;
				token->type = BLANK_TOKEN;
				break;

			default:
				if (isAlpha(source[current]) || source[current] == '_') { // Reserved Keyword and identifier
					length = 1;

					while (
							isAlpha(source[current + length])
							|| isDigit(source[current + length])
							|| source[current + length] == '_'
					) {
						length++;
					}

					token->lexeme.start = &source[current];
					token->lexeme.length = length;

					// @todo: Use a map instead of those nasty strEqual
					if (substrEqual("and", &token->lexeme)) {
						token->type = AND;
					} else if (substrEqual("or", &token->lexeme)) {
						token->type = OR;
					} else if (substrEqual("if", &token->lexeme)) {
						token->type = IF;
					} else if (substrEqual("else", &token->lexeme)) {
						token->type = ELSE;
					} else if (substrEqual("true", &token->lexeme)) {
						token->type = TRUE;
					} else if (substrEqual("false", &token->lexeme)) {
						token->type = FALSE;
					} else if (substrEqual("for", &token->lexeme)) {
						token->type = FOR;
					} else if (substrEqual("while", &token->lexeme)) {
						token->type = WHILE;
					} else if (substrEqual("fun", &token->lexeme)) {
						token->type = FUN;
					} else if (substrEqual("var", &token->lexeme)) {
						token->type = VAR;
					} else if (substrEqual("return", &token->lexeme)) {
						token->type = RETURN;
					} else if (substrEqual("nil", &token->lexeme)) {
						token->type = NIL;
					} else if (substrEqual("class", &token->lexeme)) {
						token->type = CLASS;
					} else if (substrEqual("this", &token->lexeme)) {
						token->type = THIS;
					} else if (substrEqual("super", &token->lexeme)) {
						token->type = SUPER;
					} else if (substrEqual("print", &token->lexeme)) {
						token->type = PRINT;
					} else {
						token->type = IDENTIFIER;
					}

					break;
				} else { // Unexpected character
					length = 1;
					token->type = INVALID_TOKEN;
					pushError(state, ERROR_INVALID_TOKEN, token);
				}

				break;
		}

		if (token->type != BLANK_TOKEN) {
			token->length = length;
			token->lexeme.start = &source[current];
			token->lexeme.length = length;

			DEBUG(
			printToken(token);
			printf("\n");
			)

			token = (Token *)getNext(tokens);
		}

		current += length;
	} while (length);

	DEBUG(
	printf("\n");
	printf("Tokens number: %d\n", tokens->size);
	)
}
