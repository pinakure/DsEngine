#ifndef CORE_WORLD
#define CORE_WORLD

#include "sector.hpp"

class World {
private:
public:
	static Sector *origin;
	static int cell_size;
	static bool initialize();
	static void deinitialize();
	static void draw();
	static void drawMiniMap();
	static void drawSectorInfo();
	static void update(double delta);
	static void drawRadar(Sector *o, int recursivity = 0);
	static void setOrigin(Sector *o);
	static void shutdown();
};

#endif