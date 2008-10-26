#ifndef _WIIASSERT_H_
#define _WIIASSERT_H_


#define exit(a) pause_exit((a), __FILE__, __LINE__)


#ifdef __cplusplus
extern "C" {
#endif




void WiiAssert(const char * text,const char *filename, int line);
void pause_exit(int code,const char *filename, int line);



#ifdef __cplusplus
}
#endif


#endif
