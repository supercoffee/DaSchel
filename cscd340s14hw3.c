#include "cscd340s14hw3.h"

#define DEBUG 0

char * toString(void * data){
	return (char*)data;
}

int main(){

	//load history first
	List * history = load_history(HISTORY_FILE);

	while(interp(history));

	// //save history before exit
	save_history(history, HISTORY_FILE);

	list_destroy(history);

	return 0;
}

void show_history(List* history){

	Iterator* iter = list_iterator(history);
	int number = 1;

	while(iter_hasNext(iter)){

		printf("%d %s \n", number++, (char*)iter_next(iter));

	}

	iter_destroy(iter);

}

/*
	Read commands from the user and execute them if valid.
	Saves the entered command to history. 
	Returns 0 if the user exits.
	Returns -1 if the user inputs a bogus command.
	Returns 1 if command execution was successful.
*/
int interp(List * history){

	char buf[256];
	char * copy;
	int result = 0;

	printf("DaSchel $ ");

	fgets(buf, 256, stdin);

	strip(buf);

	if(strcmp(buf, "exit") == 0){
		return 0;
	}

	if(strlen(buf) == 0){
		return -1;
	}

	// assign buf to the last command entered
	if(strcmp(buf, "!!") == 0 ){
		copy = (char*)list_get(history, list_size(history) -1);
		strncpy(buf, copy, 256);
	}
	/*
		If the bang is the first symbol in the string, the pointer
		will be the same as that of the entire string.  
	*/
	else if(index(buf, '!') == buf){

		char * bufptr = buf + 1;
		int commandNo;
		result = sscanf(bufptr, "%d", &commandNo);

		#if DEBUG
		printf("command #%d\n", commandNo);
		#endif

		/*
			If the result is not 0 and the result is some valid 
			number in the list history, then we can pull out that item
			from history to execute it again.
		*/
		if(result){
			//invalid command number.
			if(commandNo > list_size(history) && commandNo < 1){
				return -1;
			}
			/* 	the history list is 1 based indexing
			 	the linked list is 0 based, 
			 	therefore subtract one. 
			 */
			copy = (char *)list_get(history, commandNo -1);
			strncpy(buf, copy, 256);
		}


	}

	if(strcmp(buf, "history") == 0){
		show_history(history);
		return 1;
	}

	if(strncmp(buf, "cd ", 3) == 0){
		char * temp = buf + 3;
		result = chdir(temp);
		if(result){
			printf("Invalid directory: %s\n", temp);
			return -1;
		}
		return 1;
	}



	/*
		If the first character in buf is not a null terminator, 
		then the command is worth saving. 
	*/
	if(*buf){

		/*
			Check that the command just executed is not the same
			as the last command from the list.  If so, we
			don't need to add the command to the history. 
		*/
		char * lastExecuted = NULL;
		lastExecuted = list_get(history, list_size(history) -1) ;

		if( !lastExecuted || strcmp(buf, lastExecuted) ){
			copy = (char *)calloc(strlen(buf)+1, sizeof(char));
			strcpy(copy, buf);
			list_add(history, copy);
		}


	}

	/*
		If the command typed is not an internal command,
		then it is some external executable. 
		Appropriately execute the command.
	*/
	return execute(buf);

}

int execute(const char * str){

	// printf("Executing %s\n", str);

	int argc;
	//test for pipes
	char ** pipes = makeargs(str, "|", &argc);

	/*
		No pipes in this command, or at least not in a valid way. 
	*/
	if(argc == 1){

		#if DEBUG
		printf("No pipes in %s\n", str);
		#endif

		if(strchr(str, '>') || strchr(str, '<')){

			char * file;
			char * command;

			int direction = redirect(str, &command, &file);

			/*	Abort. No redirect actually exists here.
			 	This is most likely because the redirect 
				was not used in a semantically correct way.
			*/	
			if(direction == -1){

				return -1;
			}

			int filedesc;

			//stdin redirect
			if(direction == 0){
				filedesc = open(file, O_RDONLY);
			}
 
			//stdout redirect
			else if(direction == 1){
				filedesc = open(file, O_WRONLY);
			}
		} 	

		/*
			No pipes
			No redirects, 
			just plain old command execution.
		*/
		else{

			char ** command = makeargs(str, " ", &argc);
			pid_t pid = fork();
			if(!pid){

				if(execvp(command[0], command)){
					exit(-1);
				}	
			}
			else{
				wait(NULL);
			}

			freeargs(&command);

		}


	}

	/*
		Put that in your pipe and smoke it!!
	*/
	else{

		#if DEBUG
		printf("Found %d commands in %s\n", argc, str);
		#endif

		int commandcount;
		List * commandsToPipe = list_create();

		int i;
		for(i = 0; i < argc; i++){

			list_add(commandsToPipe, makeargs(pipes[i], " ", &commandcount));

		}

		pipe_to(commandsToPipe, argc);


		/*
			The normal list destroy method doesn't work here
			because we have char **.  Need to implement 
			a function that can free any type of data from the
			list. 
		*/
		for(i = 0; i < argc; i++){

			char ** temp = list_pop(commandsToPipe);

			freeargs(&temp);

		}		

		list_destroy(commandsToPipe);

	}

	freeargs(&pipes);
}


