#include "structs.h"

/* structs.h will be needed for prototypes with custom
 * structs in their parameter list */

/* Configuration parsing */
int 		read_settings	(const char *, bot_info *);
bot_config *get_setting		(const char *, bot_info *);
int 		parse_nicks		(char *, 	   bot_info *);
int 		parse_servers	(char *, 	   bot_info *);
int 		parse_channels	(char *, 	   bot_info *);
int 		parse_port		(char *, 	   bot_info *);

/* Network stuff */
int			irc_connect		(int *,		   bot_info *);

/* Module support */
int 		load_modules	(event **);

/* Error handling */
int 		_error			(const char *, int);

/* Thread stuff */
void *reader_thread         (void *);
void *writer_thread         (void *);
