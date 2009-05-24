#include "main.h"

int main(void)
{
	
	int ret = get_settings("bot.conf");
	
	printf("ret: %d\n", ret);
	
	return 0;
}
