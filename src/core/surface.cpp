#include "surface.hpp"
#include "gpu.hpp"
#include <allegro5/allegro_native_dialog.h>

Surface::Surface(int width, int height, int x, int y, int z) {
	try {
		this->bitmap = Gpu::createImage(width, height);
		this->initialized = true;
		this->width  = width;
		this->height = height;
		this->x = x;
		this->y = y;
		this->z = z;
	} catch (int e) {e;
		this->initialized = false;
		this->width = 0;
		this->height= 0;
		this->x= 0;
		this->y= 0;
		this->z= 0;
	}
}

bool Surface::load(std::string _filename) {
	std::string filename = "data/sprites/"+_filename+".png";
	this->cleanup();
	try {
		this->bitmap = al_load_bitmap(filename.c_str());
		this->width = al_get_bitmap_width(this->bitmap);
		this->height= al_get_bitmap_height(this->bitmap);
		this->initialized = true;
		return true;
	} catch (int e) {e;
		this->width = 0;
		this->height= 0;
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->initialized = false;
		return false;
	}	
}

void Surface::cleanup() {
	al_set_target_backbuffer(Gpu::screen);
	if (this->bitmap) {
		al_destroy_bitmap(this->bitmap);
		this->width = 0;
		this->height= 0;
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->bitmap = nullptr;
	}
}

Surface::~Surface() {
}

Surface::Surface(const Surface &o) {
	try {
		// copy bitmap data 
		this->width = o.width;
		this->height = o.height;
		this->y= o.y;
		this->x= o.x;
		this->z= o.z;
		this->initialized = true;
		this->bitmap = Gpu::createImage(o.width, o.height);
		((Surface)o).blit(*this, 0, 0, o.width, o.height);		
	} catch(int e){e;
		al_show_native_message_box(Gpu::screen, "Error", "Error", "Failed to create surface!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		this->width = 0;
		this->height = 0;
		this->y= 0;
		this->x= 0;
		this->z= 0;
		this->initialized = false;
		this->bitmap = nullptr;
	}
}

Surface::Surface() {
	this->bitmap = nullptr;
	this->width = 0;
	this->height = 0;
	this->y= 0;
	this->x= 0;
	this->z= 0;
	this->initialized = false;
}

void Surface::draw() {
	al_set_target_backbuffer(Gpu::screen);
	al_draw_bitmap(this->bitmap,this->x, this->y, 0);
}

void Surface::blit(Surface &destination, int dest_x, int dest_y, int width, int height) {
	// Draw picture to destination picture
	al_set_target_bitmap(destination.bitmap);
	al_draw_bitmap_region(this->bitmap, 0, 0, width > this->width?this->width:width, height>this->height?this->height:height, dest_x, dest_y, 0);
	al_set_target_backbuffer(Gpu::screen);
}