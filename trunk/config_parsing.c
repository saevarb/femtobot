#include "config_parsing.h"

bot_config *get_setting(const char *setting, bot_info *b_info)
{
	int i = 0;
	
	for(; i < b_info->config_count; i++)
	{
		if(strcmp(setting, b_info->b_config[i].setting) == 0)
		{
			return &(b_info->b_config[i]);
		}
	}
	
	return NULL;
}

/* Parse nicks, strtok, comma-separated 
 * TODO: If we just add a comma on the end of
 * nicks_setting, if it isn't already there, the
 * do..while loop will handle only one nick as well */
int parse_nicks(char *nicks_setting, bot_info *b_info)
{
	char *work_ptr = strtok(nicks_setting, ", ");
	
	do
	{
		b_info->nick_count++;
		
		b_info->nicks = realloc(b_info->nicks, b_info->nick_count * sizeof(char*));
		if(!b_info->nicks)
		{
			return ERR_CHECK_ERRNO;
		}
		b_info->nicks[b_info->nick_count-1] = malloc(strlen(work_ptr) + 1);
		if(!b_info->nicks[b_info->nick_count-1])
		{
			return ERR_CHECK_ERRNO;
		}
		
		strcpy(b_info->nicks[b_info->nick_count-1], work_ptr);
	}while((work_ptr = strtok(NULL, ", ")) != NULL);
	
	return 0;	
}

/* Parse servers, comma-separated. Must be called
 * before parse_channels, else there won't be any
 * defined servers in bot_info.
 * NOTE: Bot should only support a single server
 * at 1.0. */
int parse_servers(char *servers_setting, bot_info *b_info)
{
	/* This ensures that only the first server will be used
	 * if the user passes a comma separated list of servers
	 * in the config file. */
	char *work_ptr = strtok(servers_setting, ", ");
	
	b_info->server = malloc(strlen(work_ptr) + 1);
	
	strcpy(b_info->server, work_ptr);
	
	return 0;
}

/* Parse port. Converts it to an integer with atoi 
 * and stores it in bot_info->port */
int parse_port(char *port_setting, bot_info *b_info)
{
	b_info->port = strtol(port_setting, NULL, 10);
	
	return 0;
}

/* Parse channels, strtok, comma-separated. 
 * every comma-separated group should be checked 
 * if it contains ':' - if it does, that should be
 * parsed as well, and assigned to channel->password.
 * Otherwise, channel->password shall be set to NULL,
 * to indicate that no password is needed for the channel.*/
int parse_channels(char *channels_setting, bot_info *b_info)
{
	char *working_ptr = strtok(channels_setting, ", "),
	     *temp_ptr;
	/* For code readability */
	int count = b_info->channel_count;
	
	do
	{
		b_info->channels = realloc(b_info->channels, (count + 1) * sizeof(channel));
		if(!b_info->channels)
		{
			return ERR_CHECK_ERRNO;
		}
		
		temp_ptr = strchr(working_ptr, ':');
		if(temp_ptr)
		{
			/* So we can use work_ptr for the channel only */
			*temp_ptr = 0;
			/* Make it point to the first char in the password */
			temp_ptr++;
			
			b_info->channels[count].password = malloc(strlen(temp_ptr) + 1);
			if(!b_info->channels[count].password)
			{
				return ERR_CHECK_ERRNO;
			}
			strcpy(b_info->channels[count].password, temp_ptr);
		}
		else
		{
			/* No password */
			b_info->channels[count].password = NULL;
		}
			
		b_info->channels[count].name = malloc(strlen(working_ptr) + 1);
		if(!b_info->channels[count].name)
		{
			return ERR_CHECK_ERRNO;
		}
		strcpy(b_info->channels[count].name, working_ptr);
		
		
	}while((working_ptr = strtok(NULL, ", ")) != NULL);
	
	return 0;
}

/* No need to prototype, won't and shouldn't be used outside
 * of config_parsing.c */
int parse_line(char *buffer, bot_info *b_info)
{
	char *work_ptr;
	int cur_config_count = b_info->config_count;
	
	if(buffer[0] == '#')
		return ERR_COMMENT_LINE;
	
	/* work_ptr points to setting name */
	work_ptr = strtok(buffer, "=  ");
	
	if(!work_ptr)
	{
		return ERR_INVALID_LINE;
	}
	
	printf("First work_ptr: %s - %d\n", work_ptr, strlen(work_ptr));
	
	if(get_setting(work_ptr, b_info) != NULL)
		return ERR_ALRDY_DEFINED;
	
	/* Not in there, add a new one */
	cur_config_count++;
	/* Note: Important that b_config has been initialized to NULL
	 * before realloc is called on it */
	b_info->b_config = realloc(b_info->b_config, cur_config_count * sizeof(bot_config));
	if(!b_info->b_config)
	{
		return ERR_CHECK_ERRNO;
	}
	b_info->b_config[cur_config_count-1].setting = malloc(strlen(work_ptr) + 1); /* +1 for null byte */
	if(!b_info->b_config[cur_config_count-1].setting)
	{
		return ERR_CHECK_ERRNO;
	}
	strcpy(b_info->b_config[cur_config_count-1].setting, work_ptr);
	
	/* Make work_ptr point at the value of the setting */
	work_ptr = strtok(NULL, "= ");
	if(!work_ptr)
	{
		return ERR_INVALID_LINE;
	}
	printf("Second work_ptr: %s\n", work_ptr);
	
	b_info->b_config[cur_config_count-1].value = malloc(strlen(work_ptr) + 1);
	if(!b_info->b_config[cur_config_count-1].value)
	{
		return ERR_CHECK_ERRNO;
	}
	strncpy(b_info->b_config[cur_config_count-1].value, work_ptr, strlen(work_ptr));
	/* Update b_config with the new config count */
	b_info->config_count = cur_config_count;
	
	return 0;
}

int read_settings(const char *filename, bot_info *b_info)
{
	FILE *in;
	unsigned int read_count = 0, buffer_size = BUFFER_INCREMENTS,
				 ret;
	char *buffer = NULL, one;
		
	in = fopen(filename, "r");
	if(in == NULL)
	{
		return ERR_CHECK_ERRNO;
	}
	
	buffer = calloc(BUFFER_INCREMENTS, 1);
	if(buffer == NULL)
	{
		return ERR_CHECK_ERRNO;
	}
	
	while((one = getc(in)) != EOF && !feof(in))
	{
		if(one == EOL)
		{
			buffer[read_count] = 0;
			/* Parse the line and put whatever config entry
			 * that's in it into b_config */
			if((ret = parse_line(buffer, b_info)) != 0)
			{
				if(ret == ERR_CHECK_ERRNO)
				{
					free(buffer);
					return ret;
				}
			}
			free(buffer);
			read_count = 0;
			buffer_size = BUFFER_INCREMENTS;
			buffer = calloc(BUFFER_INCREMENTS, 1);
			if(buffer == NULL)
			{
				return ERR_CHECK_ERRNO;
			}
			continue;
		}
		if(read_count >= (buffer_size - 2))
		{
			buffer_size += BUFFER_INCREMENTS;
			buffer = realloc(buffer, buffer_size);
			if(buffer == NULL)
			{
				return ERR_CHECK_ERRNO;
			}
		}
		buffer[read_count] = one;
		read_count++;
	}
	free(buffer);
	return 0;
}
