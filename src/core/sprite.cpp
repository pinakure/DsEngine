#include "sprite.hpp"

Sprite::Sprite(const char *filename) {
	if (filename != nullptr) {
		this->picture.load(filename);
		if (this->picture.isInitialized()) {
			this->width = this->picture.width;
			this->height= this->picture.height;
			this->initialized = true;
		}
	}
}