void pipe_to(List * commandList,  int argc){

	pid_t pid;

	int status;

	int cmdNo = 0, i;

	int numpipes  = argc -1;

	int pipefds[2* numpipes];

	/*
		Set up all the pipes that will be used ahead
		of time. 
	*/
	for(i = 0; i < numpipes; i++){
		if(pipe(pipefds + i*2) < 0){
			exit(-1);
		}
	}

	i = 0;

	Iterator  * commands = list_iterator(commandList);

	while(iter_hasNext(commands)){
		char ** command = (char**)iter_next(commands);

		pid = fork();

		if(!pid){

			// not the last command
			if(iter_hasNext(commands)){
				dup2(pipefds[i + 1], 1);
			}

			//not the first command
			if(i != 0){
				dup2(pipefds[i-2], 0);
			}

			for(i = 0; i < 2 * numpipes; i++){
				close(pipefds[i]);
			}

			/*
				If this funtion ever returns
				this is bad and we need to kill the
				current proc.
			*/	
			if(execvp(command[0], command)){
				exit(-1);
			}
		}

		i += 2;
	}

	for(i = 0; i < numpipes *2; i++){
		close(pipefds[i]);
	}

	for(i = 0; i <numpipes + 1; i++){
		wait(&status);
	}

	iter_destroy(commands);

}

/*
	Implement later.  Just needs to determine the file redirect
	and set it to file.
	
*/
int redirect(const char * s, char ** command, char ** file){


	return -1;
}

void freeargs(char *** args){

	int i;
	for(i = 0; (*args)[i] != NULL; i++){
		free((*args)[i]);
	}

	free(*args);
	*args = NULL;

}


char ** makeargs(const char * s, const char * delim, int * argc){

	
	int i, tokenCount;
	char buf[256];
	char * token;
	List * tempList = list_create();
	char ** argv;

	char * dup = strdup(s);

	//this is needed so we can free the allocation at the end
	char * dup2 = dup;

	strip(dup);

	while((token = strtok(dup, delim)) != NULL){
		dup = NULL;
		char * temp = (char*)calloc(strlen(token) +1, sizeof(char));

		strcpy(temp, token);

		list_add(tempList, temp);
	}

	tokenCount = list_size(tempList);
	*argc = tokenCount;

	argv = (char**)calloc(tokenCount +1, sizeof(char*));

	for(i = 0; i < tokenCount; i++){

		char * temp = list_dequeue(tempList);

		argv[i] = temp;

	}
	
	list_destroy(tempList);
	free(dup2);

	return argv;
}

List * load_history(char * filename){

	List * historyList = list_create();

	char temp[256];

	if(access( filename, R_OK) == 0){

		FILE * fin = fopen(filename, "r");

		while( ! feof (fin)){
			/*
				This solution is compatible with files
				that contain trailing newlines and those that do 
				not. 
			*/
			if(fgets(temp, 256, fin)){

				strip(temp);
			
				char * savedCommand = (char*)calloc(strlen(temp)+1, sizeof(char));
				strcpy(savedCommand, temp);

				list_add(historyList, savedCommand);
			}
		}

		fclose(fin);

	}

	return historyList;

}

int save_history(List * history, char * filename){

	FILE * fout = fopen(filename, "w");

	if(!fout){
		return -1;
	}

	int i;
	for(i = 0; i < list_size(history); i++){

		char * command = (char *) list_get(history, i);

		fprintf(fout, "%s\n", command);

	}

	fclose(fout);
	return 0;
}

void strip(char * buf){
	int length = strlen(buf) -1;
    if (buf[length] == '\n')
        buf[length] = '\0';
}// end strip