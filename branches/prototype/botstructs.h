
#ifndef __BOTSTRUCTS_H_
#define __BOTSTRUCTS_H_H

/* Better to typedef it - easier to read */
typedef void *(*func_ptr_t)(void *);

typedef struct _variable
{
	char name[MAX_VARIABLE_LENGTH];
	char value[MAX_VARIABLE_LENGTH];
}bot_variable;

typedef struct _module
{
	char *name;
	void *handle;
	func_ptr_t *funcs;
	int func_count;
}module;

typedef struct _event
{
	char *event_name;
	int event_handler_count;
	func_ptr_t *event_handlers;
}event;

/* TODO: Divide into smaller structs
 * No need to pass a struct with all this
 * around to all functions.
 * Even thought it's passed by reference,
 * some functions shouldn't have any business
 * with sockfd and so on.
 * Sending stuff to sockfd inside a module might 
 * wreak havoc as well. */
typedef struct _irc
{
	char *nick;
	char **channels;
	char *server;
	char server_ip[20];						
	char *write_buffer;
	int  write_buffer_size;
	
	int chan_count;
	
	int port;
	int sockfd;
	struct addrinfo *servinfo;
	struct timeval tv;
	
	event *event_chain;
	int   event_count;
	
	module *modules;
	int module_count;
	
	/* Normally initalized with PTHREAD_MUTEX_INITIALIZER 
	 * Can't use that in a struct, using pthread_mutex_init */
	pthread_mutex_t mutex;
}irc;

typedef struct _message
{
	int has_prefix;
	char *prefix;
	char *command;
	char *params;
	irc *ircbot;
}message;



#endif
