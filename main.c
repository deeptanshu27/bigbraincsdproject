#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT_LENGTH 256

#ifdef _WIN32
#define CLEAR "cls"
#endif
#ifdef linux
#define CLEAR "clear"
#endif

typedef struct {
  char* content;
  int (*inputHandler)(char[], int);
} Page;

int ih_Welcome(char input[], int inputNum) {
  if (inputNum == 1) {
    // check if is valid student email
    char* str = "@snu.edu.in";
    for (int i = 0; input[i] != '\n'; i++) {
      if (i < 2) {
        if (!(input[i] <= 'Z' && input[i] >= 'A') && !(input[i] <= 'z' && input[i] >= 'a'))
          return 1;
      } else if (i < 5) {
        if (!(input[i] <= '9' && input[i] >= '0'))
          return 1;
      } else {
        if (input[i] != *str++)
          return 1;
      }
    }
  } else if (inputNum == 2) {
    printf("is password\n");
  }
  return 0;
}

int main() {
  Page p_Login, p_Curr;

  p_Login.content = "WELCOME!\nEmail: %\nPassword: %\n";
  p_Login.inputHandler = ih_Welcome;

  p_Curr = p_Login;

  int error = 0;
  while (1) {
    //system(CLEAR);
    if (error) {
      printf("ERROR! Make sure to enter the correct input!\n");
      error = 0;
    }
    int i = 0, c;
    int inputNum = 1;
    char input[MAX_INPUT_LENGTH];
    while ((c = p_Curr.content[i++]) != '\0') {
      if (c == '%') {
        fgets(input, MAX_INPUT_LENGTH, stdin);
        if (p_Curr.inputHandler(input, inputNum)) {
          error = 1;
          break;
        }
        inputNum++;
      } else
        putchar(c);
    }
    if (error) continue;
  }

  return 0;
}
