#include "config_parsing.h"

int get_settings(char *filename)
{
	FILE *in;
	unsigned int count = 0, buffer_size = BUFFER_INCREMENTS;
	char *buffer, one;
		
	in = fopen(filename, "rt");
	if(in == NULL)
	{
		fprintf(stderr, "Error: failed to open config file `%s`.", filename);
		return -1;
	}
	
	buffer = calloc(BUFFER_INCREMENTS, sizeof(char));
	if(buffer == NULL)
	{
		fprintf(stderr, "Error: Memory allocation failed.");
		exit(1);
	}
	
	while((one = getc(in)) != EOF)
	{
		if(one == EOL)
		{
			buffer[count + 1] = '\0';
			free(buffer);
			count = 0;
			buffer_size = BUFFER_INCREMENTS;
			buffer = calloc(BUFFER_INCREMENTS, sizeof(char));
			if(buffer == NULL)
			{
				fprintf(stderr, "Error: Memory allocation failed.");
				exit(1);
			}
		}
		
		if(count >= (buffer_size - 2))
		{
			buffer_size += BUFFER_INCREMENTS;
			buffer = realloc(buffer, buffer_size);
			if(buffer == NULL)
			{
				fprintf(stderr, "Error: Memory allocation failed.");
				exit(1);
			}
		}
		buffer[count] = one;
		printf("%02x = %02x (%u)\n", one, buffer[count], count);
		count++;
		buffer[count] = 0;
		printf("buffer: %s\n", buffer);
	}
	return 0;
}
