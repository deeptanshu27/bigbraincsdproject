/*
 * user_db uhh style?
 * fields:   email, password, isadmin, uhhh purchased id or wtv it was called
 * size  :    [32]     [64]     [1]                   [16]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LENGTH 64
#define MAX_FILE_CONTENT_LENGTH 256
#define DB_EMAIL_LEN 32
#define DB_PASS_LEN 64

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
  char** pageInputs;
  char** pageContent;
} Page;

Page p_Login, p_Dashboard;
char errorMsg[256];

// -1 = not exist
int authUser(char email[], char pass[]) {
  FILE *fptr;
  char input[MAX_FILE_CONTENT_LENGTH];
  fptr = fopen("user_db", "r");
  while (fgets(input, MAX_FILE_CONTENT_LENGTH, fptr)) {
    int cont = 0;
    for (int i = 0; email[i] != '\0' && email[i] != '\n'; i++) {
      if (input[i] != email[i]) {
        cont = 1;
        break;
      } 
    }
    if (cont) {
      continue;
    }
    for (int i = DB_EMAIL_LEN; pass[i - DB_EMAIL_LEN] != '\0' && pass[i - DB_EMAIL_LEN] != '\n'; i++) {
      if (input[i] != pass[i - DB_EMAIL_LEN]) {
        return 0;
      }
    }
    return 1;
  }
  return -1;
}

void createUser(char email[], char pass[]) {
  FILE *fptr = fopen("user_db", "a");
  int i = 0;
  while (email[i] != '\0' && email[i] != '\n') {
    fputc(email[i++], fptr);
  }
  for (; i < DB_EMAIL_LEN; i++) fputc('\0', fptr);
  i = 0;
  while (pass[i] != '\0' && pass[i] != '\n') {
    fputc(pass[i++], fptr);
  }
  for (; i < DB_PASS_LEN; i++) fputc('\0', fptr);
  fputc('\n', fptr);
  fclose(fptr);
}

int ih_Welcome(char input[], int inputNum, Page* p_Curr) {
  if (inputNum == 1) {
    char* str = "@snu.edu.in";
    int i;
    for (i = 0; input[i] != '\n'; i++) {
      if (i < 2) {
        if (!(input[i] <= 'Z' && input[i] >= 'A') && !(input[i] <= 'z' && input[i] >= 'a')) {
          strcpy(errorMsg, "ERROR Ivalid Email");
          return 1;
        }
      } else if (i < 5) {
        if (!(input[i] <= '9' && input[i] >= '0')) {
          strcpy(errorMsg, "ERROR Ivalid Email");
          return 1;
        }
      } else {
        if (input[i] != *str++) {
          strcpy(errorMsg, "ERROR Ivalid Email");
          return 1;
        }
      }
    }
    if (i < 15) {
      strcpy(errorMsg, "ERROR Ivalid Email");
      return 1;
    }
    p_Curr->pageInputs = (char**)malloc(sizeof(char*));
    p_Curr->pageInputs[0] = strdup(input);
  } else if (inputNum == 2) {
    int res;
    if (!(res = authUser(p_Curr->pageInputs[0], input))) {
      strcpy(errorMsg, "ERROR Invalid Email or Password");
      return 1;
    }
    if (res == -1) {
      createUser(p_Curr->pageInputs[0], input);
      strcpy(errorMsg, "New account has been created");
      return 1;
    }
    p_Dashboard.pageContent = (char**)malloc(sizeof(char*));
    p_Dashboard.pageContent[0] = strdup(p_Curr->pageInputs[0]);
    *p_Curr = p_Dashboard;
    return 0;
  }
  return 0;
}

int ih_Dashboard(char input[], int inputNum, Page* p_Curr) {
  return 0;
}

void checkDbExists() {
  FILE* fptr = fopen("user_db", "r");
  if (fptr == NULL) {
    fclose(fptr);
    fptr = fopen("user_db", "w");
    fclose(fptr);
  }
}

int main() {
  checkDbExists();
  Page p_Curr;

  strcpy(errorMsg, "ERROR");

  p_Login.content = "WELCOME!\nEmail: $Password: $\n";
  p_Login.inputHandler = ih_Welcome;

  p_Dashboard.content = "Hi, ^\n$\n";
  p_Dashboard.inputHandler = ih_Dashboard;

  p_Curr = p_Login;

  int error = 0;
  while (1) {
    //system(CLEAR);
    if (error) {
      printf("%s\n", errorMsg);
      error = 0;
    }
    int i = 0, c;
    int inputNum = 1;
    char input[MAX_INPUT_LENGTH];
    while ((c = p_Curr.content[i++]) != '\0') {
      int contentNum = 0;
      if (c == '$') {
        fgets(input, MAX_INPUT_LENGTH, stdin);
        if (p_Curr.inputHandler(input, inputNum, &p_Curr)) {
          error = 1;
          break;
        }
        inputNum++;
      } else if (c == '^') {
        int j = 0;
        for (int j = 0; (c = p_Curr.pageContent[0][j]) != '\0' && (c != '\n'); j++) putchar(c);
        contentNum++;
      } else
        putchar(c);
    }
    if (error) continue;
  }

  return 0;
}
