#include <stdio.h>
#include "includes.h"

void __attribute__((constructor)) init(void)
{
	printf("Moduletest loaded..\n");
};

ssize_t safe_send(irc *ircbot, int sockfd, const void *buf, size_t len, int flags)
{
	ssize_t ret = 0;
	pthread_mutex_lock(&ircbot->mutex);
	
	ret = send(sockfd, buf, len, flags);
	
	pthread_mutex_unlock(&ircbot->mutex);
	
	return ret;
}

void *ping_handler(void *params)
{
	message *msg = (message*)params;
	char tempbuf[256] = {0};
	int i = 0;
	
	sprintf(tempbuf, "PONG %s\r\n", msg->params);
	
	safe_send(msg->ircbot, msg->ircbot->sockfd, tempbuf, strlen(tempbuf), 0);
	
	for(i = 0; i < msg->ircbot->chan_count; i++)
	{
		sprintf(tempbuf, "PRIVMSG %s :Shiiet, I just got pinged with %s and I handled it!\r\n", msg->ircbot->channels[i], msg->params);
		safe_send(msg->ircbot, msg->ircbot->sockfd, tempbuf, strlen(tempbuf), 0);
	}
	
}

void *privmsg_handler(void *params)
{
	message *msg = (message*)params;
	int i, j;
	
	if(strstr(msg->params, "!listmodules"))
	{
		char tempbuf[256] = {0};
		
		for(i = 0; i < msg->ircbot->module_count; i++)
		{
			sprintf(tempbuf, "PRIVMSG #hackerthreads :%s\r\n", msg->ircbot->modules[i].name);
			safe_send(msg->ircbot, msg->ircbot->sockfd, tempbuf, strlen(tempbuf), 0);
			for(j = 0; j < msg->ircbot->modules[i].func_count; j++)
			{
				
				sprintf(tempbuf, "PRIVMSG #hackerthreads :     Symbol %d: %p\r\n", j, msg->ircbot->modules[i].funcs[j]);
				safe_send(msg->ircbot, msg->ircbot->sockfd, tempbuf, strlen(tempbuf), 0);
			}
		}
		
	}
	
	return 0;
}

void *notice_handler(void *params)
{
	message *msg = (message*)params;
	
	printf("%s", msg->command);
	
	return 0;
}
