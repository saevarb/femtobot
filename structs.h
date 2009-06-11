#ifndef __STRUCTS_H_
#define __STRUCTS_H_

#include <pthread.h>

/* This is used for better code readability
 * Sugges we change this file's name to "types.h"
 * or some such */

typedef void *(*func_ptr_t)(void *);

/* Unfinished */
typedef struct _thread_data
{
	pthread_mutex_t *print_mutex;
	pthread_mutex_t *quit_mutex;
	pthread_cond_t  *quit_cond;
	int             *should_quit;
} thread_data;

/* Info about a channel */
typedef struct _channel
{
	char *name;
	char *password;
	/* Used for leaving and joining channels */
	unsigned short int joined;
} channel;

/* Contains info about a server
 * NOTE: Should not be used until
 * after 1.0, when we implement
 * multi-server support */
typedef struct _server_info
{
	char *host;
	unsigned short int port;
	channel *channels;
} server_info;

/* Contains all config values from the config file */
typedef struct _bot_config
{
	char *setting;
	char *value;
} bot_config;

/* Contains the info about the bot */
typedef struct _bot_info
{
	char **nicks;
	int nick_count;
	char *server;
	int port;
	channel *channels;
	int channel_count;
	bot_config *b_config;
	unsigned int config_count;
} bot_info;

/* Needed to add this so we would have some way of
 * knowing what functions need to be removed from
 * the event chain, when we dynamically unload
 * a module, at the request of the user */
typedef struct
{
	func_ptr_t function;
	char *owner_module;
} event_handler;

/* This type will make up the event-chain */
typedef struct _event
{
	char *event_name;               /* Name of event */
	event_handler *event_handlers;  /* Array of event handlers */
	int eh_count; 					/* Num of event handlers */
} event;


#endif
