// mysh.c ... a small shell
// Started by John Shepherd, September 2018
// Completed by Claire Hua, z5209369, September/October 2018

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <glob.h>
#include <assert.h>
#include <fcntl.h>
#include "history.h"

// This is defined in string.h
// BUT ONLY if you use -std=gnu99
//extern char *strdup(char *);

// Function forward references

void trim(char *);
int strContains(char *, char *);
char **tokenise(char *, char *);
char **fileNameExpand(char **);
void freeTokens(char **);
char *findExecutable(char *, char **);
int isExecutable(char *);
void prompt(void);
void historyExecute(char *line, int *cmdNo, char **args, char **envp, char **path);
void shellExecute(char *line, char **args, char **path, char **envp, int *cmdNo);
int isNum(char *cmdNumber);
int cd(char *path);
char *redirectFile(char **args, char *redirectChar);

// Global Constants

#define MAXLINE 200
#define MAXHIST 20
#define HISTFILE ".mymysh_history"


// Global Data

/* none ... unless you want some */


// Main program
// Set up enviroment and then run main loop
// - read command, execute command, repeat

int main(int argc, char *argv[], char *envp[])
{
   char **path; // array of directory names
   int cmdNo;   // command number
   int i;       // generic index

   // set up command PATH from environment variable
   for (i = 0; envp[i] != NULL; i++) {
      if (strncmp(envp[i], "PATH=", 5) == 0) break;
   }
   if (envp[i] == NULL)
      path = tokenise("/bin:/usr/bin",":");
   else
      // &envp[i][5] skips over "PATH=" prefix
      path = tokenise(&envp[i][5],":");
#ifdef DBUG
   for (i = 0; path[i] != NULL;i++)
      printf("path[%d] = %s\n",i,path[i]);
#endif

   // initialise command history
   // - use content of ~/.mymysh_history file if it exists
   cmdNo = initCommandHistory();

   // main loop: print prompt, read line, execute command
   char line[MAXLINE];
   prompt();

   while (fgets(line, MAXLINE, stdin) != NULL) {
      trim(line); // remove leading/trailing space

      if (strcmp(line,"exit") == 0) break;
      if (strcmp(line,"") == 0) { prompt(); continue; }
      char **args = tokenise(line, " ");
      // handling history substitution 
      if (line[0] == '!') { historyExecute(line, &cmdNo, args, envp, path); }
      else {
         shellExecute(line, args, path, envp, &cmdNo);
      }
      prompt();
   }

   saveCommandHistory();
   cleanCommandHistory();
   freeTokens(path);
   printf("\n");

   return(EXIT_SUCCESS);
}

// fileNameExpand: expand any wildcards in command-line args
// - returns a possibly larger set of tokens
char **fileNameExpand(char **tokens)
{
   char line[MAXLINE];
   line[0] = '\0';
   char *wildcardChars = "?*[~";
   glob_t paths;

   int i = 0;
   while (tokens[i] != NULL) {
      // checking for any wildcard chars in each token
      if (strContains(tokens[i], wildcardChars)) {
         if (glob(tokens[i], GLOB_NOCHECK|GLOB_TILDE, 0, &paths) != GLOB_NOMATCH) {
            int j = 0;
            while (j < paths.gl_pathc) {
               // add new path names to original list of tokens
               // if the string is not empty, i.e. not the first argument add ' '
               if (strlen(line) != 0) {
                  strcat(line, " ");
               }
               strcat(line, paths.gl_pathv[j]);
               j++;
            }
         }
         else {
            // from the spec: "If there are no matches, use the token unchanged."
            // if the string is not empty, i.e. not the first argument add ' '
            if (strlen(line) != 0) {
               strcat(line, " ");
            }
            strcat(line, tokens[i]);
         }
      }

      else {
         // if the string is not empty, i.e. not the first argument add ' '
         if (strlen(line) != 0) {
            strcat(line, " ");
         }
         strcat(line, tokens[i]);
      }
      i++;
   }

   char **args = tokenise(line, " ");
   freeTokens(tokens);

   return args;
}

// findExecutable: look for executable in PATH
char *findExecutable(char *cmd, char **path)
{
   char executable[MAXLINE];
   executable[0] = '\0';
   if (cmd[0] == '/' || cmd[0] == '.') {
      strcpy(executable, cmd);
      if (!isExecutable(executable))
         executable[0] = '\0';
   }
   else {
      int i;
      for (i = 0; path[i] != NULL; i++) {
         sprintf(executable, "%s/%s", path[i], cmd);
         if (isExecutable(executable)) break;
      }
      if (path[i] == NULL) executable[0] = '\0';
   }
   if (executable[0] == '\0')
      return NULL;
   else
      return strdup(executable);
}

