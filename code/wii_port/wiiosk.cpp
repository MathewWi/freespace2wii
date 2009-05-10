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

struct ButtonData
{
	ButtonData() : text_p(NULL) {};
	const char * text_p;
	std::string text;
	ButtonState s;
	int x,y,w,h;
	bool selectable;
};

class OSK
{
private:
	typedef std::vector<std::vector<ButtonData> > buts;
	buts buttons;
	size_t max_len;
	int button_w, button_h;
	int spacing;
	int draw_offset_x, draw_offset_y;
public:
	OSK(const char *char_matrix[], int width, int height)
	{
		draw_offset_x = 0;
		draw_offset_y = 0;
		
		int count = 2;
		
		const char **s = char_matrix;
		
		max_len = 0;
		
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
			ButtonData & but = b->at(0);
			but.text_p = NULL;
			but.s = BS_NONE;
			but.x = x;
			but.y = y;
			but.w = width;
			but.h = button_h;
			but.selectable = false;
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
				
				ButtonData & but = b->at(i);
				but.text = c;
				but.text_p = but.text.c_str();
				but.s = BS_NONE;
				but.x = x;
				but.y = y;
				but.w = button_w;
				but.h = button_h;
				but.selectable = true;
				
				x += button_w + spacing;
			}
			y += button_h + spacing;
			
			++s;
			++b;
		}
		
		x = 0;
		
		b->resize(2);
		{
			ButtonData & but = b->at(0);
			but.text = "Accept";
			but.text_p = but.text.c_str();
			but.s = BS_NONE;
			but.x = x;
			but.y = y;
			but.w = (width-spacing)/2;
			but.h = button_h;
			but.selectable = true;
			x += but.w + spacing;
		}
		{
			ButtonData & but = b->at(1);
			but.text = "Cancel";
			but.text_p = but.text.c_str();
			but.s = BS_NONE;
			but.x = x;
			but.y = y;
			but.w = (width-spacing)/2;
			but.h = button_h;
			but.selectable = true;
		}
	}
	
	ButtonData * getButton(int x, int y)
	{
		x -= draw_offset_x;
		y -= draw_offset_y;
		
		for(size_t i = 0; i < buttons.size(); ++i)
		{
			for(size_t j = 0; j < buttons[i].size(); ++j)
			{
				ButtonData & but = buttons[i][j];
				
				if(x >= but.x && x <= but.x+but.w &&
					y >= but.y && y <= but.y+but.h)
				{
					return &but;
				}
			}
		}
		
		return NULL;
	}
	
	void draw(int xoff, int yoff)
	{
		draw_offset_x = xoff;
		draw_offset_y = yoff;
		
		for(size_t i = 0; i < buttons.size(); ++i)
		{
			for(size_t j = 0; j < buttons[i].size(); ++j)
			{
				ButtonData & but = buttons[i][j];
				
				if(!but.selectable)
				{
					but.s = BS_NONE;
				}
				
				switch(but.s)
				{
				case BS_OVER:
					gr_set_color_fast( &Color_green );
					break;
				case BS_DOWN:
					gr_set_color_fast( &Color_bright_blue );
					break;
				default:
				case BS_NONE:
					gr_set_color_fast( &Color_dim_red );
					break;
				}
				
				gr_rect(but.x+xoff, but.y+yoff, but.w, but.h);
			
				if(but.text_p)
				{
					gr_set_color_fast( &CWHITE );
					gr_string(but.x+xoff+but.w/2, but.y+yoff+but.h/2, but.text_p);
				}
			}
		}			
	}
	
	void setText(const char * msg)
	{
		buttons[0][0].text_p = msg;
	}
	
	void clearState()
	{
		for(size_t i = 0; i < buttons.size(); ++i)
		{
			for(size_t j = 0; j < buttons[i].size(); ++j)
			{
				buttons[i][j].s = BS_NONE;
			}
		}
	}
};

OSK kb(char_matrix, 560, 420);

void draw_osk(int x, int y, int width, int height)
{
	kb.draw(x, y);
}

int char_to_SDLkey(char c)
{
	int key;
	
	switch(c)
	{
	case 'A': key = SDLK_RETURN; break;
	case 'C': key = SDLK_ESCAPE; break;
	case 'a': key = SDLK_a; break;
	case 'b': key = SDLK_b; break;
	case 'c': key = SDLK_c; break;
	case 'd': key = SDLK_d; break;
	case 'e': key = SDLK_e; break;
	case 'f': key = SDLK_f; break;
	case 'g': key = SDLK_g; break;
	case 'h': key = SDLK_h; break;
	case 'i': key = SDLK_i; break;
	case 'j': key = SDLK_j; break;
	case 'k': key = SDLK_k; break;
	case 'l': key = SDLK_l; break;
	case 'm': key = SDLK_m; break;
	case 'n': key = SDLK_n; break;
	case 'o': key = SDLK_o; break;
	case 'p': key = SDLK_p; break;
	case 'q': key = SDLK_q; break;
	case 'r': key = SDLK_r; break;
	case 's': key = SDLK_s; break;
	case 't': key = SDLK_t; break;
	case 'u': key = SDLK_u; break;
	case 'v': key = SDLK_v; break;
	case 'w': key = SDLK_w; break;
	case 'x': key = SDLK_x; break;
	case 'y': key = SDLK_y; break;
	case 'z': key = SDLK_z; break;
	case '0': key = SDLK_0; break;
	case '1': key = SDLK_1; break;
	case '2': key = SDLK_2; break;
	case '3': key = SDLK_3; break;
	case '4': key = SDLK_4; break;
	case '5': key = SDLK_5; break;
	case '6': key = SDLK_6; break;
	case '7': key = SDLK_7; break;
	case '8': key = SDLK_8; break;
	case '9': key = SDLK_9; break;
	default: key = -1;
	}
	
	return key;
}

static bool last_right = false;

void process_osk(int mouse_x, int mouse_y, bool mouse_left, bool mouse_right, osk_key_hit callfun)
{
	ButtonData * but = kb.getButton(mouse_x, mouse_y);
	
	if(mouse_right && !last_right)
	{
		kb.setText(callfun(SDLK_BACKSPACE, SDL_KEYDOWN));
	}
	
	last_right = mouse_right;
	
	if(but)
	{	
		if(mouse_left)
		{			
			switch(but->s)
			{
			case BS_OVER:
			case BS_DOWN: but->s = BS_DOWN; break;
			default:
			case BS_NONE: but->s = BS_NONE; break;
			}
		}
		else
		{
			if(but->s == BS_DOWN)
			{
				if(but->text_p)
				{
					int key = char_to_SDLkey(but->text_p[0]);
					if(key != -1)
					{
						kb.setText(callfun(key, SDL_KEYUP));
					}
				}
			}
			
			but->s = BS_OVER;
		}	
	}
	
	ButtonState b_s = BS_NONE;
	if(but) b_s = but->s;
	kb.clearState();
	if(but) but->s = b_s;
}
