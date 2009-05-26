#include "error_handling.h"

int _error(const char *function, int error)
{
	if(error == ERR_CHECK_ERRNO)
	{
		fprintf(stderr, "FATAL: %s failed with error: (%d)%s\n", function, errno, strerror(errno));
	}
	else
	{
		fprintf(stderr, "FATAL: %s failed with error: %d\n", function, error);
	}
	return 0;
}
