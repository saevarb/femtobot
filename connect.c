#include "connect.h"

#define MAX_MESSAGE_LENGTH 512	/* To be replaced or moved, added for quick and dirty connection test */

/* TODO write a decent auth function
 * or change nicks that are in use in the standard module? */
int irc_authenticate(bot_info *b_info, int *sockfd)
{
	char authbuffer[MAX_MESSAGE_LENGTH+1] = {0};
	int len = 0, ret;

	len = snprintf(	authbuffer,
					MAX_MESSAGE_LENGTH+1,
					"USER %1$s %1$s %1$s :%1$s\r\n"
					"NICK %1$s\r\n",
					b_info->nicks[0]);
	ret = send(*sockfd, authbuffer, len, 0);
	return 0;
}

int irc_connect(int *sockfd, bot_info *b_info)
{
	struct addrinfo		hints, *servinfo, *p;
	int ret, i;
	char port[6] = {0};

	snprintf(port, 5, "%d", b_info->port);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family	= AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	/* Retry 5 times, increase sleep by 100 ms after each try */
	for(i = 0; (ret = getaddrinfo(b_info->server, port, &hints, &servinfo)) == EAI_AGAIN && (i < 5); i++)
	{
		fprintf(stderr, "Warning: Retrying host resolving (%d)\n", (i + 1));
		usleep(100*i);
	}

	switch(ret)
	{
		case 0:
			break;
		case EAI_MEMORY:
			return ERR_OUT_MEMORY;
			break;
		default:
			return ERR_CONNECTION_FAIL;
			break;
	}

	/* Connect to the first possible result */
	ret = 0;
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			continue;
		}

		if(connect(*sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(*sockfd);
			continue;
		}
		ret = 1;
		break;
	}

	freeaddrinfo(servinfo);

	if(ret != 1)
	{
		return ERR_CHECK_ERRNO;
	}

	irc_authenticate(b_info, sockfd);

	return 0;
}
