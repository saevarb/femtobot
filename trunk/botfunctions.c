#include "includes.h"

int get_settings(char *filename, bot_variable **variables)
{
	FILE *in;
	char name[64] = {0}, value[64] = {0};
	int count = 0;
	
	if(!filename)
	{
		in = fopen("bot.conf", "r");
	}
	else
	{
		in = fopen(filename, "r");
	}
	
	if(!in)
	{
		return -1;
	}
	
	while(!feof(in))
	{
		fscanf(in, "%[^=]=%s\n", name, value);
		
		
		if(strlen(name) > MAX_VARIABLE_LENGTH-1 || strlen(value) > MAX_VARIABLE_LENGTH-1) // 1 byte for 0 terminator.
		{
			fprintf(stderr, "WARNING: Variable pair '%s'='%s' is too long -- skipping.\n", name, value);
			continue;
		}
		
		count++;
		
		*variables = realloc(*variables, count * sizeof(bot_variable));
		
		strncpy((*variables)[count-1].name, name, MAX_VARIABLE_LENGTH);
		strncpy((*variables)[count-1].value, value, MAX_VARIABLE_LENGTH);
		
	}
	
	fclose(in);
	
	return count;
}

int load_modules(irc *ircbot)
{
	FILE *in = fopen("bot.modules", "r");
	char module_name[256] = {0}, cur_event_name[256] = {0}, functions[256] = {0}, *ptr;
	int count = 0;
	func_ptr_t temp_fptr = NULL;
	int i = 0;
	
	if(!in)
	{
		return -1;
	}
	
	while(!feof(in))
	{
		if(fscanf(in, "[module %d %[^]]]\n", &count, module_name))
		{
			/* Add a module */
			ircbot->modules = realloc(ircbot->modules, (ircbot->module_count + 1) * sizeof(module));
			ircbot->modules[ircbot->module_count].name = malloc(strlen(module_name));
			strncpy(ircbot->modules[ircbot->module_count].name, module_name, strlen(module_name));
			ircbot->modules[ircbot->module_count].handle = dlopen(module_name, RTLD_LAZY);
			ircbot->modules[ircbot->module_count].func_count = 0;
			ircbot->modules[ircbot->module_count].funcs = NULL;
			for(; count > 0; count--)
			{
				fscanf(in, "event=%[^\n]\n", cur_event_name);
				fscanf(in, "functions=%[^\n]\n", functions);
				
				/* i will either be the index of the event in the event chain
				 * so we can add functions to it, or the index of the new event
				 * to be added. */
				for(i = 0; i < ircbot->event_count; i++)
				{
					if(strcmp(ircbot->event_chain[i].event_name, cur_event_name) == 0)
						break;
				}
				
				/* If it loops through without breaking, the event hasn't been added yet */
				if(i == ircbot->event_count)
				{
					/* Add the event to the event chain */
					ircbot->event_chain = realloc(ircbot->event_chain, (i+1) * sizeof(event));
					ircbot->event_chain[i].event_name = malloc(strlen(cur_event_name));
					strncpy(ircbot->event_chain[ircbot->event_count].event_name, cur_event_name, strlen(cur_event_name)); 
					ircbot->event_chain[i].event_handler_count = 0;
					ircbot->event_chain[i].event_handlers = NULL;
					ircbot->event_count++;
				}
				
				ptr = strtok(functions, ", ");
				while(ptr)
				{
					temp_fptr = dlsym(ircbot->modules[ircbot->module_count].handle, ptr);
					
					if(!temp_fptr)
					{
						printf("Could not resolve symbol '%s': %s\n", functions, dlerror());
						ptr = strtok(NULL, ", ");
						continue;
					}
					/* Add the function to the its respective owner module struct.
					 * Needed for unloading on request, so we know which functions
					 * need to be removed from the event_chain, when a module is unloaded. */
					ircbot->modules[ircbot->module_count].funcs = realloc(ircbot->modules[ircbot->module_count].funcs, (ircbot->modules[ircbot->module_count].func_count + 1) * sizeof(func_ptr_t));
					ircbot->modules[ircbot->module_count].funcs[ircbot->modules[ircbot->module_count].func_count] = temp_fptr;
					ircbot->modules[ircbot->module_count].func_count++;
					
					ircbot->event_chain[i].event_handlers = realloc(ircbot->event_chain[i].event_handlers, (ircbot->event_chain[i].event_handler_count + 1) * sizeof(func_ptr_t));
					ircbot->event_chain[i].event_handlers[ircbot->event_chain[i].event_handler_count] = temp_fptr;
					ircbot->event_chain[i].event_handler_count++;
					
					ptr = strtok(NULL, ", ");
				}
				
				
				
			}
			
			ircbot->module_count++;
			
		}
		
	}
	
	
	return 0;
}

