//*****************************************************************************
// purpose: CSE 4713 / 6713 Assignment 3 example recursive descent parser
//  author: J. Edward Swan II
//  editor: Josh Hawkins
//*****************************************************************************

#ifndef LEXER_H
#define LEXER_H

// Maximum lexeme length
#define MAX_LEXEME_LEN 100

// List of token codes

// Keywords
#define TOK_LET          1011
#define TOK_READ         1012
#define TOK_PRINT        1013
#define TOK_IF           1014
#define TOK_ELSE         1015
#define TOK_WHILE        1016
#define TOK_NOT          1021
#define TOK_AND          1022
#define TOK_OR           1023

// Punctuation
#define TOK_SEMICOLON    2000
#define TOK_OPENPAREN    2001
#define TOK_CLOSEPAREN   2002
#define TOK_OPENBRACE    2005
#define TOK_CLOSEBRACE   2006
#define TOK_LESSTHAN     2010
#define TOK_GREATERTHAN  2011
#define TOK_EQUALTO      2012

// Operators
#define TOK_PLUS         3000
#define TOK_MINUS        3001
#define TOK_MULTIPLY     3002
#define TOK_DIVIDE       3003
#define TOK_ASSIGN       3004

// Useful abstractions
#define TOK_IDENTIFIER   4000  // identifier
#define TOK_INTLIT       4001  // integer literal
#define TOK_FLOATLIT     4002  // float literal
#define TOK_STRINGLIT    4003  // string literal
#define TOK_EOF          5000  // end of file
#define TOK_EOF_SL       5001  // end of file while parsing string literal
#define TOK_UNKNOWN      6000  // unknown lexeme

#endif
