
#ifdef __STRUCTS_H_
#define __STRUCTS_H_

typedef struct _thread_data
{
	unsigned char *write_buffer;
	unsigned int *write_buffer_size;
	pthread_mutex_t *write_mutex;
	pthread_cond_t  *quit_event;
} thread_data;


#endif
