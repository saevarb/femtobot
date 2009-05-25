#include "main.h"

int main(void)
{
	bot_info b_info;
	bot_config *setting_ptr;
	int i;
	
	/* Mandatory intialization. Failure to do this
	 * may result in havoc later on. Especially
	 * if you are using the _count variables without 
	 * properly intializing them to zero */
	b_info.server_count = 0;
	b_info.nick_count = 0;
	b_info.nicks = NULL;
	b_info.servers = NULL;
	b_info.b_config = NULL;
	b_info.config_count = 0;
	
	int ret = parse_file("bot.conf", &b_info);
	
	if((setting_ptr = get_setting("NICKS", &b_info)) == NULL)
	{
		printf("ERROR: No nicknames defined in the config file.\n");
		return 0;
	}
	
	/* Todo: Error handling */
	parse_nicks(setting_ptr->value, &b_info);
	
	for(i = 0; i < b_info.nick_count; i++)
	{
		printf("%s\n", b_info.nicks[i]);
	}
	
	
	printf("ret: %d\n", ret);
	
	return 0;
}
