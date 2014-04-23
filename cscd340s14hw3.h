#pragma once

#ifndef CSCD340S14HW3_H
#define CSCD340S14HW3_H

#include "linkedlist/linkedlist.h"
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define HISTORY_FILE ".ssh_history"

/*
	Loads the history of the shell from a file. 
	If no file is present, then nothing is loaded. 
*/
List * load_history(char * filename);

/*
	Saves the history of the shell to a file. 
	If the file doesn't exist, it will be created in 
	the present working directory at time of exit. 
*/
int save_history(List * history, char * filename);

/*
	Remove a trailing carriage return from the string. 
*/
void strip(char * buf);


/*
	Read input from stdin dispatch appropriately.
	If the input is an internal command, the 
	command is dealt with accordingly.
	If the input is not a recognized internal
	command, the command is assumed to be an external 
	program.
*/
int interp(List * history); 

/*
	List the shell history in oldest -> newest
	order. 
	No more than 200 of the last commands are shown.
*/
void show_history(List* history);


/*
	Executes an external command. 
	Determines whether 'command'
	contains a pipe, redirect, or is just
	a plain ol' command.
*/
int execute(const char * command);

/*
	Split s into a 2D array of strings using the 
	delim specified by 'delim'.
	Argc is updated to reflect the number of arguments
	parsed. 
	A 2D char array is returned.
	The returned array must be cleaned up with a call 
	to freeargs. 
*/
char ** makeargs(const char * s, const char * delim, int * argc);


/*
	Free a 2D array of strings.
	Takes a triple pointer because the passed in 
	argument will be set to NULL to avoid double frees.
*/
void freeargs(char *** args);


/*
	Determine whether a redirect is present in 's'.
	Doesn't actually execute a command.
	'command' is set to the argument on the left of the redirect.
	'file' is set to the file to/from which redirection occurs. 
	Returns 0 to redirect stdin
	return 1 to redirect stdout
	returns -1 if neither.
	returns -2 if the command is botched
*/
int redirect(const char * s, char ** command, char ** file);


/*
	Executes a command that contains pipes.
	'commands' is expected to contain references
	to type char **.  Argc is the number of commands in the list.
*/
void pipe_to(List * commands, int argc);


/*
	If the string 's' contains leading whitespace
	the entire string is shifted left until no
	whitespace remains at the front of the string. 
*/
void lstrip(char * s);


#endif