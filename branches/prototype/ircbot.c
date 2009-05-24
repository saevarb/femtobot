#include "includes.h"


int main(int argc, char **argv)
{
	bot_variable *ptr, *variables = NULL;
	struct addrinfo hints;
	struct sockaddr_in *sin;
	int ret,
		variable_count, i;
	irc ircbot;
	
	variable_count = get_settings(NULL, &variables); 
	
	ircbot.event_count = ircbot.module_count = 0;
	ircbot.event_chain =  NULL;
	ircbot.modules = NULL;
	
	
	if(load_modules(&ircbot) < 0)
		printf("failed");
	
	for(i = 0; i < ircbot.module_count; i++)
	{
		printf("Module: %s\n", ircbot.modules[i].name);
		int x;
		
		for(x = 0; x < ircbot.modules[i].func_count; x++)
		{
			printf("\tSymbol %d: %p\n", x, ircbot.modules[i].funcs[x]);
		}
	}
	
	for(i = 0; i < ircbot.event_count; i++)
	{
		printf("Event: %s\n", ircbot.event_chain[i].event_name);
		
		int x;
		
		for(x = 0; x < ircbot.event_chain[i].event_handler_count; x++)
		{
			printf("\tSymbol %d: %p\n", x, ircbot.event_chain[i].event_handlers[x]);
		}
	}
	
	
	ptr = find_setting("SERVER", variable_count, variables);
	ircbot.server = ptr ? ptr->value : NULL;
	
	ptr = find_setting("NICK", variable_count, variables);
	ircbot.nick = ptr ? ptr->value : NULL;
	
	ptr = find_setting("PORT", variable_count, variables);
	ircbot.port = ptr ? atoi(ptr->value) : (int)NULL;
	
	if((ptr = find_setting("CHANNELS", variable_count, variables)) != NULL)
	{
		get_channels(&ircbot, ptr);
	}
	
	if(!ircbot.server)
	{
		fprintf(stderr, "FATAL: Server was not specified in config.\n");
		return 0;
	}
	if(!ircbot.nick)
	{
		fprintf(stderr, "FATAL: Nick was not specified in config.\n");
		return 0;
	}
	if(!ircbot.port)
	{
		fprintf(stderr, "FATAL: Port was not specified in config.\n");
		return 0;
	}
	
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	if((ret = getaddrinfo(ircbot.server, "6667", &hints, &(ircbot.servinfo))) != 0)
	{
		fprintf(stderr, "FATAL: getaddrinfo() returned: (%d) %s\n", ret, gai_strerror(ret));
		return 0;
	}
	
	
	sin = (struct sockaddr_in*)ircbot.servinfo->ai_addr;
	
	inet_ntop(ircbot.servinfo->ai_family, &(sin->sin_addr), ircbot.server_ip, sizeof(ircbot.server_ip));
	
	
	ircbot.sockfd = socket(ircbot.servinfo->ai_family, ircbot.servinfo->ai_socktype, ircbot.servinfo->ai_protocol);
	
	if(ircbot.sockfd == -1)
	{
		fprintf(stderr, "FATAL: socket() failed with error: %d..\n", errno);
		return 0;
	}
	
	/* Initialize ircbot stuff here until we have a function for it */
	pthread_mutex_init(&ircbot.mutex, NULL);
	ircbot.write_buffer = NULL;
	ircbot.write_buffer_size = 0;
	
	/****************************************************************/
	
	if((ret = connect(ircbot.sockfd, ircbot.servinfo->ai_addr, ircbot.servinfo->ai_addrlen)) == -1)
	{
		fprintf(stderr, "FATAL: connect() faild with error: %d..\n", errno);
		
		return 0;
	}
	fprintf(stderr, "[+] Connected to %s(%s)..\n", ircbot.server, ircbot.server_ip);
	
	authenticate(&ircbot);
	join_channels(&ircbot);
	
	while(1)
	{
		
		ret = check_sock_state(&ircbot);
		
		//fprintf(stderr, "Here..\n");
		if(ret == -1)
		{
			fprintf(stderr, "FATAL: select() failed with error: %d..\n", errno);
			return 0;
		}
		/* Can read */
		else if(ret)
		{
			recv_and_handle_data(&ircbot);
		}
		
	}
	
	
	close(ircbot.sockfd);
	freeaddrinfo(ircbot.servinfo);
	free(variables);
	pthread_mutex_destroy(&ircbot.mutex);
	
	return 0;
}




