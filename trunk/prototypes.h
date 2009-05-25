#include "structs.h"

/* structs.h will be needed for prototypes with custom
 * structs in their parameter list */


int 		get_settings(char *filename, bot_info *b_info);
bot_config *get_setting(const char *setting, bot_info *b_info);
int 		parse_nicks(char *nicks_setting, bot_info *b_info);
int 		parse_channels(char *channels_setting, bot_info *b_info);
