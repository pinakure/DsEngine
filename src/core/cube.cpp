#include "cube.hpp"

Cube::Cube(int width, int height, int depth, int color, Picture texture) {
	this->width = width;
	this->height = height;
	this->depth = depth;
	this->color = color;
	this->texture = texture;
}

Cube::Cube(const Cube &o) {
	this->width  = o.width;
	this->height = o.height;
	this->depth  = o.depth;
	this->color  = o.color;
	// Copy vertex data
	// Copy texture data
}

Cube::~Cube() {
	// Destroy vertex data
	// Destroy texture data
}
