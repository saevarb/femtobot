#include "main.h"

int main(void)
{
	bot_info b_info;
	bot_config *setting_ptr;
	int i, ret;
	
	/* Mandatory intialization. Failure to do this
	 * may result in havoc later on. Especially
	 * if you are using the _count variables without 
	 * properly intializing them to zero */
	b_info.nick_count = 0;
	b_info.nicks = NULL;
	b_info.server = NULL;
	b_info.b_config = NULL;
	b_info.config_count = 0;
	b_info.channel_count = 0;
	b_info.channels = NULL;
	
	if( (ret = read_settings("bot.conf", &b_info)) != 0)
	{
		_error("read_settings()", ret);
		/* Do cleanup here */
	}
	
	for(i = 0; i < b_info.config_count; i++)
	{
		printf("'%s' = '%s' len = %d\n", b_info.b_config[i].setting, b_info.b_config[i].value, strlen(b_info.b_config[i].setting));
	}
	
	if((setting_ptr = get_setting("NICKS", &b_info)) == NULL)
	{
		_error("get_settings(NICKS)", ERR_SETTING_MISSING);
		/* Do cleanup here */
		return 0;
	}
	if((ret = parse_nicks(setting_ptr->value, &b_info)) != 0)
	{
		_error("parse_nicks()", ret);
		/* Do cleanup here */
		return 0;
	}
	
	if((setting_ptr = get_setting("SERVER", &b_info)) == NULL)
	{
		_error("get_settings(SERVER)", ERR_SETTING_MISSING);
		/* Do cleanup here */
		return 0;
	}
	if((ret = parse_servers(setting_ptr->value, &b_info)) != 0)
	{
		_error("parse_servers()", ret);
		/* Do cleanup here */
		return 0;
	}
	
	if((setting_ptr = get_setting("PORT", &b_info)) == NULL)
	{
		_error("get_settings(PORT)", ERR_SETTING_MISSING);
		/* Do cleanup here */
		return 0;
	}
	
	if((ret = parse_port(setting_ptr->value, &b_info)) != 0)
	{
		_error("parse_port()", ret);
		/* Do cleanup here */
		return 0;
	}
	
	if((setting_ptr = get_setting("CHANNELS", &b_info)) == NULL)
	{
		_error("get_settings(CHANNELS)", ERR_SETTING_MISSING);
		/* Do cleanup here */
		return 0;
	}
	
	if((ret = parse_channels(setting_ptr->value, &b_info)) != 0)
	{
		_error("parse_channels()", ret);
		/* Do cleanup here */
		return 0;
	}
	
	printf("Nicks:\n");
	for(i = 0; i < b_info.nick_count; i++)
		printf("\t%s\n", b_info.nicks[i]);
	printf("Channels:\n");
	for(i = 0; i < b_info.channel_count; i++)
		printf("\t%s\n", b_info.channels[i].name);
	printf("Server:\n\t%s\n", b_info.server);
	printf("Port:\n\t%d\n", b_info.port);
	
	
	printf("ret: %d\n", ret);
	
	return 0;
}