// isExecutable: check whether this process can execute a file
int isExecutable(char *cmd)
{
   struct stat s;
   // must be accessible
   if (stat(cmd, &s) < 0)
      return 0;
   // must be a regular file
   //if (!(s.st_mode & S_IFREG))
   if (!S_ISREG(s.st_mode))
      return 0;
   // if it's owner executable by us, ok
   if (s.st_uid == getuid() && s.st_mode & S_IXUSR)
      return 1;
   // if it's group executable by us, ok
   if (s.st_gid == getgid() && s.st_mode & S_IXGRP)
      return 1;
   // if it's other executable by us, ok
   if (s.st_mode & S_IXOTH)
      return 1;
   return 0;
}

// tokenise: split a string around a set of separators
// create an array of separate strings
// final array element contains NULL
char **tokenise(char *str, char *sep)
{
   // temp copy of string, because strtok() mangles it
   char *tmp;
   // count tokens
   tmp = strdup(str);
   int n = 0;
   strtok(tmp, sep); n++;
   while (strtok(NULL, sep) != NULL) n++;
   free(tmp);
   // allocate array for argv strings
   char **strings = malloc((n+1)*sizeof(char *));
   assert(strings != NULL);
   // now tokenise and fill array
   tmp = strdup(str);
   char *next; int i = 0;
   next = strtok(tmp, sep);
   strings[i++] = strdup(next);
   while ((next = strtok(NULL,sep)) != NULL)
      strings[i++] = strdup(next);
   strings[i] = NULL;
   free(tmp);
   return strings;
}

// freeTokens: free memory associated with array of tokens
void freeTokens(char **toks)
{
   for (int i = 0; toks[i] != NULL; i++)
      free(toks[i]);
   free(toks);
}

// trim: remove leading/trailing spaces from a string
void trim(char *str)
{
   int first, last;
   first = 0;
   while (isspace(str[first])) first++;
   last  = strlen(str)-1;
   while (isspace(str[last])) last--;
   int i, j = 0;
   for (i = first; i <= last; i++) str[j++] = str[i];
   str[j] = '\0';
}

// strContains: does the first string contain any char from 2nd string?
int strContains(char *str, char *chars)
{
   for (char *s = str; *s != '\0'; s++) {
      for (char *c = chars; *c != '\0'; c++) {
         if (*s == *c) return 1;
      }
   }
   return 0;
}

// prompt: print a shell prompt
// done as a function to allow switching to $PS1
void prompt(void)
{
   printf("mymysh$ ");
}

// historyExecute: handles ! history substitution
// uses getCommandFromHistory(int cmdNo)
void historyExecute(char *line, int *cmdNo, char **args, char **envp, char **path) {
   // execute the previous command (!!)
   if (line[1] == '!') { // this corresponds to how the sample shell works
      if (*cmdNo == 1) { 
         printf("No command #0\n"); 
      }
      else {
         // !! calls for the previous command to be executed, hence cmdNo - 1
         int commandNo = (*cmdNo) - 1;
         char *command = getCommandFromHistory(commandNo);
         if (command == NULL) {
            printf("Invalid history substitution\n");
         }
         else {
            printf("%s\n", command);
            args = tokenise(command, " ");
            shellExecute(command, args, path, envp, cmdNo); 
         } 
      }  
   }

   // execute !? if its exists
   else {
      // determining the command number
      char *cmdNumber = &line[1];
      if (isNum(cmdNumber)) {
         int num = atoi(cmdNumber);
         if (num > *cmdNo) {
            printf("No command #%d\n", num);
         }
         // execute command from history
         else {
            char *command = getCommandFromHistory(num);
            if (command == NULL) {
               printf("Invalid history substitution\n");
            }
            else {
               printf("%s\n", command);
               args = tokenise(command, " ");
               shellExecute(command, args, path, envp, cmdNo); 
            }
         }   
      }
      else { 
         printf("Invalid history substitution\n");
      }
   }
}