int authenticate(irc *ircbot)
{
	char authbuffer[MAX_MESSAGE_LENGTH+1] = {0};
	int len = 0;
	
	len = snprintf(authbuffer, 
	               MAX_MESSAGE_LENGTH+1, 
				   "USER %1$s %1$s %1$s :%1$s\r\n"
				   "NICK %1$s\r\n", 
				   ircbot->nick);
	safe_send(ircbot, ircbot->sockfd, authbuffer, len, 0);				 
	
	
	return 0;
}

bot_variable *find_setting(const char *name, const int variable_count, bot_variable *variables)
{
	int i = 0;
	
	for(; i < variable_count; i++)
	{
		if(strcmp(name, variables[i].name) == 0)
			return &variables[i];
	}
	
	return NULL;
}

int get_channels(irc *ircbot, bot_variable *channels)
{
	char *ptr, *last;
	int len = 0;
	ircbot->chan_count = 0;
	ircbot->channels = NULL;
	ptr = strchr(channels->value, ',');
	last = channels->value;
	while(ptr)
	{
		len = ptr - last;
		
		ircbot->chan_count++;
		
		
		ircbot->channels = realloc(ircbot->channels, sizeof(char*) * ircbot->chan_count);
		ircbot->channels[ircbot->chan_count - 1] = malloc(len + 1);
		
		/* snprintf max len argument takes the null terminator into account */
		snprintf(ircbot->channels[ircbot->chan_count - 1], len + 1, "%s", last);
		

	
		last = ptr+1;
		ptr = strchr(ptr+1, ',');
	}
	
	if(!ptr)
	{
		/* Only one channel; channels->value */
		if(!ircbot->chan_count) 
		{
			//fprintf(stderr, "Only one channel: %s\n", channels->value);
			ircbot->chan_count++;
		
			ircbot->channels = malloc(ircbot->chan_count * sizeof(char*));
		
			ircbot->channels[0] = malloc(strlen(channels->value) + 1);
		
			snprintf(ircbot->channels[0], strlen(channels->value) + 1, "%s", channels->value );
		}
		/* Handle edge case where last char isn't a comma */
		else if(channels->value[strlen(channels->value)-1] != ',') 
		{
			/* lastptr should point to char after comma, not the comma */
			char *nullptr = strchr(channels->value, 0), *lastptr = strrchr(channels->value, ',') + 1;
			
			ircbot->chan_count++;
		
			ircbot->channels = realloc(ircbot->channels, ircbot->chan_count * sizeof(char*));
		
			ircbot->channels[ircbot->chan_count - 1] = malloc(nullptr - lastptr + 1);
			
			snprintf(ircbot->channels[ircbot->chan_count - 1], nullptr - lastptr + 1, "%s", lastptr);
			
			//fprintf(stderr, "Got last channel: %s\n", lastptr);
			
		}
		else
		{
			return 0;
		}	
	}
	
	return 1;
}

void join_channels(irc *ircbot)
{
	int i, len = 0;
	char tempbuf[256] = {0};
	
	for(i = 0; i < ircbot->chan_count; i++)
	{
		len = snprintf(tempbuf, 256, "JOIN %s\r\n", ircbot->channels[i]);
		safe_send(ircbot, ircbot->sockfd, tempbuf, len, 0); 
	}
	
}

ssize_t safe_send(irc *ircbot, int sockfd, const void *buf, size_t len, int flags)
{
	ssize_t ret = 0;
	pthread_mutex_lock(&ircbot->mutex);
	
	ret = send(sockfd, buf, len, flags);
	
	pthread_mutex_unlock(&ircbot->mutex);
	
	return ret;
}

int add_to_write_buf(irc *ircbot, char *str, int str_size)
{
	pthread_mutex_lock(&ircbot->mutex);
	
	ircbot->write_buffer_size += str_size;
	
	/* +1 for null */
	ircbot->write_buffer = realloc(ircbot->write_buffer, ircbot->write_buffer_size + 1);
	
	strncat(ircbot->write_buffer, str, str_size);
	
	pthread_mutex_unlock(&ircbot->mutex);
	
	return str_size;
}

int check_sock_state(irc *ircbot)
{
	fd_set readfds;
	int ret = 0;
	struct timeval tv;
	
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	
	FD_ZERO(&readfds);
	FD_SET(ircbot->sockfd, &readfds);
	
	ret = select(ircbot->sockfd+1, &readfds, NULL, NULL, &tv);
	
	return ret ? FD_ISSET(ircbot->sockfd, &readfds) : ret;
}

