#include <stdio.h>

void one(void) {
  printf("In one...\n");
}

void two(void) {
  printf("In two...\n");
}

int main(int argc, char** argv) {

  printf("Bajja...\n");
  int x = 2;

  if (x == 2) {
    two();
  } else {
    one();
  }

  return 0;
}

