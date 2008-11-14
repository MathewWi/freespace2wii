#include "wii_port/wiiosk.h"
#include "globalincs/alphacolors.h"
#include "ui/uidefs.h"

/*
void ui_hline(int x1, int x2, int y );
void ui_vline(int y1, int y2, int x );
void ui_string_centered( int x, int y, char * s );
void ui_draw_shad( int x1, int y1, int x2, int y2, int r1, int g1, int b1, int r2, int g2, int b2 );
void ui_draw_frame( int x1, int y1, int x2, int y2 );
void ui_rect( int x1, int y1, int x2, int y2 );
void ui_draw_box_out( int x1, int y1, int x2, int y2 );
void ui_draw_box_in( int x1, int y1, int x2, int y2 );
void ui_draw_line_in( int x1, int y1, int x2, int y2 );
void ui_draw_sunken_border( int x1, int y1, int x2, int y2 );
*/

static const char *char_matrix[] = {"1234567890","qwertyuiop","asdfghjkl","zxcvbnm",""};

enum ButtonState
{
	BS_NONE,
	BS_OVER,
	BS_DOWN
};

struct Button
{
	Button() : text_p(NULL) {};
	const char * text_p;
	std::string text;
	ButtonState s;
	int x,y,w,h;
};

class OSK
{
private:
	typedef std::vector<std::vector<Button> > buts;
	buts buttons;
	size_t max_len;
	int button_w, button_h;
	int spacing;
public:
	OSK(const char *char_matrix[], int width, int height)
	{
		int count = 2;
		
		const char **s = char_matrix;
		
		while(**s)
		{
			size_t len = strlen(*s++);
			if(len > max_len)
			{
				max_len = len;
			}
			
			++count;
		}
		
		buttons.resize(count);
		
		int spacing = 5;
		
		button_w = (width-(max_len-1)*spacing)/max_len;
		button_h = (height-(count-1)*spacing)/count;
		
		int x = 0;
		int y = 0;
		
		
		s = char_matrix;
		
		buts::iterator b = buttons.begin();
		
		b->resize(1);
		
		{
			Button & but = b->at(0);
			but.text_p = NULL;
			but.s = BS_NONE;
			but.x = x;
			but.y = y;
			but.w = width;
			but.h = height;
		}
		
		++b;
			
		y += button_h + spacing;
			
		while(**s)
		{
			size_t len = strlen(*s);
			
			x = ((max_len-len)*(button_w+spacing))/2;
			
			b->resize(len);
			
			for(size_t i = 0; i < len; ++i)
			{			
				char c[2];
				c[0] = (*s)[i];
				c[1] = 0;
				
				Button & but = b->at(i);
				but.text = c;
				but.text_p = but.text.c_str();
				but.s = BS_NONE;
				but.x = x;
				but.y = y;
				but.w = button_w;
				but.h = button_h;
				
				x += button_w + spacing;
			}
			y += button_h + spacing;
			
			++s;
			++b;
		}
		
		x = 0;
		
		b->resize(2);
		{
			Button & but = b->at(0);
			but.text = "Accept";
			but.text_p = but.text.c_str();
			but.s = BS_NONE;
			but.x = x;
			but.y = y;
			but.w = (width-spacing)/2;
			but.h = height;
			x += but.w;
		}
		{
			Button & but = b->at(1);
			but.text = "Cancel";
			but.text_p = but.text.c_str();
			but.s = BS_NONE;
			but.x = x;
			but.y = y;
			but.w = (width-spacing)/2;
			but.h = height;
		}
	}
	
	Button * getButton(int x, int y)
	{
		for(size_t i = 0; i < buttons.size(); ++i)
		{
			for(size_t j = 0; j < buttons[i].size(); ++j)
			{
				Button & but = buttons[i][j];
				
				if(x >= but.x && x <= but.x+but.w &&
					y >= but.y && y <= but.y+but.h)
				{
					return &but;
				}
			}
		}
	}
	
	void draw(int xoff, int yoff)
	{
		for(size_t i = 0; i < buttons.size(); ++i)
		{
			for(size_t j = 0; j < buttons[i].size(); ++j)
			{
				Button & but = buttons[i][j];
				
				switch(but.s)
				{
				case BS_NONE:
					gr_set_color_fast( &Color_dim_red );
					break;
				case BS_OVER:
					gr_set_color_fast( &Color_green );
					break;
				case BS_DOWN:
					gr_set_color_fast( &Color_bright_green );
					break;
				}
				
				gr_rect(but.x+xoff, but.x+yoff, but.w, but.h);
			
				if(but.text_p)
				{
					gr_set_color_fast( &CWHITE );
					gr_string(but.x+xoff+but.w/2, but.x+yoff+but.h/2, but.text_p);
				}
			}
		}
				
	}
};

static const char *msg = NULL;

void draw_osk(int x, int y, int width, int height)
{
	OSK kb(char_matrix, width, height);
	
	kb.draw(x, y);
}

void process_osk(int mouse_x, int mouse_y, bool mouse_left, bool mouse_right, osk_key_hit callfun)
{
}
