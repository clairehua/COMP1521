// COMP1521 18s2 mysh ... command history
// Implements an abstract data object
// Completed by Claire Hua, z5209369, September/October 2018

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// This is defined in string.h
// BUT ONLY if you use -std=gnu99
//extern char *strdup(const char *s);

// Command History
// array of command lines
// each is associated with a sequence number

#define MAXHIST 20
#define MAXSTR  200

#define HISTFILE ".mymysh_history"

typedef struct _history_entry {
   int   seqNumber;
   char *commandLine;
} HistoryEntry;

typedef struct _history_list {
   int nEntries;
   HistoryEntry commands[MAXHIST];
} HistoryList;

HistoryList CommandHistory;

// initCommandHistory()
// - initialise the data structure
// - read from .history if it exists

int initCommandHistory()
{
   char buffer[BUFSIZ];
   buffer[0] = '\0';
   char *fileName = strdup(getenv("HOME"));
   strcat(buffer,fileName);
   strcat(buffer, "/");
   strcat(buffer, HISTFILE);

   int i = 0;
   int cmdNo = 0;
   CommandHistory.nEntries = 0;
   struct stat buf;
   int fileExists = stat(buffer, &buf);
   if (fileExists != 0) {
      return 1; // i.e. current command is the first command
   }
   else {
      FILE *history = fopen(buffer, "r");
      char str[MAXSTR];
      char newline;
      while (fgets(str, MAXSTR, history) != NULL) {
         // breaking down each line into cmdNo and command components
         sscanf(str, " %d  %[^\n]%c", &cmdNo, str, &newline);
         CommandHistory.commands[i].commandLine = strdup(str);
         CommandHistory.commands[i].seqNumber = cmdNo;
         i++;
      }
      fclose(history);
      CommandHistory.nEntries = cmdNo++;
      return cmdNo; // returns num of curr command
   }
}

// addToCommandHistory()
// - add a command line to the history list
// - overwrite oldest entry if buffer is full

void addToCommandHistory(char *cmdLine, int seqNo)
{
   // taking seqNo to be current command number (i.e. one more that the number of commands in history)
   // shift all entries up one and append new one
   if (seqNo > MAXHIST) {
      int i;
      free(CommandHistory.commands[0].commandLine); // freeing oldest command
      for (i = 0; i < MAXHIST - 1; i++) {
         CommandHistory.commands[i].commandLine = strdup(CommandHistory.commands[i + 1].commandLine);
         CommandHistory.commands[i].seqNumber = CommandHistory.commands[i + 1].seqNumber;     
      }
      // i = MAXHIST - 1 here (last element in the array)
      CommandHistory.commands[i].commandLine = strdup(cmdLine);
      CommandHistory.commands[i].seqNumber = seqNo;
   }
   else {
      // seqNo - 1 for the first 20 since indices start at 0
      CommandHistory.commands[seqNo - 1].commandLine = strdup(cmdLine);
      CommandHistory.commands[seqNo - 1].seqNumber = seqNo;
   }
   CommandHistory.nEntries++;      
}

// showCommandHistory()
// - display the list of (at most) 20 most recent commands
void showCommandHistory(FILE *outf)
{
   if (CommandHistory.nEntries == 0) {
      printf("\n");
   }
   else {
      int i = 0;
      int max = MAXHIST;
      if (CommandHistory.nEntries < MAXHIST) { 
         max = CommandHistory.nEntries; 
      }
      while (i < max) {
         printf(" %3d  %s\n", CommandHistory.commands[i].seqNumber, CommandHistory.commands[i].commandLine); 
         i++;
      }  
   } 
}

// getCommandFromHistory()
// - get the command line for specified command
// - returns NULL if no command with this number
char *getCommandFromHistory(int cmdNo)
{
   int i = 0;
   int max = MAXHIST;
   if (CommandHistory.nEntries < MAXHIST) { 
      max = CommandHistory.nEntries; 
   }
   while (i < max) {
      if (CommandHistory.commands[i].seqNumber == cmdNo) {
         return CommandHistory.commands[i].commandLine;
      }
      i++;
   }
   return NULL;
}

// saveCommandHistory()
// - write history to $HOME/.mymysh_history
void saveCommandHistory()
{
   char buffer[BUFSIZ];
   buffer[0] = '\0';
   char *fileName = strdup(getenv("HOME"));
   strcat(buffer,fileName);
   strcat(buffer, "/");
   strcat(buffer, HISTFILE);
   FILE *history = fopen(buffer, "w+");
   
   if (history == NULL) {
      printf("Error opening file! Command history not saved!\n");
      exit(1);
   }
   else {
      int max = MAXHIST;
      if (CommandHistory.nEntries < MAXHIST) {
         max = CommandHistory.nEntries; 
      }
      for (int i = 0; i < max; i++) {
         fprintf(history, " %3d  %s\n", CommandHistory.commands[i].seqNumber,
                                        CommandHistory.commands[i].commandLine);   
      }  
      fclose(history);
   } 
}

// cleanCommandHistory
// - release all data allocated to command history
void cleanCommandHistory()
{
   int max = MAXHIST;
   if (CommandHistory.nEntries < MAXHIST) { 
      max = CommandHistory.nEntries; 
   }
   for (int i = 0; i < max; i++) {
      free(CommandHistory.commands[i].commandLine);
   }   
}
