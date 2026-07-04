#ifndef CORE_GPU
#define CORE_GPU

#define GPU_DEFAULT_FULLSCREEN false
//#define DRAW_TILE_IDS

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <vector>

#include "surface.hpp"

class Gpu {
private:
public:
	static ALLEGRO_COLOR		font_color;
	static int					width;
	static int					height;
	static bool					fullscreen;	
	static float				zoom;
	static Surface				buffer;
	static Surface				overlay;
	static Surface				midlayer;
	static ALLEGRO_DISPLAY		*screen;
	static ALLEGRO_FONT*		font;
	static bool					redraw;
	static int					frame_rate;
	
	 Gpu(){};
	~Gpu(){};
	static double fps;
	static int frames;
	static int initialize(int width, int height, bool fullscreen=GPU_DEFAULT_FULLSCREEN);
	static void flip();
	static void preflip();
	static void cleanup();
	static void wait(double time);
	static void clear(int r = 0, int g = 0, int b = 0);
	static ALLEGRO_BITMAP *createImage(int width, int height);
	static void setFontColor(int r, int g, int b, int a=255);
	static void print(const char *text, int x = 0, int y = 0);
	static void printRight(const char *text, int x = 0, int y = 0);
	static void printCentered(const char *text, int x = 0, int y = 0);
	static void shutdown();
};

#endif