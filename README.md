# DaSchel
A Linux shell written for my CSCD340 Operating Systems class at EWU


## Building
`make`

## Running
`./ssh`

_Note: It's only called 'ssh' because that was the convention required for the class._


## Notes

The history file ".ssh_history" will be created in the pwd upon successful exit.

No more than 200 commands will be saved into the history file.

Quirks:
	If the history file is removed via "rm" while inside the shell,
	 it will be recreated upon shell exit.

	The shell only supports redirecting a file once.
	Multiple redirects result in undefined behavior.
