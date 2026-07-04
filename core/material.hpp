#ifndef CORE_MATERIAL
#define CORE_MATERIAL

#include "terrains.hpp"

class Material {
private:
public:
	int r = 0;
	int g = 0;
	int b = 0;
	int alpha = 255;
	int moisture = 0;
	bool swimable  = false;
	bool walkable  = false;
	Terrains type;
	static Material preset[16];
	static int toPreset(Terrains type);
	static void setup(Material *self, Terrains type);
	static Terrains identify(int index);
	static void initialize();
	Material(Terrains type=Terrains::SEA) { Material::setup(this, type); }
	const char *toString();
};

#endif