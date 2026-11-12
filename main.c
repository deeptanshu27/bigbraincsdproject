/*
 * db uhh style?
 * fields:   email, password, isadmin, uhhh purchased id or wtv it was called
 * size  :    [32]     [64]     [1]                   [16]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LENGTH 64

#ifdef _WIN32
#define CLEAR "cls"
#endif
#ifdef linux
#define CLEAR "clear"
#endif

typedef struct Page {
  char* content;
  int (*inputHandler)(char[], int, struct Page*);
  struct Page* pages;
} Page;

Page p_Login, p_Dashboard;

int ih_Welcome(char input[], int inputNum, Page* p_Curr) {
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
    // TODO
    if (0) return 1;
    *p_Curr = p_Dashboard;
    return 0;
  }
  return 0;
}

int main() {
  Page p_Curr;

  p_Login.content = "WELCOME!\nEmail: $Password: $\n";
  p_Login.inputHandler = ih_Welcome;

  p_Dashboard.content = "Hi, ^\n$\n";

  p_Curr = p_Login;

  int error = 0;
  while (1) {
    system(CLEAR);
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
        if (p_Curr.inputHandler(input, inputNum, &p_Curr)) {
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
