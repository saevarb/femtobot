
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
/* Failed to load a shared object */
#define ERR_MOD_LOAD_FAILED  0x0006
/* Out of memory / allocation failed */
#define ERR_OUT_MEMORY		 0x0007
/* Failed to connect */
#define ERR_CONNECTION_FAIL	 0x0008
