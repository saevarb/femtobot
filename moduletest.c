#include <stdio.h>
#include <dlfcn.h>

int main(void)
{
	int (*moduletest)(void);
	void *lib_handle;
	char *error;
	
	
	lib_handle = dlopen("./libmodule.so", RTLD_LAZY);
	
	if(!lib_handle)
	{
		printf("could not load module.: %s", dlerror());
		return 0;
	}
	
	moduletest = dlsym(lib_handle, "privmsg_handler");
	
	if((error = dlerror()) != NULL)
	{
		printf("could not resolve symbol: %s\n", error);
		return 0;
	}
	
	moduletest();
	
	dlclose(lib_handle);
	
}
