#ifndef CORE_SPRITE
#define CORE_SPRITE

#include "surface.hpp"

class Sprite {
private:
	bool initialized = false;

public:
	int width	= 0; 
	int height	= 0;
	Surface picture;
	bool isInitialized() { return this->initialized; }
	Sprite(const char *filename=nullptr);

};

#endif 
