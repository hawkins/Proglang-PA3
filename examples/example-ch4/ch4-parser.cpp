//*****************************************************************************
// purpose: CSE 4713 / 6713 Ch4 example recursive descent parser
//  author: J. Edward Swan II
//*****************************************************************************
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

#include "lexer.h"

extern "C"
{
// Instantiate global variables
extern FILE *yyin;         // input stream
extern FILE *yyout;        // output stream
extern int   yyleng;       // length of current lexeme
extern char *yytext;       // text of current lexeme
extern int   yylex();      // the generated lexical analyzer
}

// Production functions
void E( void );
void T( void );
void F( void );

// Function declarations for checking whether the current token is
// in the first set of each production rule.
bool IsFirstOfE( void );
bool IsFirstOfT( void );
bool IsFirstOfF( void );

// Function to help print the tree nesting
string psp( int );

// Needed global variables
int iTok;                  // The current token

//*****************************************************************************
// The main processing loop
int main( int argc, char* argv[] )
{
  int token;   // hold each token code

  // Set the input and output streams
  yyin = stdin;
  yyout = stdout;

  // Get the first token
  iTok = yylex();
  
  // Fire up the parser!
  try
  {
    if( !IsFirstOfE() ) // Check for ID | INTLIT | (
      throw "first token not 'ID' | 'INTLIT' | '('";

    // Process E Production
    E(); 

    if( iTok != TOK_EOF )
      throw "end of file expected, but there is more here!";
  }
  catch( char const *errmsg )    
  {
    cerr << endl << "***ERROR: " << errmsg << endl;
    return 1;
  }

  cout << endl << "=== GO BULLDOGS! Your parse was successful! ===" << endl;
  return 0;
}

//*****************************************************************************
// E --> T { ( + | - ) T }
void E( void )
{
  static int Ecnt = 0; // Count the number of E's
  int CurEcnt = Ecnt++;
  char const *Terr = 
    "term does not start with 'ID' | 'INTLIT' | '('";

  cout << psp( CurEcnt ) << "enter E " << CurEcnt << endl;

  // We next expect to see a T
  if( IsFirstOfT() )
    T();
  else
    throw Terr;

  // As long as the next token is + or -, keep parsing T's
  while( iTok == TOK_PLUS || iTok == TOK_MINUS )
  {
    cout << "-->found " << yytext << endl;
    iTok = yylex();
    if( IsFirstOfT() )
      T();
    else
      throw Terr;
  }

  cout << psp( CurEcnt ) << "exit E " << CurEcnt << endl;
}

//*****************************************************************************
// T --> F { ( * | / ) F }
void T( void )
{
  static int Tcnt = 0; // Count the number of T's
  int CurTcnt = Tcnt++;
  char const *Ferr = 
    "factor does not start with 'ID' | 'INTLIT' | '('";

  cout << psp( CurTcnt ) << "enter T " << CurTcnt << endl;

  // We next expect to see a F
  if( IsFirstOfF() )
    F();
  else
    throw Ferr;
  
  // As long as the next token is * or /, keep parsing F's
  while( iTok == TOK_MULTIPLY || iTok == TOK_DIVIDE )
  {
    cout << "-->found " << yytext << endl;
    iTok = yylex();
    if( IsFirstOfF() )
      F();
    else
      throw Ferr;
  }

  cout << psp( CurTcnt ) << "exit T " << CurTcnt << endl;
}

//*****************************************************************************
// F --> ID | INTLIT | (
void F( void )
{
  static int Fcnt = 0; // Count the number of F's
  int CurFcnt = Fcnt++;

  cout << psp( CurFcnt ) << "enter F " << CurFcnt << endl;

  // Determine what token we have
  switch( iTok )
  {
  case TOK_IDENTIFIER:
    cout << "-->found ID: " << yytext << endl;
    iTok = yylex(); // Read past what we have found
    break;
    
  case TOK_INTLIT:
    cout << "-->found INTLIT: " << yytext << endl;
    iTok = yylex(); // Read past what we have found
    break;

  case TOK_OPENPAREN:
    // We expect (E); parse it
    cout << "-->found (" << endl;
    iTok = yylex();
    if( !IsFirstOfE() ) // Check for ID | INTLIT | (
      throw "first token not 'ID' | 'INTLIT' | '('";
    E();
    if( iTok == TOK_CLOSEPAREN ) {
      cout << "-->found )" << endl;
      iTok = yylex();
    }
    else
      throw "E does not end with )";
    break;

  default:
    // If we made it to here, syntax error
    throw "factor does not start with 'ID' | 'INTLIT' | '('";
  }

  cout << psp( CurFcnt ) << "exit F " << CurFcnt << endl;
}

//*****************************************************************************
bool IsFirstOfE( void )
{
  return iTok == TOK_IDENTIFIER || iTok == TOK_INTLIT || iTok == TOK_OPENPAREN;
}
//*****************************************************************************
bool IsFirstOfT( void )
{
  return iTok == TOK_IDENTIFIER || iTok == TOK_INTLIT || iTok == TOK_OPENPAREN;
}
//*****************************************************************************
bool IsFirstOfF( void )
{
  return iTok == TOK_IDENTIFIER || iTok == TOK_INTLIT || iTok == TOK_OPENPAREN;
}

//*****************************************************************************
string psp( int n ) // Stands for p-space, but I want the name short
{
  string str( n, ' ' );
  return str;
}
