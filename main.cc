#include "lang.h"

#include <cstdlib>
#include <cstdio>

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage main <source_file>\n");
    exit(EXIT_FAILURE);
  }
  char* source = argv[1];
  printf("Going to parse source '%s'\n", source);
  int token = gettok();
  printf("got token %d\n", token);
  exit(EXIT_SUCCESS);
}
