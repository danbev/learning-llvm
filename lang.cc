#include <string>
#include <cctype>
#include <cstdio>
#include "lang.h"

// This global will hold the one of the elements in Token or it will be an
// unknown character.
static std::string IdentifierStr;
// If the current token is a numeric value NumVal will hold that value.
static double NumVal;

// Lexer implementation
int gettok() {
  static int LastChar = ' ';

  while (isspace(LastChar)) {
    LastChar = getchar();
    printf("LastChar: %d\n", LastChar);
  }
  if (isalpha(LastChar)) {
    IdentifierStr = LastChar;  // note that IdentifierStr is of type std::string
    while (isalnum((LastChar = getchar()))) {
      IdentifierStr += LastChar;
    }
    if (IdentifierStr == "def") {
      return tok_def;
    }
    if (IdentifierStr == "extern") {
      return tok_extern;
    }
    return tok_identifier;
  }


  return 0;
}
