
#include "cscd340s14hw3.h"

char * toString(void * data){
	return (char*)data;
}

int main(){

	//load history first
	List * history = load_history(HISTORY_FILE);

	while(interp(history));

	//save history before exit
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
		printf("command #%d\n", commandNo);

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

	if(strncmp(buf, "cd ", 3)){
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

int execute(const char * command){

	printf("Executing %s\n", command);

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