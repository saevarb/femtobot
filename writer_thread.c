#include "writer_thread.h"


void *writer_thread(void *params)
{
    struct timespec ts;
    struct timeval now;
    int quit = 0;
	thread_data *tdata = (thread_data*)params;
	
	gettimeofday(&now, NULL);
	
	ts.tv_sec = now.tv_sec + 2;
	ts.tv_nsec = now.tv_usec * 1000;
	
	pthread_mutex_lock(tdata->quit_mutex);
	
	while(*tdata->should_quit != 1)
	{
		switch(pthread_cond_timedwait(tdata->quit_cond, tdata->quit_mutex, &ts))
		{
			case ETIMEDOUT:
				pthread_mutex_lock(tdata->print_mutex);
				printf("Writer thread still waiting..\n");
				pthread_mutex_unlock(tdata->print_mutex);
				gettimeofday(&now, NULL);
				ts.tv_sec = now.tv_sec + 2;
				ts.tv_nsec = now.tv_usec * 1000;
				break;
			
			case 0:
				quit = 1;
				break;
				
			default:
				break;
		}
		
		if(quit)
			break;
	}

	
	
	pthread_mutex_unlock(tdata->quit_mutex);
	pthread_exit(NULL);
}
