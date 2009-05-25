#include "error_handling.h"

int _error(int error)
{
	if(error == ERR_CHECK_ERRNO)
	{
		fprintf(stderr, "Fatal error occurred: (%d)%s\n", errno, strerror(errno));
	}
	else
	{
		fprintf(stderr, "Fatal error occurred: %d\n", errno);
	}
	return 0;
}
