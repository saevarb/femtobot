
int 			get_settings			(char *filename, bot_variable **variables);
int 			authenticate			(irc *ircbot);
bot_variable* 	find_setting			(const char *name, const int variable_count, bot_variable *variables);
int 			get_channels			(irc *ircbot, bot_variable *channels);
void 			join_channels			(irc *ircbot);
int 			check_sock_state		(irc *ircbot);
int 			recv_and_handle_data	(irc *ircbot);
int 			parse_message			(irc *ircbot, char *raw_msg);
ssize_t		 	safe_send				(irc *ircbot, int sockfd, const void *buf, size_t len, int flags);
int 			load_modules			(irc *ircbot);


