#include <stdio.h>
#include <string.h>

typedef struct _serverinfo
{
	char **nick;
	char *host;
	unsigned short int port;
	channel *channels;
}botinfo;

typedef struct _channel
{
	char *channelname;
	char *password;
}channel;

int get_settings(char *filename)
{
	FILE *in;
	but
}
