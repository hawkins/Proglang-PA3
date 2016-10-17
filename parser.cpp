//*****************************************************************************
// purpose: CSE 4713 / 6713 Assignment 3 example recursive descent parser
//  author: J. Edward Swan II
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <map>

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
extern int   yyLine;       // the current source code line
}

// Production functions
void  P( void );
void  A( void );
float E( void );
float T( void );
float F( void );

// Function declarations for checking whether the current token is
// in the first set of each production rule.
bool IsFirstOfP( void );
bool IsFirstOfA( void );
bool IsFirstOfE( void );
bool IsFirstOfT( void );
bool IsFirstOfF( void );

// Function to help print the tree nesting
string psp( int );

// Data type for the Symbol Table
typedef map<string, float> SymbolTableT;

// Needed global variables
int iTok;                  // The current token
SymbolTableT SymbolTable;  // The symbol table


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
    if( !IsFirstOfP() ) // Check for {
      throw "missing '{' at beginning of program";

    // Process P Production
    P(); 

    if( iTok != TOK_EOF )
      throw "end of file expected, but there is more here!";
  }
  catch( char const *errmsg )    
  {
    cout << endl << "***ERROR (line " << yyLine << "): "<< errmsg << endl;
    return 1;
  }

  // Tell the world about our success!!
  cout << endl 
       << "=== GO BULLDOGS! Your parse was successful! ===" 
       << endl << endl;;

  // Print out the symbol table
  SymbolTableT::iterator it;
  for( it = SymbolTable.begin(); it != SymbolTable.end(); ++it ) {
    cout << "symbol = " << it->first << ", value = " << it->second << endl;
  }

  return 0;
}

//*****************************************************************************
// P --> { A \{ A \} }
void P( void )
{
  static int Pcnt = 0; // Count the number of P's
  int CurPcnt = Pcnt++;
  char const *Perr = 
    "assignment statement does not start with 'let'";

  cout << psp( CurPcnt ) << "enter P " << CurPcnt << endl;

  // We know the first token is '{'; talk about it and read next token
  cout << "-->found " << yytext << endl;
  iTok = yylex();

  // We next expect to see an A
  if( IsFirstOfA() )
    A();
  else 
    throw Perr;

  // The A might be followed by a series of A's
  while( IsFirstOfA() )
    A();

  // Last should be a '}'
  if( iTok != TOK_CLOSEBRACE )
    throw "missing '}' at end of program";
  cout << "-->found " << yytext << endl;

  // Read the next token
  iTok = yylex();
    
  cout << psp( CurPcnt ) << "exit P " << CurPcnt << endl;
}

//*****************************************************************************
// A --> let ID := E ; 
void A( void )
{
  float rValue;        // Value returned from expression
  static int Acnt = 0; // Count the number of A's
  int CurAcnt = Acnt++;

  cout << psp( CurAcnt ) << "enter A " << CurAcnt << endl;

  // We know we have found a 'let' token
  cout << "-->found " << yytext << endl;

  // Next should be an identifier; save its name
  iTok = yylex();
  if( iTok != TOK_IDENTIFIER )
    throw "missing identifier in assignment statement";
  cout << "-->found ID: " << yytext << endl;
  string IDname = yytext;

  // Next shoud be an assignment statement
  iTok = yylex();
  if( iTok != TOK_ASSIGN )
    throw "missing ':=' symbol in assignment statement";
  cout << "-->found " << yytext << endl;
  
  // Next should be an expression
  iTok = yylex();
  if( IsFirstOfE() )
    rValue = E();
  else
    throw "missing expression in assignment statement";

  // If the identifier is not yet in the symbol table, store it there
  SymbolTableT::iterator it = SymbolTable.find( IDname );
  //cout << "symbol table says: " << (it == SymbolTable.end()) << endl;
  if( it == SymbolTable.end() ) {
    SymbolTable.insert( pair<string, float>(IDname, 1.0) );
  }

  // Update ID in symbol table with value from expression
  it = SymbolTable.find( IDname );
  it->second = rValue;
  //cout << "key: " << it->first << " val: " << it->second << endl;

  // Last should be a ';' token
  if( iTok != TOK_SEMICOLON )
    throw "missing ';' at end of assignment statement";
  cout << "-->found " << yytext << endl;  

  // Read the next token
  iTok = yylex();

  cout << psp( CurAcnt ) << "exit A " << CurAcnt << endl;
}

