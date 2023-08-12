/*********************************************************************
   Program  : miniShell                   Version    : 1.3
 --------------------------------------------------------------------
   skeleton code for linix/unix/minix command line interpreter
 --------------------------------------------------------------------
   File			: minishell.c
   Compiler/System	: gcc/linux

********************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>


#define NV 20			/* max number of command tokens */
#define NL 100			/* input buffer size */
#define BG 50     /* max number of background processes*/
char            line[NL];	/* command input buffer */
char lineCpy[NL];

// struct holding process ID and associated command of a background process
typedef struct {
    pid_t pid;
    char command[NL];
} BGProcess;

// Intialising storage of background process
BGProcess BGProcesses[BG]; 
int numBG = 0;

// Handles when a signal of a child process being terminated
void sigChildHandler(int signum) {
  int status;
  pid_t pid;
  // Checks for a child being terminated without stop execution of other code
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    //print any BG process that have been finished
    for (int i = 0; i < numBG; i++) {
      if (BGProcesses[i].pid == pid) {
        printf("[%d]+ Done %s\n", i + 1, BGProcesses[i].command);
      }
    }
  }
}



/*
	shell prompt
 */

void prompt(void){
  //fprintf(stdout, "\n msh> ");
  fflush(stdout);
}

int main(int argk, char *argv[], char *envp[])
/* argk - number of arguments */
/* argv - argument vector from command line */
/* envp - environment pointer */

{
  int             frkRtnVal;	/* value returned by fork sys call */
  int             wpid;		/* value returned by wait */
  char           *v[NV];	/* array of pointers to command line tokens */
  char           *sep = " \t\n";/* command line token separators    */
  int             i;		/* parse index */
  int background = 0;

  signal(SIGCHLD, sigChildHandler);


  /* prompt for and process one command line at a time  */

  while (1) {			/* do Forever */
    prompt();
    fgets(line, NL, stdin);
    strcpy(lineCpy, line);
    fflush(stdin);

    

    if (feof(stdin)) {		/* non-zero on EOF  */

      //fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(),
	      //feof(stdin), ferror(stdin));
      exit(0);
    }
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
      continue;			/* to prompt */

    // Tokenizing input and store keywords in an array 
    v[0] = strtok(line, sep);
    int size = 0;
    for (i = 1; i < NV; i++) {
      v[i] = strtok(NULL, sep);
      size = i;
      if (v[i] == NULL){
	      break;
      }
    }

    // Check if input ends with '&' and classify if a BG process
    if (strcmp("&", v[size - 1]) != 0) {
      background = 0;
    } else {
      v[size - 1] = NULL;
      background = 1;
    }

    // Check if a change directory command is received and changes to directory if it exists
    if (strcmp("cd", v[0]) == 0) {
      if (chdir(v[1]) < 0) {
        perror("Error");
        }
      continue;
    }

    /* assert i is number of tokens + 1 */

    /* fork a child process to exec the command in v[0] */

    switch (frkRtnVal = fork()) {
    case -1:			/* fork returns error to parent process */
      {
        perror("Error");
	break;
      }
    case 0:			/* code executed only by child process */
      {
        // Returns error if execution fails
        if (execvp(v[0], v) < 0) {
          perror("Error");
          exit(1);
        }	
      }
    default:			/* code executed only by parent process */
      {
        // Checks if the command is not a BG process
        if (!background) {
          // Parent waits for child to terminate
          wpid = wait(0);
          if (wpid < 0 && errno != ECHILD) {
            perror("Error");
          }
        } else {
          // Stores the information of the background process
          BGProcesses[numBG].pid = frkRtnVal;
          int length = (int)strlen(lineCpy);

          // Cleans the input command to expected output format
          for (int i = length-1; i > -1; i--){
            if (lineCpy[i] == '\n'){
              lineCpy[i] = '\0';
            }
            if (lineCpy[i] == '&'){
              lineCpy[i] = '\0';
            }
          }
          
          strncpy(BGProcesses[numBG].command, lineCpy, NL);
          numBG++;
        }
      }				
    }				/* switch */
  }				/* while */
  return 0;
}       /* main */
