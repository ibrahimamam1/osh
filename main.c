#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXLINE 80 /* The maximum length command */

int parse_input(char *str, char *dest[]) {
  int token_count = 0;

  int i = 0;
  while (str[i]) {

    // Skip multiple spaces
    while (str[i] == ' ') {
      i++;
    }
    if(!str[i])break;

    char current_token[MAXLINE] = {0};
    size_t pos = 0;
    while(str[i] && str[i] != ' ') {
      current_token[pos++] = str[i++];
    }
    current_token[pos] = '\0';
    
    dest[token_count] = malloc(pos+2);
    strcpy(dest[token_count++], current_token);
  }

  return token_count;
}

int main(void) {
  char *args[MAXLINE / 2 + 1];/* command line arguments */
  int should_run = 1;          /* flag to determine when to exit program */
  while (should_run) {
    printf("\nosh>");
    fflush(stdout);

    // read the user's input
    char input[MAXLINE];
    fgets(input, MAXLINE, stdin);
    input[strcspn(input, "\n")] = 0; // remove the trailing new line character

    // parse input string
    int arg_count = parse_input(input, args);
    args[arg_count] = NULL; //last argument should be null for execvp

    if(arg_count == 0) continue;

    if(strcmp(args[0] , "exit") == 0){
      exit(1);
    }
    
    //should process run in background??
    int background = 0;
    if(strcmp(args[arg_count-1], "&") == 0){
      background = 1;
      args[arg_count-1] = NULL;
    }
 
    //fork a child process to execute command
    int pid = fork();
    if(pid == 0){
      //child process
      execvp(args[0], args);
    }
    else{
      //parent process
      if(background) {
        wait(NULL);
      }
    }
  }
  return 0;
}
