//*****************************************************************************
// purpose: CSE 4713 / 6713 Assignment 3 example recursive descent parser
//  author: J. Edward Swan II
//  editor: Josh Hawkins
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
void  P ( void );
void  S ( void );
void  A ( void );
float E ( void );
float B ( void ); // TODO: Is this a boolean? Not float? What?
float R ( void ); // TODO: This is arithmetic
float T ( void );
float F ( void );
float U ( void );
void  G ( void );
void  O ( void );
void  C ( void );
void  W ( void );

// Function declarations for checking whether the current token is
// in the first set of each production rule.
bool IsFirstOfP ( void );
bool IsFirstOfS ( void );
bool IsFirstOfA ( void );
bool IsFirstOfE ( void );
bool IsFirstOfB ( void );
bool IsFirstOfR ( void );
bool IsFirstOfT ( void );
bool IsFirstOfF ( void );
bool IsFirstOfU ( void );
bool IsFirstOfG ( void );
bool IsFirstOfO ( void );
bool IsFirstOfC ( void );
bool IsFirstOfW ( void );

// Function to help print the tree nesting
string psp ( int );

// Data type for the Symbol Table
typedef map<string, float> SymbolTableT;

// Needed global variables
int iTok;                  // The current token
SymbolTableT SymbolTable;  // The symbol table


//*****************************************************************************
// The main processing loop
int main ( int argc, char* argv[] )
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
// P --> \{ {S} \}
void P ( void )
{
  static int Pcnt = 0; // Count the number of P's
  int CurPcnt = Pcnt++;

  cout << psp( CurPcnt ) << "enter P " << CurPcnt << endl;

  // We know the first token is '{'; talk about it and read next token
  cout << "-->found " << yytext << endl;
  iTok = yylex();

  // There might be a series of S's
  while( IsFirstOfS() )
    S();

  // Last should be a '}'
  if( iTok != TOK_CLOSEBRACE )
    throw "missing '}' at end of program";
  cout << "-->found " << yytext << endl;

  // Read the next token
  iTok = yylex();

  cout << psp( CurPcnt ) << "exit P " << CurPcnt << endl;
}

//*****************************************************************************
// S --> A | G | O | C | W
void S ( void )
{
  static int Scnt = 0; // Count the number of P's
  int CurScnt = Scnt++;
  char const *Serr =
    "statement does not start with 'let', 'read', 'print', if', or 'while'";

  cout << psp( CurScnt ) << "enter S " << CurScnt << endl;

  // We know we have found a beginning token, but which?
  switch( iTok ) {
    case TOK_LET:
      A();
      break;

    case TOK_READ:
      G();
      break;

    case TOK_PRINT:
      O();
      break;

    case TOK_IF:
      C();
      break;

    case TOK_WHILE:
      W();
      break;

    default:
      // If we made it to here, syntax error
      throw Serr;
  }

  cout << psp( CurScnt ) << "exit S " << CurScnt << endl;
}


//*****************************************************************************
// A --> let ID := E ;
void A ( void )
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
  // iTok = yylex(); // TODO: remove?
  if( iTok != TOK_SEMICOLON )
    throw "missing ';' at end of assignment statement";
  cout << "-->found " << yytext << endl;

  // Read the next token
  iTok = yylex();

  cout << psp( CurAcnt ) << "exit A " << CurAcnt << endl;
}

//*****************************************************************************
// E --> B {( and | or ) B }
float E ( void )
{
  float rValue = 0;   // The value to return
  static int Ecnt = 0; // Count the number of E's
  int CurEcnt = Ecnt++;
  char const *Berr =
    "expression does not start with 'not' | '-' | 'ID' | 'FLOATLIT' | '('";

  cout << psp( CurEcnt ) << "enter E " << CurEcnt << endl;

  // We next expect to see a B
  if( IsFirstOfB() )
    rValue = B();
  else
    throw Berr;

  // As long as the next token is and or or, keep parsing B's
  while( iTok == TOK_AND || iTok == TOK_OR )
  {
    cout << "-->found " << yytext << endl;
    int iTokLast = iTok;
    iTok = yylex();
    if( IsFirstOfB() )
      B();
    else
      throw Berr;
  }

  cout << psp( CurEcnt ) << "exit E " << CurEcnt << endl;

  return rValue;
}

