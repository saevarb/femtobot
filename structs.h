
#ifndef __STRUCTS_H_
#define __STRUCTS_H_

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
	char *channelname;
	char *password;
	/* Used for leaving and join channels */
	unsigned short int joined;
} channel;

/* Contains info about a server */
typedef struct _serverinfo
{
	char *host;
	unsigned short int port;
	channel *channels;
} serverinfo;

/* Contains the info about the bot */
typedef struct _botinfo
{
	char **nicks;
	unsigned int server_count;
	serverinfo *servers;
} botinfo;

#endif
