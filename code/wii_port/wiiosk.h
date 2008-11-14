#ifndef _WIIOSK_H_
#define _WIIOSK_H_

typedef const char * (*osk_key_hit)(int key, int modifier);

void draw_osk(int x = 40, int y = 40, int width = 560, int height = 420);
void process_osk(int mouse_x, int mouse_y, bool mouse_left, bool mouse_right, osk_key_hit callfun);


#endif
