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

typedef struct User {
  char* email;
  int level;
} User;

typedef struct Page {
  char* content;
  int (*inputHandler)(char[], int, struct Page*, struct User*);
  struct Page* pages;
  char** pageInputs;
  char** pageContent;
} Page;

Page p_Login, p_Dashboard;
char errorMsg[256];


/*
 * dbName -- db we want data from
 * identifiers -- list of items we wanna identify
 * identifierCols -- columns in which these items should be located
 * identifiersLen -- number of stuff we tryna identify
 * colNums -- cols from which we tryna get back data
 * colNumsCount -- number of stuff we tryna collect
 */
char** getData(char dbName[], char** identifiers, int* identifierCols, int identifiersLen, int* colNums, int colNumsCount) {
  char input[128];
  int colSizes[16];
  int colSize = 0, index = 0;
  FILE *fptr;
  char dbNameExt[32];
  strcpy(dbNameExt, dbName);
  strcat(dbNameExt, "_");
  fptr = fopen(dbNameExt, "r");
  
  fgets(input, 128, fptr);
  for (int i = 0; input[i] != '\0' || input[i] != '\n'; i++) {
    if (i != ',' && input[i + 1 != '\0'] && input[i + 1] != '\n') {
      colSizes[index] = colSize;
      colSize = 0;
      index++;
    } else {
      colSize *= 10;
      colSize += i - '0';
    }
  }

  fclose(fptr);
  fptr = fopen(dbName, "r");

  while (fgets(input, 128, fptr)) { // line by line
    int cont = 0;
    for (int i = 0; i < identifiersLen; i++) { // col by col
      int starting = 0;
      for (int j = 0; j < identifierCols[i] - 1; j++) starting += colSizes[j]; // sum col lenghts
      int ending = starting + colSizes[identifierCols[i]];

      for (int j = starting; j <= ending || *identifiers[i] == '\0' || *identifiers[i] == '\n'; j++) { // char by char
        if (input[j] != identifiers[i][j - starting]) {
          cont = 1;
          break;
        }
      }
      if (cont) break;
    }
    if (cont) continue;
    char** res = (char**)malloc(sizeof(char*) * colNumsCount);
    for (int i = 0; i < colNumsCount; i++) { // col by col
      int resLen = 0;
      int starting = 0;
      for (int j = 0; j < colNums[i] - 1; j++) starting += colSizes[j]; // sum col lenghts
      int ending = starting + colSizes[colNums[i]];
    
      for (int j = starting; j <= ending || input[j] == '\0' || input[j] == '\n'; j++) { // char by char
        res[i] = (char*) realloc(res[i], resLen);
        res[i][resLen - 1] = input[j];
        resLen++;
      }
      res[i] = (char*) realloc(res[i], resLen);
      res[i][resLen - 1] = '\0';
    }
    return res;
  }
  return NULL;
}

/*
 * dbName -- database we wanna use
 * cols -- cols we wanna put our data in
 * colNums -- number of cols we wanna put our data in
 * data -- data we wanna put in (like obviously lol)
*/
void setData(char dbName[], char** data, int* cols, int colNums) {
  char input[128];
  int colSizes[16] = { 0 };
  int colSize = 0, index = 0;
  int totalLen = 0;
  FILE *fptr;
  char dbNameExt[32];
  strcpy(dbNameExt, dbName);
  strcat(dbNameExt, "_");
  fptr = fopen(dbNameExt, "r");
  
  fgets(input, 128, fptr);
  for (int i = 0; input[i] != '\0' && input[i] != '\n'; i++) {
    if (input[i] == ',') {
      colSizes[index] = colSize;
      totalLen += colSize;
      colSize = 0;
      index++;
    } else {
      colSize *= 10;
      colSize += input[i] - '0';
    }
  }
  colSizes[index] = colSize;
  totalLen += colSize;

  fclose(fptr);
  fptr = fopen(dbName, "a");

  char* res = (char*) malloc(totalLen);
  for (int i = 0; i < totalLen; i++) {
    res[i] = '\0';
  }
  for (int i = 0; i < colNums; i++) { // col by col
    int starting = 0;
    for (int j = 0; j < cols[i]; j++) starting += colSizes[j];
    int ending = starting + colSizes[cols[i]];

    for (int j = starting; j < ending || data[i][j - starting] == '\0' || data[i][j - starting] == '\n'; j++) {
      res[j] = data[i][j - starting];
    }
  }

    printf("b: %s\n", res);
  for (int i = 0; i < totalLen; i++) {
    fputc(res[i], fptr);
  }
}


/*
 * -1 = not exist
 *  0 = regular user
 *  1 = auditorium sec
 *  2 = society president
 *  3 = show manager
 *  4 = sales person (assignable)
 *  5 = accounts clerk
 */
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
    return 1 + input[DB_EMAIL_LEN + DB_PASS_LEN] - '0';
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
  fputc('0', fptr); // 0 for not_admin
  fputc('\n', fptr);
  fclose(fptr);
}

int ih_Welcome(char input[], int inputNum, Page* p_Curr, User* user) {
  if (inputNum == 1) {
    char* str = "@snu.edu.in";
    int i;
    for (i = 0; input[i] != '\n'; i++) {
      if (i < 2) {
        if (!(input[i] <= 'Z' && input[i] >= 'A') && !(input[i] <= 'z' && input[i] >= 'a')) {
          strcpy(errorMsg, "ERROR Invalid Email");
          return 1;
        }
      } else if (i < 5) {
        if (!(input[i] <= '9' && input[i] >= '0')) {
          strcpy(errorMsg, "ERROR Invalid Email");
          return 1;
        }
      } else {
        if (input[i] != *str++) {
          strcpy(errorMsg, "ERROR Invalid Email");
          return 1;
        }
      }
    }
    if (i < 15) {
      strcpy(errorMsg, "ERROR Invalid Email");
      return 1;
    }
    user->email = strdup(input);
  } else if (inputNum == 2) {
    int res;
    if (!(res = authUser(user->email, input))) {
      strcpy(errorMsg, "ERROR Invalid Email or Password");
      return 1;
    }
    if (res == -1) {
      //createUser(user->email, input);
      setData("user_db", (char*[MAX_INPUT_LENGTH]){ user->email, input }, (int[2]){ 0, 1 }, 2);
      strcpy(errorMsg, "New account has been created");
      return 1;
    }
    p_Dashboard.pageContent = (char**)malloc(sizeof(char*) * 2);
    p_Dashboard.pageContent[0] = strdup(user->email);
    user->level = res - 1;
    *p_Curr = p_Dashboard;
    return 0;
  }
  return 0;
}

int ih_Dashboard(char input[], int inputNum, Page* p_Curr, User* user) {
  return 0;
}

void checkDbExists(char* name) {
  FILE* fptr = fopen(name, "r");
  if (fptr == NULL) {
    fclose(fptr);
    fptr = fopen(name, "w");
    fclose(fptr);
  }
}

int main() {
  checkDbExists("user_db");
  Page p_Curr;
  User user;

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
        if (p_Curr.inputHandler(input, inputNum, &p_Curr, &user)) {
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
