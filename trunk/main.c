#include "main.h"

int main(void)
{
	bot_info b_info;
	bot_config *setting_ptr;
	int i, ret, no_events = 0, j;
	event *event_chain = NULL;
	
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
		return 0;
	}
	
	for(i = 0; i < b_info.config_count; i++)
	{
		printf("'%s' = '%s'\n", b_info.b_config[i].setting, b_info.b_config[i].value);
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
	
	
	no_events = load_modules(&event_chain);
	
	for(i = 0; i < no_events; i++)
	{
		Dl_info dlinfo;
		printf("Event handlers for %s\n", event_chain[i].event_name);
		for(j = 0; j < event_chain[i].eh_count; j++)
		{
			dladdr(event_chain[i].event_handlers[j].function, &dlinfo);
			printf("\t%p <%s> from %s\n", event_chain[i].event_handlers[j].function, dlinfo.dli_sname, event_chain[i].event_handlers[j].owner_module);
		}
	}
	
	return 0;
}