int recv_and_handle_data(irc *ircbot)
{
	char *startptr, 
		 *endptr, 
		 cur_raw_msg[MAX_MESSAGE_LENGTH+1] = {0},
		 recv_buffer[MAX_MESSAGE_LENGTH+1] = {0};
	static char last_recv_buffer[MAX_MESSAGE_LENGTH+1] = {0};
	
	recv(ircbot->sockfd, recv_buffer, MAX_MESSAGE_LENGTH, 0);
				
	startptr = endptr = recv_buffer;
		
	/* Handle first message */
	endptr = strstr(endptr, "\r\n");
	strncpy(cur_raw_msg, recv_buffer, endptr - recv_buffer + 2);
	
		
	/* Might be a partial */
	if(recv_buffer[0] != ':')
	{
		/* If the end of last_recv_buffer is a \r\n
		 * this is not a partial */
		int len = strlen(last_recv_buffer), i;
			
		if(last_recv_buffer[len-2] != '\r' && last_recv_buffer[len-1] != '\n')
		{
			/* Message is a partial. Need to get start from end of last_recv_buffer
			 * and end from start of recv_buffer */
				
			/* Point to character after \r\n, not \r */
			startptr = strrchr(last_recv_buffer, '\r') + 2;
			memset(cur_raw_msg, 0, MAX_MESSAGE_LENGTH+1);
				
			for(i = 0; *startptr; i++)
			{
				cur_raw_msg[i] = *startptr++;
			}
			startptr = recv_buffer;
			for(; startptr < endptr+2; i++)
			{
				cur_raw_msg[i] = *startptr++;
			}
			
		}
			
	}
		
	//fprintf(stderr, "%s", cur_raw_msg);
	//fprintf(stderr, "%s", strstr(cur_raw_msg, "\r\n") ? "yes" : "nope");
	
	parse_message(ircbot, cur_raw_msg);
		
	/* Handle rest of messages in buffer */
	startptr = strchr(endptr, ':');
	endptr = startptr ? strstr(startptr, "\r\n") : NULL;
		
	while(startptr && endptr)
	{
		/* strncpy doesn't null terminate in this case */
		/* only if string src string ends before max len */
		strncpy(cur_raw_msg, startptr, endptr - startptr + 2);
		//fprintf(stderr, "%s", cur_raw_msg);
		//fprintf(stderr, "%s", strstr(cur_raw_msg, "\r\n") ? "yes\n" : "nope\n");
		
		parse_message(ircbot, cur_raw_msg);
		
		memset(cur_raw_msg, 0, MAX_MESSAGE_LENGTH+1);	
		startptr = strchr(endptr, ':');
		endptr = startptr ? strstr(startptr, "\r\n") : NULL;
	}
		
	memset(last_recv_buffer, 0, MAX_MESSAGE_LENGTH+1);
	snprintf(last_recv_buffer, MAX_MESSAGE_LENGTH+1, "%s", recv_buffer);
	memset(recv_buffer, 0, MAX_MESSAGE_LENGTH);
	
	return 0;		
}

int parse_message(irc *ircbot, char *raw_msg)
{
	char *startptr, *endptr;	
	int i = 0;
	message *msg = malloc(sizeof(message));
	
	if(raw_msg[0] != ':')
	{
		msg->has_prefix = 0;
		msg->prefix = NULL;
	}
	else
		msg->has_prefix = 1;
		
	if(msg->has_prefix)
	{
		/* Point to char after first char ':' */
		startptr = raw_msg+1;
		/* Point to space after prefix */
		endptr = strchr(startptr, ' ');
		
		msg->prefix = malloc(endptr - startptr + 1);
		
		i = 0;
		while(startptr != endptr)
		{
			msg->prefix[i++] = *startptr++;
		}
		msg->prefix[i] = 0;
		
		startptr++;
		endptr = strchr(startptr, ' ');
		
		i = 0;
		
		msg->command = malloc(endptr - startptr + 1);
		while(startptr != endptr)
		{
			msg->command[i++] = *startptr++;
		}
		msg->command[i] = 0;
		
		/* Point to first char of params */
		endptr++;
		
		msg->params = malloc(strlen(endptr));
		
		strcpy(msg->params, endptr);
	}
	else
	{
		/* Point to first char */
		startptr = raw_msg;
		/* And next space after command */
		endptr = strchr(raw_msg, ' ');
		
		msg->command = malloc(endptr - startptr + 1);
		
		i = 0;
		while(startptr != endptr)
		{
			msg->command[i++] = *startptr++;
		}
		msg->command[i] = 0;
		
		/* Point to first char in params */
		endptr++;
		/* Skip : */
		if(*endptr == ':')
			endptr++;
		
		msg->params = malloc(strlen(endptr));
		
		strcpy(msg->params, endptr);
	}
	
	msg->ircbot = ircbot;
	
	for(i = 0; i < ircbot->event_count; i++)
	{
		if(strcmp(ircbot->event_chain[i].event_name, msg->command) == 0)
		{
			int x;
			
			for(x = 0; x < ircbot->event_chain[i].event_handler_count; x++)
			{
				func_ptr_t temp = ircbot->event_chain[i].event_handlers[x];
				temp(msg);
			}
		}
	}
	
	free(msg);
	
	
	return 0;
}


