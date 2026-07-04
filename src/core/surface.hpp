#ifndef CORE_SURFACE
#define CORE_SURFACE

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "vertex.hpp"
#include <string>

class Surface {
private:
	bool	initialized = false;
public:
	bool    isInitialized() { return this->initialized; }

	ALLEGRO_BITMAP *bitmap = NULL;
	int		width	= 32;
	int		height	= 32;
	int		x		= 0;
	int		y		= 0;
	int		z		= -127;
	Surface(int width, int height, int x=1, int y=200, int z=0);
	Surface();
	Surface(const Surface &o);
	~Surface();
	void draw();
	void blit(Surface &destination, int dest_x, int dest_y, int width, int height);
	void cleanup();
	bool load(std::string filename);	
};

#endif