//*****************************************************************************
// B --> R [( < | > | == ) R ]
float B ( void )
{
  float rValue = 0;
  static int Bcnt = 0;
  int CurBcnt = Bcnt++;
  char const *Rerr =
    "term does not start with 'not' | '-' | 'ID' | 'FLOATLIT' | '('";

  cout << psp( CurBcnt ) << "enter B " << CurBcnt << endl;

  // We next expect to see an R
  if( IsFirstOfR() )
    rValue = R();
  else
    throw Rerr;

  // We may find one more R
  if( iTok == TOK_LESSTHAN || iTok == TOK_GREATERTHAN || iTok == TOK_EQUALTO )
  {
    cout << "-->found " << yytext << endl;
    int iTokLast = iTok;
    iTok = yylex();
    if( IsFirstOfR() )
      R();
    else
      throw Rerr;
  }

  // In case this was not used as a boolean expression so return whatever we found
  cout << psp( CurBcnt ) << "exit B " << CurBcnt << endl;
  return rValue;
}

//*****************************************************************************
// R --> T {( + | - ) T }
float R ( void )
{
  float rValue1 = 0;   // The value to return
  float rValue2;
  static int Rcnt = 0; // Count the number of R's
  int CurRcnt = Rcnt++;
  char const *Terr =
    "term does not start with 'not' | '-' | 'ID' | 'FLOATLIT' | '('";

  cout << psp( CurRcnt ) << "enter R " << CurRcnt << endl;

  // We next expect to see a T
  if( IsFirstOfT() )
    rValue1 = T();
  else
    throw Terr;

  // We may see more T's
  while( iTok == TOK_PLUS || iTok == TOK_MINUS )
  {
    cout << "-->found " << yytext << endl;
    int iTokLast = iTok;
    iTok = yylex();
    if( IsFirstOfT() )
      rValue2 = T();
    else
      throw Terr;

    // Perform the operation to update rValue1 according to rValue2
    switch( iTokLast )
    {
      case TOK_PLUS:
        rValue1 = rValue1 + rValue2;
        break;

      case TOK_MINUS:
        rValue1 = rValue1 - rValue2;
        break;
    }
  }

  cout << psp( CurRcnt ) << "exit R " << CurRcnt << endl;

  return rValue1;
}

//*****************************************************************************
// T --> F { ( * | / ) F }
float T ( void )
{
  float rValue1 = 0;   // The value to return
  float rValue2;
  static int Tcnt = 0; // Count the number of T's
  int CurTcnt = Tcnt++;
  char const *Ferr =
    "factor does not start with 'ID' | 'FLOATLIT' | '('";

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
// F --> [ not | - ] U
float F ( void ) {
  float rValue = 0;
  // TODO: Replace all rValue1 with rValue where there is no rValue2
  static int Fcnt = 0;
  int CurFcnt = Fcnt++;

  cout << psp( CurFcnt ) << "enter F " << CurFcnt << endl;

  // We may have a NOT or a MINUS
  if( iTok == TOK_NOT || iTok == TOK_MINUS )
  {
    cout << "-->found " << yytext << endl;
    iTok = yylex();
  }

  // Next should be U
  if( IsFirstOfU() )
    rValue = U();
  else
    throw "missing 'U' at end of 'F'";

  cout << psp( CurFcnt ) << "exit F " << CurFcnt << endl;

  return rValue;
}

//*****************************************************************************
// U --> ID | FLOATLIT | (E)
float U ( void )
{
  float rValue = 0;           // the value to return
  SymbolTableT::iterator it;  // look up values in symbol table
  static int Ucnt = 0;        // Count the number of F's
  int CurUcnt = Ucnt++;

  cout << psp( CurUcnt ) << "enter U " << CurUcnt << endl;

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

  case TOK_FLOATLIT:
    cout << "-->found FLOATLIT: " << yytext << endl;

    // Capture the value of this literal
    rValue = (float)atof( yytext );

    iTok = yylex();
    break;

  case TOK_OPENPAREN:
    // We expect (E); parse it
    cout << "-->found (" << endl;
    iTok = yylex();
    rValue = E(); // TODO: Improve this with isfirstofe
    if( iTok == TOK_CLOSEPAREN ) {
      cout << "-->found )" << endl;
      iTok = yylex();
    }
    else
      throw "E does not end with )";
    break;

  default:
    // If we made it to here, syntax error
    throw "factor does not start with 'ID' | 'FLOATLIT' | '('";
  }

  cout << psp( CurUcnt ) << "exit U " << CurUcnt << endl;

  return rValue;
}

