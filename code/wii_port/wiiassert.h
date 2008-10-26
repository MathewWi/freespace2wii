#ifndef _WIIASSERT_H_
#define _WIIASSERT_H_

void WiiAssert(const char * text,const char *filename, int line);
void pause_exit(int code);
#define exit pause_exit

#endif
