#include "material.hpp"
Material Material::preset[16];

void Material::initialize() {
#define SETUPMATERIAL(t)	Material::setup(&Material::preset[Material::toPreset(t)], t)
	SETUPMATERIAL(Terrains::SEA);
	SETUPMATERIAL(Terrains::SHORE);
	SETUPMATERIAL(Terrains::SAND);
	SETUPMATERIAL(Terrains::GRASS);
	SETUPMATERIAL(Terrains::DEEP_GRASS);
	SETUPMATERIAL(Terrains::TALL_GRASS);
	SETUPMATERIAL(Terrains::MOUNTAIN);
	SETUPMATERIAL(Terrains::TERRAINS_UNKNOWN);
#undef SETUPMATERIAL	
}

int Material::toPreset(Terrains type) {
	switch (type) {
		default:	
		case Terrains::TERRAINS_UNKNOWN: return  0;
		case Terrains::SEA:				 return  1;
		case Terrains::SHORE:			 return  2;
		case Terrains::SAND:			 return  3;
		case Terrains::GRASS:			 return  4;
		case Terrains::DEEP_GRASS:		 return  5;
		case Terrains::TALL_GRASS:		 return  7;
		case Terrains::MOUNTAIN:		 return  8;
		case Terrains::TERRAINS_MAX:	 return  15;
	}
}

const char *Material::toString() {
	switch (this->type) {
		default:	
		case Terrains::TERRAINS_UNKNOWN: return  "TERRAINS_UNKNOWN";
		case Terrains::SEA:				 return  "SEA";
		case Terrains::SHORE:			 return  "SHORE";
		case Terrains::SAND:			 return  "SAND";
		case Terrains::GRASS:			 return  "GRASS";
		case Terrains::DEEP_GRASS:		 return  "DEEP_GRASS";
		case Terrains::TALL_GRASS:		 return  "TALL_GRASS";
		case Terrains::MOUNTAIN:		 return  "MOUNTAIN";
		case Terrains::TERRAINS_MAX:	 return  "TERRAINS_MAX";
	}
}

void Material::setup(Material *self, Terrains type) {
	self->type = type;
	self->r = 0;
	self->g = 0;
	self->b = 0;
	self->moisture = 0;
	self->walkable = true;
	self->swimable = false;			
	switch (type) {
		case SEA:
			self->b = 255;
			self->moisture = 100;
			self->swimable = true;
			self->walkable = false;
			return;
		case SHORE:
			self->moisture = 50;
			self->r = 185;
			self->g = 185;
			self->b = 100;
			return;
		case SAND:
			self->moisture = 15;			
			self->r = 230;
			self->g = 230;
			self->b =  62;
			return;
		case GRASS:
			self->moisture = 35;			
			self->r =  64;
			self->g = 240;
			self->b =  16;
			return;
		case DEEP_GRASS:
			self->moisture = 40;			
			self->r =  32;
			self->g = 180;
			self->b =  32;
			return;
		case TALL_GRASS:
			self->moisture = 45;
			self->r =  16;
			self->g = 128;
			self->b =  16;
			return;
		case MOUNTAIN:
			self->moisture = 30;			
			self->r = 64;
			self->g = 32;
			self->b = 32;
			return;
		default: case TERRAINS_UNKNOWN:
			self->alpha = 0;
			return;
	}
}

Terrains Material::identify(int index) {
	if((index>=0) && (index < Terrains::SHORE)) {
		return Terrains::SEA;
	} else if (index < Terrains::SAND) {
		return Terrains::SHORE;
	} else if (index < Terrains::GRASS) {
		return Terrains::SAND;
	} else if (index < Terrains::DEEP_GRASS) {
		return Terrains::GRASS;
	} else if (index < Terrains::TALL_GRASS) {
		return Terrains::DEEP_GRASS;
	} else if (index < Terrains::MOUNTAIN){
		return Terrains::TALL_GRASS;
	} else if (index < Terrains::TERRAINS_MAX){
		return Terrains::MOUNTAIN;
	} else return Terrains::TERRAINS_UNKNOWN;
}