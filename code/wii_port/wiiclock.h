#ifndef _WIICLOCK_H_
#define _WIICLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

clock_t wiiclock();
void init_wiiclock();

#ifdef __cplusplus
}
#endif

#endif
