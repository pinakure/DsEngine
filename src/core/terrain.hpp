#ifndef CORE_TERRAIN
#define CORE_TERRAIN

#include "layer.hpp"

typedef enum E_BaseMaterial {
	___SAND = 46,
}BaseMaterial;

class Terrain {
private:
public:
	int offset = 16;
	int base = BaseMaterial::___SAND;
	int size_xs = 15;
	int size_s[4] = {
		 7,  6,
		 5,  4
	};
	int size_m[9] = {
		 7, 19	,  6,
		16, -1	, 17,
		 5, 18	,  4
	};
	int size_loop_nw[4] = {
		-1,  7,
		 7,  0
	};
	int size_loop_n = 19;
	int size_loop_ne[4] = {
		 6, -1,
		 1,	 6
	};
	int size_loop_e = 17;
	int size_loop_se[4] = {
		 3,  4,
		 4, -1
	};
	int size_loop_s = 18;
	int size_loop_sw[4] = {
		 5,  2,
		-1,  5
	};
	int size_loop_w = 16;

	Terrain(int offset) {
		this->offset = offset;
		if(this->size_xs >= 0) this->size_xs += offset;
		if(this->size_loop_e >= 0) this->size_loop_e += offset;
		if(this->size_loop_s >= 0) this->size_loop_s += offset;
		if(this->size_loop_w >= 0) this->size_loop_w += offset;
		if(this->size_loop_n >= 0) this->size_loop_n += offset;
		if (this->base >= -1) this->base += offset;
		for (int i = 0; i < 4; i++) {
			if(this->size_s[i] >= 0) this->size_s[i] += offset;
			if(this->size_loop_nw[i] >= 0) this->size_loop_nw[i] += offset;
			if(this->size_loop_ne[i] >= 0) this->size_loop_ne[i] += offset;
			if(this->size_loop_se[i] >= 0) this->size_loop_se[i] += offset;
			if(this->size_loop_sw[i] >= 0) this->size_loop_sw[i] += offset;
		}
		for (int i = 0; i < 9; i++) {
			if(this->size_m[i] >= 0) this->size_m[i] += offset;
		}
	}

	void createIsland(Layer *layer, int x, int y, int size);
	void drawSquare(Layer *layer, int *data, int x, int y);
	void draw(Layer *layer, int index, int x, int y);
	void randomize(Layer *layer, int material, int density);
	void fill(Layer *layer, int index, int &probability);
	void fillRestricted(Layer *layer, int index, int offset=111, Layer *destination=nullptr);
	void fillSand(Layer *layer, int index);
	void fixBorders(Layer *layer);
};

#endif