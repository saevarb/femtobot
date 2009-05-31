#include "module_support.h"

int filter(const struct dirent *cur_dir)
{
	return strlen(cur_dir->d_name) > 3 ? !strcmp(cur_dir->d_name+strlen(cur_dir->d_name)-3, ".so") : 0;
}

int strisupper(char *str)
{
	while(*str)
		if(!isupper(*str++)) 
			return 0;
		
	return 1;
}

int is_event_in_chain(char *name, event **event_chain, int event_count)
{
	int i;
	
	for(i = 0; i < event_count; i++)
	{
		if(strcmp((*event_chain)[i].event_name, name) == 0)
			return 1;
	}
	
	return 0;
}

int add_event_to_chain(char *name, event **event_chain, int event_count)
{
	*event_chain = realloc(*event_chain, (event_count + 1) * sizeof(event));
	if(!*event_chain)
	{
		return ERR_CHECK_ERRNO;
	}
	(*event_chain)[event_count].event_name = malloc(strlen(name) + 1);
	if(!(*event_chain)[event_count].event_name)
	{
		return ERR_CHECK_ERRNO;
	}
	strcpy((*event_chain)[event_count].event_name, name);
	
	(*event_chain)[event_count].event_handlers = NULL;
	(*event_chain)[event_count].eh_count = 0;
	
	return 0;
}

int add_symbol_to_chain(char *func_name, char *cur_event_name, const char *module_name, void *module_handle, event **event_chain, int event_count)
{
	int i;
	
	for(i = 0; i < event_count; i++)
	{
		/* Please, someone deobfuscate this code.. Was too lazy to add extra
		 * variables to make it more readable */
		if(strcmp((*event_chain)[i].event_name, cur_event_name) == 0)
		{
			(*event_chain)[i].event_handlers = realloc((*event_chain)[i].event_handlers, ((*event_chain)[i].eh_count + 1) * sizeof(event_handler));
			(*event_chain)[i].event_handlers[(*event_chain)[i].eh_count].function = dlsym(module_handle, func_name);
			/* Possibly add error checking here, but should barely be necessary since we already know that the symbol
			 * is in the module, after lookin through it on disk */
			(*event_chain)[i].event_handlers[(*event_chain)[i].eh_count].owner_module = malloc(strlen(module_name) + 1);
			strcpy((*event_chain)[i].event_handlers[(*event_chain)[i].eh_count].owner_module, module_name);
			
			(*event_chain)[i].eh_count++;
		}
	}
	
	return 0;
}

