
/* Define error constants here */

/* Means the erring function uses errno 
 * report error. The error can thus be 
 * retrieved with strerror */
#define ERR_CHECK_ERRNO    	 0x0001
/* Means a setting in the config file
 * was defined twice. */
#define ERR_ALRDY_DEFINED  	 0x0002
/* Invalid config line */
#define ERR_INVALID_LINE 	 0x0003
/* Line is a comment */
#define ERR_COMMENT_LINE	 0x0004
/* Could not find essential setting in config file */
#define ERR_SETTING_MISSING  0x0005
