/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

#define _DEBUG 0
int lineCount = 0;


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  // TODO
  while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE)
	{
		readChar();
	}
}

void skipComment() {
  // TODO
  int state = 0;
  while (currentChar != EOF && state != 2)
  {
      readChar();
      switch(charCodes[currentChar])
      {
        case CHAR_TIMES:
            state = 1;
            break;
        case CHAR_RPAR:
            if (state == 1)
            {
                state = 2;
                readChar();
            }
            break;
        default:
            state = 0;
      }
  }
  if (state != 2)
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
}

Token* readIdentKeyword(void) {
  // TODO
    Token* token = makeToken(TK_IDENT, lineNo, colNo);
    char string[MAX_IDENT_LEN + 1];
    int i = 0, state = 0;
    while (currentChar != EOF && (charCodes[currentChar] == CHAR_DIGIT || charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_UNDERSCORE))
        {
            if (i >= MAX_IDENT_LEN)
            {
		error(ERR_IDENTTOOLONG, lineNo, colNo);
                readChar();
                continue;
            }
	    if (state == 0 && charCodes[currentChar] == CHAR_UNDERSCORE)
	      {
		state = 1;
		string[i++] = currentChar;
		readChar();
	      }
	    else if (state == 1 && charCodes[currentChar] == CHAR_UNDERSCORE)
	      break;
	    else
	      {
		string[i++] = currentChar;
		readChar();
	      }
        }

    string[i] = 0;

	// make token, because checkKeyword returns TK_NONE if kw is not type of Keyword,
	// so we make token type is TK_IDENT instead of TK_NONE
    TokenType tokenType = checkKeyword(string);
    if (charCodes[string[0]] == CHAR_UNDERSCORE || charCodes[string[0]] == CHAR_LETTER)
      {
	if (tokenType == TK_NONE)
		token->tokenType = TK_IDENT;
	else
		token->tokenType = tokenType;
	strcpy(token->string, string);
	return token;
      }
    else
      {
	token->tokenType = tokenType;
	strcpy(token->string, string);
	return token;
      }
}

Token* readNumber(void) {
  // TODO
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
	int i = 0;
	while (currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT)
	{
		if (i >= MAX_NUMBER_LEN)
		{
			error(ERR_NUMBERTOOLONG, lineNo, colNo);
			readChar();
			continue;
		}
		token->string[i++] = currentChar;
		readChar();
	}
	token->string[i] = 0;
	token->value = atoi(token->string);
	return token;
}

Token* readConstChar(void) {
    Token * token;
	int beginColNo = colNo;
	int beginLineNo = lineNo;
	char string[MAX_IDENT_LEN + 1];

	readChar();
	string[0] = currentChar;
	string[1] = '\0';

	if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
	{
		readChar();
		if (charCodes[currentChar] != CHAR_SINGLEQUOTE)
		{
			token = makeToken(TK_NONE, lineNo, colNo);
			error(ERR_INVALIDCHARCONSTANT, beginColNo, beginLineNo);
			return token;
		}
	}

	readChar();
	switch (charCodes[currentChar])
	{
	case CHAR_SINGLEQUOTE:
		token = makeToken(TK_CHAR, beginLineNo, beginColNo);
		strcpy(token->string, string);
		readChar();
		return token;
	default:
		token = makeToken(TK_NONE, lineNo, colNo);
		error(ERR_INVALIDCHARCONSTANT, beginColNo, beginLineNo);
		return token;
	}
}

Token* getToken(void) {
  Token *token;
  int ln, cl;
  if (currentChar == EOF)
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_UNDERSCORE: return readIdentKeyword();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS:
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_LPAR:
	ln = lineNo;
	cl = colNo;
	readChar();
	switch (charCodes[currentChar])
	{
		case CHAR_TIMES:
			skipComment();
			return getToken();
		case CHAR_PERIOD:
			token = makeToken(SB_LSEL, ln, cl);
			readChar();
			return token;
		default:
			token = makeToken(SB_LPAR, ln, cl);
			return token;
	}
  case CHAR_RPAR:
	token = makeToken(SB_RPAR, lineNo, colNo);
	readChar();
	return token;
  case CHAR_TIMES:
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SLASH:
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SEMICOLON:
	token = makeToken(SB_SEMICOLON, lineNo, colNo);
	readChar();
	return token;
  case CHAR_COLON:
	ln = lineNo;
	cl = colNo;
	readChar();
	switch (charCodes[currentChar])
	{
		case CHAR_EQ:
            token = makeToken(SB_ASSIGN, ln, cl);
			readChar();
			return token;
        default:
			token = makeToken(SB_COLON, ln, cl);
			readChar();
			return token;
    }
  case CHAR_PERIOD:
	ln = lineNo;
	cl = colNo;
	readChar();
	switch (charCodes[currentChar])
	{
		case CHAR_RPAR:
			token = makeToken(SB_RSEL, ln, cl);
			readChar();
			return token;
		default:
			token = makeToken(SB_PERIOD, ln, cl);
			readChar();
			return token;
	}
  case CHAR_EQ:
	token = makeToken(SB_EQ, lineNo, colNo);
	readChar();
	return token;
  case CHAR_EXCLAIMATION:
	ln = lineNo;
	cl = colNo;
	readChar();
	switch (charCodes[currentChar])
	{
		case CHAR_EQ:
			token = makeToken(SB_NEQ, ln, cl);
			readChar();
			return token;
		default:
			token = makeToken(TK_NONE, ln, cl);
			error(ERR_INVALIDSYMBOL, ln, cl);
			readChar();
			return token;
	}
  case CHAR_COMMA:
	token = makeToken(SB_COMMA, lineNo, colNo);
	readChar();
	return token;
  case CHAR_SINGLEQUOTE:
	return readConstChar();
    // TODO
  case CHAR_LT:
	ln = lineNo;
	cl = colNo;
	readChar();
	switch (charCodes[currentChar])
	{
		case CHAR_EQ:
			token = makeToken(SB_LE, ln, cl);
			readChar();
			return token;
		default:
			token = makeToken(SB_LT, ln, cl);
			return token;
	}
  case CHAR_GT:
	ln = lineNo;
	cl = colNo;
	readChar();
	switch (charCodes[currentChar])
	{
		case CHAR_EQ:
            token = makeToken(SB_GE, ln, cl);
			readChar();
			return token;
        default:
			token = makeToken(SB_GT, ln, cl);
			return token;
    }
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}