int load_event_handlers(const char *filename, event **event_chain, int *no_events)
{
	FILE *in = fopen(filename, "r");
	char *file_buffer, 
	/* See next comment */
		**cur_symbol_names = NULL,
		**cur_event_names  = NULL;
	int file_size = 0, i, j, symbolstrtbl,
	/* Need this to keep track of the count of symbols
	 * that have valid event handler names, that are
	 * exported by this module */
	    cur_symbol_count = 0;
	Elf32_Ehdr *hdr;
	Elf32_Shdr *shdr;
	Elf32_Sym  *sym;
	void *module_handle;
	
	fseek(in, 0, SEEK_END);
	file_size = ftell(in);
	rewind(in);
	file_buffer = malloc(file_size);
	if(!file_buffer)
	{
		return ERR_CHECK_ERRNO;
	}
	
	fread(file_buffer, 1, file_size, in);
	
	fclose(in);
	
	hdr = (Elf32_Ehdr*)file_buffer;
	shdr = (Elf32_Shdr*)(file_buffer + hdr->e_shoff);
	
	/* We need to find the symbol string table */
	for(i = 0; i < hdr->e_shnum; i++)
	{
		if(shdr[i].sh_type == SHT_STRTAB)
		{
			if(strcmp(file_buffer+shdr[hdr->e_shstrndx].sh_offset+shdr[i].sh_name, ".dynstr") == 0)
			{
				symbolstrtbl = i;
				break;
			}
			
		}
	}
	
	for(i = 0; i < hdr->e_shnum; i++)
	{
		if(shdr[i].sh_type == SHT_DYNSYM)
		{
			sym = (Elf32_Sym*)(file_buffer+shdr[i].sh_offset);
			for(j = 0; j < shdr[i].sh_size/shdr[i].sh_entsize; j++)
			{
				/* If the function has no name, we don't need it */
				if(!sym[j].st_name)
					continue;
				/* If the size is 0, the function is in a dependency module */
				if(!sym[j].st_size)
					continue;
				/* Copying the function name into a buffer and using strtok is
				 * easier than splitting up with strchr and so on manually */
				char func_name[256] = {0}, *ptr, *original = file_buffer+shdr[symbolstrtbl].sh_offset+sym[j].st_name;
				strcpy(func_name, original);
				ptr = strtok(func_name, "_");
				/* If it starts with underscores, strtok will have stripped 
				 * them, and it should be pointing at the start of the actual
				 * name. We don't want them to start with underscores */
				if(func_name != ptr)
					continue;
				/* The function's first part, before the underscore, should be
				 * all uppercase */
				if(!strisupper(ptr))
					continue;
				
				/* Add the name of the symbol to an array */	
				cur_symbol_names = realloc(cur_symbol_names, (cur_symbol_count + 1) * sizeof(char*));
				if(!cur_symbol_names)
				{
					return ERR_CHECK_ERRNO;
				}
				cur_symbol_names[cur_symbol_count] = malloc(strlen(original) + 1);
				if(!*cur_symbol_names)
				{
					return ERR_CHECK_ERRNO;
				}
				strcpy(cur_symbol_names[cur_symbol_count], original);
				
				/* And add the event name to an array as well
				 * will make it easier to put in event chain */
				cur_event_names = realloc(cur_event_names, (cur_symbol_count + 1) * sizeof(char*));
				if(!cur_event_names)
				{
					return ERR_CHECK_ERRNO;
				}
				cur_event_names[cur_symbol_count] = malloc(strlen(ptr) + 1);
				if(!*cur_event_names)
				{
					return ERR_CHECK_ERRNO;
				}
				strcpy(cur_event_names[cur_symbol_count], ptr);
				cur_symbol_count++;
			}
		}
	}
	
	char tempbuf[256] = {0};
	sprintf(tempbuf, "./%s", filename);
	
	module_handle = dlopen(tempbuf, RTLD_LAZY);
	
	if(!module_handle)
	{
		return ERR_MOD_LOAD_FAILED;
	}
	
	for(i = 0; i < cur_symbol_count; i++)
	{
		if(!is_event_in_chain(cur_event_names[i], event_chain, *no_events))
		{
			add_event_to_chain(cur_event_names[i], event_chain, *no_events);
			(*no_events)++;
		}
		add_symbol_to_chain(cur_symbol_names[i], cur_event_names[i], filename, module_handle, event_chain, *no_events);
	}
	
	for(i = 0; i < cur_symbol_count; i++)
	{
		free(cur_symbol_names[i]);
		free(cur_event_names[i]);
	}
	free(cur_symbol_names);
	free(cur_event_names);
	free(file_buffer);
	return 0;
}



int load_modules(event **event_chain)
{
	int no_events = 0;
	int socount = 0;
	int ret = 0;
	struct dirent **solist;
	
	socount = scandir(".", &solist, filter, NULL);
	while(socount--)
	{
		//printf("%s\n", solist[socount]->d_name);
		if( (ret = load_event_handlers(solist[socount]->d_name, event_chain, &no_events)) != 0)
		{
			printf("WARNING: load_event_handlers failed with %d for %s\n", ret, solist[socount]->d_name);
		}
		free(solist[socount]);
	}
	free(solist);
	
	return no_events;
}


