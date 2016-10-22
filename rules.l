/*******************************************************************
purpose: CSE 4713 / 6713 Assignment 3 example recursive descent parser
 author: J. Edward Swan II
 editor: Josh Hawkins
 *******************************************************************/
%option noyywrap
%{
#include "lexer.h"

int yyLine = 1;
%}

%%

 /* Keywords */

let       { return TOK_LET; }

 /* Punctuation */

;         { return TOK_SEMICOLON; }
\(        { return TOK_OPENPAREN; }
\)        { return TOK_CLOSEPAREN; }
\{        { return TOK_OPENBRACE; }
\}        { return TOK_CLOSEBRACE; }


 /* Operators */

\+        { return TOK_PLUS; }
-         { return TOK_MINUS; }
\*        { return TOK_MULTIPLY; }
\/        { return TOK_DIVIDE; }
:=        { return TOK_ASSIGN; }

 /* Abstractions */

[A-Za-z][0-9A-Za-z_]*  { return TOK_IDENTIFIER; }
[0-9]+                 { return TOK_INTLIT; }

 /* Eat any whitespace */

[ \t\r]+               /* nop */

 /* Eat newline characters, AND increment line counter */

[\n]      { yyLine++; }

 /* Found an unknown character */

.         { return TOK_UNKNOWN; }

 /* Recognize end of file */

<<EOF>>   { return TOK_EOF; }
