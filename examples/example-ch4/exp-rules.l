/******************************************************************* 
purpose: CSE 4713 / 6713 Ch4 example recursive descent parser
 author: J. Edward Swan II
 *******************************************************************/
%option noyywrap
%{
#include "lexer.h"
%}

%%
 
 /* Punctuation */

\(        { return TOK_OPENPAREN; }
\)        { return TOK_CLOSEPAREN; }

 /* Operators */

\+        { return TOK_PLUS; }
-         { return TOK_MINUS; }
\*        { return TOK_MULTIPLY; }
\/        { return TOK_DIVIDE; }

 /* Abstractions */

[A-Za-z][0-9A-Za-z_]*  { return TOK_IDENTIFIER; }
[0-9]+                 { return TOK_INTLIT; }

 /* Eat any whitespace */

[ \t\r\n]+             /* nop */

 /* Found an unknown character */

.         { return TOK_UNKNOWN; }

 /* Recognize end of file */

<<EOF>>   { return TOK_EOF; }


