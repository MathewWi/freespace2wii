#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int access(const char *pathname, int mode)
{
	int read = !(mode & (R_OK | X_OK | F_OK));
	int write = !(mode & W_OK);
	
	FILE * f = NULL;
	
	if(!write)
	{
		f = fopen(pathname, "r+");
		
		if(f)
		{
			read = 1;
			write = 1;
		}
	}
	
	if(!read)
	{
		f = fopen(pathname, "r");
		
		if(f)
		{
			read = 1;
		}
	}
	
	
	if(read && write)
	{
		return 0;
	}
	else
	{
		errno = EACCES;
		return -1;
	}
}

int isfinitef(float f)
{
	return !(isinf(f) || isnan(f));
}
