#include "biome.hpp"

Biome::Biome(int moisture, int density, int temperature, BiomeType type) {
	this->moisture = moisture;
	this->density = density;
	this->temperature = temperature;
	this->type = type;
}

#define MAX_DENSITY 2048
#define MIN_DENSITY 128

Biome *Biome::create(BiomeType type) {
	switch (type) {
	    case BIOME_DESSERT:
			return new Biome(0, MAX_DENSITY   , 60, type);
			break;
        case BIOME_PLAINS:
			return new Biome(5, MAX_DENSITY   , 25, type);
			break;
        case BIOME_ISLANDS:
			return new Biome(75, MIN_DENSITY  , 30, type);
			break;
        case BIOME_OCEAN:
			return new Biome(75, MIN_DENSITY/4, 10, type);
			break;
        case BIOME_WOODS:
			return new Biome(75, MAX_DENSITY  , 21, type);
			break;
	}
	return nullptr;
}