#ifndef __STRUCTS_H_
#define __STRUCTS_H_

/* Unfinished */
typedef struct _thread_data
{
	unsigned char *write_buffer;
	unsigned int *write_buffer_size;
	pthread_mutex_t *write_mutex;
	pthread_cond_t  *quit_event;
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

#endif
