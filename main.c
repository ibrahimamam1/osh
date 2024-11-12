#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE 80 /* The maximum length command */
#define PERM 0666
#define MAXHIST 10
#define MEMSIZE MAXHIST * MAXLINE + sizeof(int)

typedef struct{
  unsigned int count;
  char entries[MAXHIST][MAXLINE];
}History;

int parse_input(char *str, char *dest[]) {
  int token_count = 0;

  int i = 0;
  while (str[i]) {

    // Skip multiple spaces
    while (str[i] == ' ') {
      i++;
    }
    if (!str[i])
      break;

    char current_token[MAXLINE] = {0};
    size_t pos = 0;
    while (str[i] && str[i] != ' ') {
      current_token[pos++] = str[i++];
    }
    current_token[pos] = '\0';

    dest[token_count] = malloc(pos + 2);
    strcpy(dest[token_count++], current_token);
  }

  return token_count;
}

int main(void) {
  char *args[MAXLINE / 2 + 1]; /* command line arguments */
  int should_run = 1;          /* flag to determine when to exit program */

  // establish shared memory area for storing recent commands
  int shm_fd = shm_open("history", O_CREAT | O_RDWR, PERM);
  ftruncate(shm_fd, MEMSIZE);
  void *shared_mem =
      mmap(NULL, MEMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  
  //initialise history structure
  History *hist = (History*)shared_mem;
  memset(hist, 0, MEMSIZE);

  while (should_run) {
    printf("\nosh>");
    fflush(stdout);

    // read the user's input
    char input[MAXLINE];
    fgets(input, MAXLINE, stdin);
    input[strcspn(input, "\n")] = 0; // remove the trailing new line character

    // parse input string
    int arg_count = parse_input(input, args);
    args[arg_count] = NULL; // last argument should be null for execvp

    if (arg_count == 0)
      continue;
    
    //check osh_specific commands
    if (strcmp(args[0], "exit") == 0) {
      exit(1);
    }
    if (strcmp(args[0], "!!") == 0) {
      //execute latest command in history
      if(hist->count < 1){
        printf("No Command in history\n");
      }
      else{
        //clear arguments and parse last command in history
        memset(args, 0, arg_count * (MAXLINE / 2 + 1));
        strcpy(input, hist->entries[hist->count-1]);
        arg_count = parse_input(input, args);
      }
    }
    else if (strcmp(args[0], "!") == 0) {
      // ! 5 should execute 5th most recent  command 
      int n = atoi(args[1]);

      if(n < 1 || hist->count < n){
        printf("No such Command in history\n");
      }
      else{
        //clear arguments and parse last command in history
        memset(args, 0, arg_count * (MAXLINE / 2 + 1));
        strcpy(input, hist->entries[hist->count-1]);
        arg_count = parse_input(input, args);
      }
    } 
    else if (strcmp(args[0], "history") == 0) {
      if (hist->count == 0)
        printf("No Commands in history\n");
      else {
        for (int i = hist->count; i >= 1; i--) {
          printf("%d- %s\n", hist->count-i+1, hist->entries[i - 1]);
        }
      }
      continue;
    } 
    
    //end of osh_specifi commands. execute linux commands

      // should process run in background??
      int background = 0;
      if (strcmp(args[arg_count - 1], "&") == 0) {
        background = 1;
        args[arg_count - 1] = NULL;
      }

      // fork a child process to execute command
      int pid = fork();
      if (pid == 0) {
        // child process
        strcpy(hist->entries[hist->count++], input); // add command to history
        execvp(args[0], args);
      } else {
        // parent process
        if (background) {
          wait(NULL);
        }
      }
    }
  return 0;
}