//*****************************************************************************
// G --> read [ STRINGLIT ] ID;
void G ( void )
{
  static int Gcnt = 0;        // Count the number of F's
  int CurGcnt = Gcnt++;

  cout << psp( CurGcnt ) << "enter G " << CurGcnt << endl;

  // We've already seen the read
  cout << "--> found " << yytext << endl;

  // A string literal may be next
  iTok = yylex();
  if( iTok == TOK_STRINGLIT )
  {
    cout << "-->found string: " << yytext << endl;
    iTok = yylex();
  }

  // Next should be ID
  if( iTok == TOK_IDENTIFIER )
  {
    cout << "-->found ID: " << yytext << endl;
    // If the identifier is not yet in the symbol table, store it there
    SymbolTableT::iterator it = SymbolTable.find( yytext );
    if( it == SymbolTable.end() ) {
      SymbolTable.insert( pair<string, float>(yytext, 1.0) );
    }
  }
  else
  {
    throw "missing 'ID' at end of read statement";
  }

  // Next should be ;
  iTok = yylex();
  if( iTok == TOK_SEMICOLON )
    cout << "-->found: " << yytext << endl;
  else
    throw "missing ';' at end of read statement";

  iTok = yylex();
  cout << psp( CurGcnt ) << "exit G " << CurGcnt << endl;
}

//*****************************************************************************
// O --> print [ STRINGLIT ] [ ID ];
void O ( void )
{
  static int Ocnt = 0;        // Count the number of F's
  int CurOcnt = Ocnt++;

  cout << psp( CurOcnt ) << "enter O " << CurOcnt << endl;

  // We've already seen the read
  cout << "-->found " << yytext << endl;

  // A string literal may be next
  iTok = yylex();
  if( iTok == TOK_STRINGLIT )
  {
    cout << "-->found string: " << yytext << endl;
    iTok = yylex();
  }

  // Is next ID?
  if( iTok == TOK_IDENTIFIER )
  {
    cout << "-->found ID: " << yytext << endl;
    iTok = yylex();
  }

  // Next should be ;
  if( iTok == TOK_SEMICOLON )
    cout << "-->found " << yytext << endl;
  else
    throw "missing ';' at end of print statement!";

  // Read next token
  iTok = yylex();

  cout << psp( CurOcnt ) << "exit O " << CurOcnt << endl;
  return;
}

