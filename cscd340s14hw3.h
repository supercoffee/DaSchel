#pragma once

#ifndef CSCD340S14HW3_H
#define CSCD340S14HW3_H

#include "linkedlist/linkedlist.h"
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define HISTORY_FILE ".ssh_history"

List * load_history(char * filename);

int save_history(List * history, char * filename);

void strip(char * buf);

int interp(List * history);

void show_history(List* history);

int execute(const char * command);

char ** makeargs(const char * s, const char * delim, int * argc);

void freeargs(char *** args);


/*
	Returns 0 to redirect stdin
	return 1 to redirect stdout
	returns -1 if neither.
*/
int redirect(const char * s, char ** command, char ** file);


#endif