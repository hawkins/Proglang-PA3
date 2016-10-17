//*****************************************************************************
// purpose: CSE 4713 / 6713 Ch4 example recursive descent parser
// created: 10-26-2014
//  author: J. Edward Swan II
//*****************************************************************************

#ifndef LEXER_H
#define LEXER_H

// Include standard IO streams
#include <stdio.h>

// Maximum lexeme length
#define MAX_LEXEME_LEN 100

// List of token codes

// Punctuation
#define TOK_OPENPAREN    2001	
#define TOK_CLOSEPAREN   2002	

// Operators
#define TOK_PLUS         3000	
#define TOK_MINUS        3001	
#define TOK_MULTIPLY     3002	
#define TOK_DIVIDE       3003	

// Useful abstractions
#define TOK_IDENTIFIER   4000  // identifier
#define TOK_INTLIT       4001  // integer literal 
#define TOK_EOF          5000  // end of file
#define TOK_EOF_SL       5001  // end of file while parsing string literal
#define TOK_UNKNOWN      6000  // unknown lexeme

#endif