//*****************************************************************************
// C --> if (E) P [ else P ]
void C ( void )
{
  static int Ccnt = 0;        // Count the number of F's
  int CurCcnt = Ccnt++;

  cout << psp( CurCcnt ) << "enter C " << CurCcnt << endl;

  // We've already seen the if
  cout << "-->found " << yytext << endl;

  // A ( should be next
  iTok = yylex();
  if( iTok == TOK_OPENPAREN )
  {
    cout << "-->found " << yytext << endl;
    iTok = yylex();
  }
  else
  {
    throw "missing ( after 'if' in if statement!";
  }

  // We should see an E next
  if( IsFirstOfE() )
    E();
  else
    throw "missing expression after '(' in if statement!";

  // We should see ) next
  if( iTok == TOK_CLOSEPAREN )
  {
    cout << "-->found " << yytext << endl;
    iTok = yylex();
  }
  else
  {
    throw "missing ')' after expression in if statement!";
  }

  // We should see a P next
  if( IsFirstOfP() )
    P();
  else
    throw "missing 'P' after if clause!";

  // We may find else next
  if( iTok == TOK_ELSE )
  {
      cout << "-->found " << yytext << endl;
      iTok = yylex();

      // We should find a P next
      if( IsFirstOfP() )
        P();
      else
        throw "missing 'P' after else!";
  }

  cout << psp( CurCcnt ) << "exit C " << CurCcnt << endl;
}

//*****************************************************************************
// W --> while (E) P
void W ( void )
{
  static int Wcnt = 0;        // Count the number of F's
  int CurWcnt = Wcnt++;

  cout << psp( CurWcnt ) << "enter W " << CurWcnt << endl;

  // We've already seen the while
  cout << "-->found " << yytext << endl;

  // A ( should be next
  iTok = yylex();
  if( iTok == TOK_OPENPAREN )
  {
    cout << "-->found " << yytext << endl;
    iTok = yylex();
  }
  else
  {
    throw "missing ( after 'while' in while!";
  }

  // We should see an E next
  if( IsFirstOfE() )
    E();
  else
    throw "missing expression after '(' in while!";

  // We should see ) next
  if( iTok == TOK_CLOSEPAREN )
  {
    cout << "-->found " << yytext << endl;
    iTok = yylex();
  }
  else
  {
    throw "missing ')' after expression in while!";
  }

  // We should see a P next
  if( IsFirstOfP() )
    P();
  else
    throw "missing 'P' after while clause!";

  cout << psp( CurWcnt ) << "exit W " << CurWcnt << endl;
}

//*****************************************************************************
// IsFirstOfX Functions
//*****************************************************************************
bool IsFirstOfP( void )
{
  return iTok == TOK_OPENBRACE;
}
//*****************************************************************************
bool IsFirstOfS( void )
{
  return iTok == TOK_LET || iTok == TOK_READ || iTok == TOK_PRINT || iTok == TOK_IF || iTok == TOK_WHILE;
}
//*****************************************************************************
bool ISFirstOfA( void)
{
  return iTok == TOK_LET;
}
//*****************************************************************************
bool IsFirstOfE( void )
{
  return iTok == TOK_NOT || iTok == TOK_MINUS || iTok == TOK_OPENPAREN || iTok == TOK_IDENTIFIER || iTok == TOK_FLOATLIT;
}
//*****************************************************************************
bool IsFirstOfB( void )
{
  // Same as E
  return IsFirstOfE();
}
//*****************************************************************************
bool IsFirstOfR( void )
{
  // Same as E
  return IsFirstOfE();
}
//*****************************************************************************
bool IsFirstOfT( void )
{
  // Same as E
  return IsFirstOfE();
}
//*****************************************************************************
bool IsFirstOfF( void )
{
  // Same as E
  return IsFirstOfE();
}
//*****************************************************************************
bool IsFirstOfU( void )
{
  return iTok == TOK_OPENPAREN || iTok == TOK_IDENTIFIER || iTok == TOK_FLOATLIT;
}
//*****************************************************************************
bool IsFirstOfG( void )
{
  return iTok == TOK_READ;
}
//*****************************************************************************
bool IsFirstOfO( void )
{
  return iTok == TOK_PRINT;
}
//*****************************************************************************
bool IsFirstOfC( void )
{
  return iTok == TOK_IF;
}
//*****************************************************************************
bool IsFirstOfW( void )
{
  return iTok == TOK_WHILE;
}
//*****************************************************************************
// Helper Functions
//*****************************************************************************
string psp( int n ) // Stands for p-space, but I want the name short
{
  string str( n, ' ' );
  return str;
}
