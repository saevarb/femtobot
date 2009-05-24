#include "config_parsing.h"

int get_settings(char *filename)
{
	FILE *in;
	unsigned int read_count = 0, buffer_size = BUFFER_INCREMENTS;
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
			buffer[read_count ] = '\0';
			printf("%s\n", buffer);
			
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