// shellExecute: carries out all commands not covered by special cases above
// done as a function to allow implementation of ! history substitutions
void shellExecute(char *line, char **args, char **path, char **envp, int *cmdNo) {
   pid_t pid;   // pid of child process
   int stat;    // return status of child
   int status; 
   char *input = "<";
   char *output = ">";
   int inputRedirect = strContains(line, input);
   int outputRedirect = strContains(line, output);
   args = fileNameExpand(args);

   // handle ! history substitution 
   if ((strcmp(args[0], "h") == 0) || (strcmp(args[0], "history") == 0)) { 
      showCommandHistory(NULL); 
      addToCommandHistory(line, *cmdNo);
      (*cmdNo)++;
   }

   // handle shell cd builtin
   else if (strcmp(args[0], "cd") == 0) {
      int cdReturn = cd(args[1]);
      if (cdReturn == -1) {
         printf("%s: No such file or directory\n", args[1]);
      }
      else {
         char cwd[BUFSIZ];
         getcwd(cwd, BUFSIZ);
         printf("%s\n", cwd); // printing out cwd (as in sample shell)
         addToCommandHistory(line, *cmdNo);
         (*cmdNo)++;
      }
   }

   else {
      pid = fork();
      int saved_stdout;
      saved_stdout = dup(STDOUT_FILENO); // saving stdout to restore to later
      int saved_stdin;
      saved_stdin = dup(STDIN_FILENO); // saving stdin to restore to later
      // child shell
      if (pid == 0) {
         char *command = findExecutable(args[0], path);
         if (command != NULL) {
            // check for and carry out input/output redirections
            if (outputRedirect == 1) {
               char *file = redirectFile(args, output);
               if (file == NULL) { 
                  printf("Invalid i/o redirection\n");
                  kill(getpid(), SIGKILL);
               }
               else {
                  printf("Running %s ...\n--------------------\n", command);
                  // opening file to redirect output to                  
                  int fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR 
                                 | S_IRGRP | S_IWGRP | S_IWUSR);
                  dup2(fd, STDOUT_FILENO);   // make stdout go to file
                  close(fd);
                  status = execve(command, args, envp);
                  // execve only returns on fail
                  fprintf(stderr, "%s: unknown type of executable\n", command); 
                  printf("--------------------\nReturns %u\n", (unsigned char)status);
                  kill(getpid(), SIGKILL);
               }
            }
            else if (inputRedirect == 1) {
               char *file = redirectFile(args, input);
               if (file == NULL) { 
                  printf("Invalid i/o redirection\n");
                  kill(getpid(), SIGKILL);
               }
               else {
                  // opening file to redirect input from
                  int fd = open(file, O_RDWR);
                  if (fd < 0) {
                     fprintf(stderr, "Input redirection: No such file or directory\n");
                     kill(getpid(), SIGKILL);
                  }
                  else {
                     printf("Running %s ...\n--------------------\n", command);
                     // change input from stdin to file
                     dup2(fd, STDIN_FILENO);   
                     close(fd);
                     status = execve(command, args, envp);
                     // execve only returns on fail
                     fprintf(stderr, "%s: unknown type of executable\n", command); 
                     printf("--------------------\nReturns %u\n", (unsigned char)status);
                     kill(getpid(), SIGKILL);
                  }
               }
            }
            // case with no redirections
            else {
               printf("Running %s ...\n--------------------\n", command);
               status = execve(command, args, envp);
               // execve only returns on fail
               fprintf(stderr, "%s: unknown type of executable\n", command); 
               printf("--------------------\nReturns %u\n", (unsigned char)status);
               kill(getpid(), SIGKILL);
            }
         }
         else {
            printf("%s: Command not found\n", args[0]);
            kill(getpid(), SIGKILL);
         }
      }
      // parent shell
      else {
         wait(&stat);
         freeTokens(args);
         // restore stdout and stdin
         dup2(saved_stdout, STDOUT_FILENO);
         close(saved_stdout);
         dup2(saved_stdin, STDIN_FILENO);
         close(saved_stdin);
         // if the child exited normally, print return message
         if (WIFEXITED(stat)) {
            printf("--------------------\nReturns %d\n", WEXITSTATUS(stat));
            addToCommandHistory(line, *cmdNo);
            (*cmdNo)++;
         }
      }
   }     
}


// isNum: checks if string argument is a number
int isNum(char *cmdNumber) {
   int i = 0;
   while (cmdNumber[i] != '\0') {
      if (!(cmdNumber[i] >= '0' && cmdNumber[i] <= '9')) {
         return 0;
      }
      i++;
   }
   return 1;
}

// cd: handles cd command
// returns 0 on success and -1 on error
int cd(char *path) {
   // 'cd' (no other arguments) changes directory to HOME
   if (path == NULL) {
      return chdir(getenv("HOME"));
   }
   else {
      return chdir(path);
   }
}

// redirectFile: finds the file to redirect input/output to/from 
char *redirectFile(char **args, char *redirectChar) {
   int i = 0;
   for (i = 0; strcmp(args[i], redirectChar) != 0; i++);
   // i is now still at the redirection character
   
   // we want to go one more argument through args and return the value 
   // if NULL, it means we have an invalid I/O redirection
   if (args[i + 1] == NULL) {
      return NULL;
   }
   
   char *file = strdup(args[i + 1]);
   
   // clearing input/out redirection char and file name for passing to execve
   // clear and check next 2 indexes in array
   args[i] = NULL; // cleared redirection char
   i++;
   args[i] = NULL; // cleared file name
   i++;
  
   // if we reached here, there is more than one input/output file,
   // which is invalid
   if (args[i] != NULL) {
      return NULL;
   }
   return file;
}
