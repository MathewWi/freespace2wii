#ifndef _WIICLOCK_H_
#define _WIICLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

clock_t wiiclock();

void tic();
void tocInternal(const char *file, int line);
void tocInternalMsg(const char *file, int line, const char *msg);
#define toc() tocInternal(__FILE__, __LINE__)
#define tocMsg(m) tocInternalMsg(__FILE__, __LINE__, m)

#ifdef __cplusplus
}
#endif

#endif
