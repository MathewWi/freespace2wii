#ifndef _WIIRUMBLE_H_
#define _WIIRUMBLE_H_

#ifdef __cplusplus
extern "C" {
#endif


void doRumble();
void timedRumble(float time = .1f);
void stopRumble();

#ifdef __cplusplus
}
#endif


#endif