//*****************************************************************************
// E --> T { ( + | - ) T }
float E( void )
{
  float rValue1 = 0;   // The value to return
  float rValue2;
  static int Ecnt = 0; // Count the number of E's
  int CurEcnt = Ecnt++;
  char const *Terr = 
    "term does not start with 'ID' | 'INTLIT' | '('";

  cout << psp( CurEcnt ) << "enter E " << CurEcnt << endl;

  // We next expect to see a T
  if( IsFirstOfT() )
    rValue1 = T();
  else
    throw Terr;

  // As long as the next token is + or -, keep parsing T's
  while( iTok == TOK_PLUS || iTok == TOK_MINUS )
  {
    cout << "-->found " << yytext << endl;
    int iTokLast = iTok;
    iTok = yylex();
    if( IsFirstOfT() )
      rValue2 = T();
    else
      throw Terr;

    // Perform the operation to update rValue1 acording to rValue2
    switch( iTokLast )
    {
    case TOK_PLUS:
      rValue1 = rValue1 + rValue2;
      break;

    case TOK_MINUS:
      rValue1 = rValue1 - rValue2;
    }    
  }

  cout << psp( CurEcnt ) << "exit E " << CurEcnt << endl;

  return rValue1;
}

//*****************************************************************************
// T --> F { ( * | / ) F }
float T( void )
{
  float rValue1 = 0;   // The value to return
  float rValue2;
  static int Tcnt = 0; // Count the number of T's
  int CurTcnt = Tcnt++;
  char const *Ferr = 
    "factor does not start with 'ID' | 'INTLIT' | '('";

  cout << psp( CurTcnt ) << "enter T " << CurTcnt << endl;

  // We next expect to see a F
  if( IsFirstOfF() )
    rValue1 = F();
  else
    throw Ferr;
  
  // As long as the next token is * or /, keep parsing F's
  while( iTok == TOK_MULTIPLY || iTok == TOK_DIVIDE )
  {
    cout << "-->found " << yytext << endl;
    int iTokLast = iTok;
    iTok = yylex();
    if( IsFirstOfF() )
      rValue2 = F();
    else
      throw Ferr;

    // Perform the operation to update rValue1 acording to rValue2
    switch( iTokLast )
    {
    case TOK_MULTIPLY:
      rValue1 = rValue1 * rValue2;
      break;

    case TOK_DIVIDE:
      rValue1 = rValue1 / rValue2;
    }    
  }

  cout << psp( CurTcnt ) << "exit T " << CurTcnt << endl;

  return rValue1;
}

//*****************************************************************************
// F --> ID | INTLIT | (
float F( void )
{
  float rValue = 0;           // the value to return
  SymbolTableT::iterator it;  // look up values in symbol table
  static int Fcnt = 0;        // Count the number of F's
  int CurFcnt = Fcnt++;

  cout << psp( CurFcnt ) << "enter F " << CurFcnt << endl;

  // Determine what token we have
  switch( iTok )
  {
  case TOK_IDENTIFIER:
    cout << "-->found ID: " << yytext << endl;

    // Look up value of identifier in symbol table
    it = SymbolTable.find( yytext );
    // If the symbol is not in the table, uninitialized identifier error
    if( it == SymbolTable.end() )
      throw "uninitialized identifier used in expression";
    // Return the value of the identifier
    rValue = it->second;

    // Read past what we have found
    iTok = yylex(); 
    break;
    
  case TOK_INTLIT:
    cout << "-->found INTLIT: " << yytext << endl;

    // Capture the value of this literal
    rValue = (float)atof( yytext );

    iTok = yylex();
    break;

  case TOK_OPENPAREN:
    // We expect (E); parse it
    cout << "-->found (" << endl;
    iTok = yylex();
    rValue = E();
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

  return rValue;
}

//*****************************************************************************
bool IsFirstOfP( void )
{
  return iTok == TOK_OPENBRACE;
}
//*****************************************************************************
bool IsFirstOfA( void )
{
  return iTok == TOK_LET;
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
