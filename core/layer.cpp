#include "sector.hpp"
#include "layer.hpp"
#include "gpu.hpp"
#include "kore.hpp"
#include "camera.hpp"
#include "world.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

Layer::Layer(void *parent) {
	this->parent = parent;
	strcpy_s(this->name, 64, "Untitled");
	this->width = 0;
	this->height = 0;
	this->r += float(TRUERAND(64)) - 32.0f;
	this->g += float(TRUERAND(64)) - 32.0f;
	this->b += float(TRUERAND(64)) - 32.0f;
}

void Layer::fill(int index) {
	int c = 0;
	for (int y = 0; y < this->height; y++) {
		for (int x = 0; x < this->width; x++) {
			this->set(x, y, index);
			c++;
		}
	}
}

Layer::Layer(const char *name, int width, int height, double parallax, __int64 *data, size_t data_size, int x, int y, int z, void *parent){
	this->parent = parent;
	try {
		load(name, width, height, parallax, data, data_size);
		this->r += float(TRUERAND(64)) - 32.0f;
		this->g += float(TRUERAND(64)) - 32.0f;
		this->b += float(TRUERAND(64)) - 32.0f;
		this->x = x * Sector::size * World::cell_size;
		this->y = y * Sector::size * World::cell_size;
		this->z = z * Sector::size * World::cell_size;		
	}
	catch (int e) {
		e;
		al_show_native_message_box(Gpu::screen, "Error", "Error", "Failed to initialize layer!", NULL, ALLEGRO_MESSAGEBOX_ERROR);			
	}
}

void Layer::load(const char *name, int width, int height, double parallax, __int64 *data, size_t data_size) {
	strcpy_s(this->name, 64, name);
	this->width		= width;
	this->height	= height;
	this->parallax	= parallax;
	this->data = new __int64[data_size];
	memcpy(this->data, data, data_size);
}

#include "direction.hpp"

__int64 Layer::get(int x, int y){
	Sector *p = (Sector*)this->parent;
	if (x < 0) if (p->sibiling[Direction::LEFT]) { return p->sibiling[Direction::LEFT ]->data->get(x + Sector::size, y); }	else return -1;
	if(y  < 0) if (p->sibiling[Direction::NORTH]){ return p->sibiling[Direction::NORTH]->data->get(x, y + Sector::size); }	else return -1;
	if(x >= this->width	) if (p->sibiling[Direction::RIGHT]) { return p->sibiling[Direction::RIGHT ]->data->get(x - Sector::size, y); }	else return -1;
	if(y >= height		) if (p->sibiling[Direction::SOUTH]) { return p->sibiling[Direction::SOUTH ]->data->get(x, y - Sector::size); }	else return -1;
	return this->data[(y * this->width) + x];
}

void Layer::set(int x, int y, __int64 value){
	Sector *p = (Sector*)this->parent;
	if (x < 0) if (p->sibiling[Direction::LEFT]) { p->sibiling[Direction::LEFT]->data->set(x + Sector::size, y, value); return; } else return;
	if (y < 0) if (p->sibiling[Direction::NORTH]) { p->sibiling[Direction::NORTH]->data->set(x, y + Sector::size, value); return; }	else return;
	if(x >= this->width	) if (p->sibiling[Direction::RIGHT]) { p->sibiling[Direction::RIGHT ]->data->set(x - Sector::size, y,value); return; }	else return;
	if(y >= height		) if (p->sibiling[Direction::SOUTH]) { p->sibiling[Direction::SOUTH ]->data->set(x, y - Sector::size, value); return; }	else return;
	__int64 *i;
	i = this->data;
	i[((y * this->width) + x)] = value;
}

#include "material.hpp"

void Layer::draw(int camera_offset_x, int camera_offset_y){
	int x, y;
	int ix = (Camera::x - camera_offset_x);// *this->parallax;
	int iy = (Camera::y - camera_offset_y);// *this->parallax;
	int ox = ix;
	int oy = iy;
	int lx = ox + Gpu::width;
	int ly = oy + Gpu::height;
	int dx = -World::cell_size, dy = -World::cell_size; // Start one row and one column before visible screen
	
	dx -= ox % World::cell_size;
	dy -= oy % World::cell_size;
	
	ox /= World::cell_size;
	oy /= World::cell_size;
	lx /= World::cell_size;
	ly /= World::cell_size;
	
	int dxInit = dx;
	int dyInit = dy;
	for(y = oy; y < ly+2; y++){
		for(x = ox; x < lx+2; x++){
			
			__int64 index = (y * this->width) + x;

			if( 
				(index >= 0 && index < (this->width*this->height))
										&& 
					  	  ((x >= 0)&&(x < this->width)) 
			){
				int c = this->data[index];
				
				if(c >= 0) {
					if(c < __int64(std::size(Sector::animations))) {
						Animation *t = &Sector::animations[c];
						if (t) {
							t->draw(Gpu::buffer, dx, dy);
						}
					} else {
						//al_set_target_bitmap(Gpu::buffer.bitmap);
						al_draw_bitmap(Sector::tile[c - std::size(Sector::animations)], dx, dy, 0);
					}
					if (this->color==true) {
						float factor = float(abs((index % Sector::size) - (Sector::size/2) ) ) / (Sector::size*2);
						ALLEGRO_COLOR color = al_map_rgba(float(this->r)*factor, float(this->g)*factor, float(this->b)*factor, 1.0f - factor);
						if (index == this->highlight_index) {
							Material *m = &Material::preset[
								Material::toPreset(
									Material::identify(c)
								)
							];
							color = al_map_rgba(m->r/2, m->g/2, m->b/2, 16);
						}
						al_draw_filled_rectangle(
							dx,
							dy,
							dx + World::cell_size,
							dy + World::cell_size,
							color
						);
						if (this->grid) {
							Material *m = &Material::preset[
								Material::toPreset(
									Material::identify(c)
								)
							];
							color = al_map_rgba(m->r/2, m->g/2, m->b/2, 16);
							al_draw_rectangle(
								dx,
								dy,
								dx + World::cell_size-1,
								dy + World::cell_size-1,
								color,
								1.0f
							);
						}
					}			
				}

				#ifdef DRAW_TILE_IDS
				if (this->numbers) {
					Gpu::setFontColor(0, 255, 255, 24);
					Gpu::printCentered(
						std::to_string(c).c_str(),
						dx + (World::cell_size >> 1),
						dy + (World::cell_size >> 2)
					);
					Gpu::setFontColor(0, 255, 0, 200);
				}
				#endif 
			}
			dx += World::cell_size;
		}
		dy += World::cell_size;
		dx = dxInit;
	}
}

#include "material.hpp"

ALLEGRO_COLOR Layer::getColor(int x, int y, int opacity) {
	int t = this->get(x, y);
	Material *m = &Material::preset[Material::toPreset(Material::identify(t))];
	if (m->alpha > 0)m->alpha = opacity;
	return al_map_rgba(m->r, m->g, m->b, m->alpha);	
}

void Layer::randomize() {
	int i = 0;
	for (int y = 0; y < this->height; y++) {
		for (int x = 0; x < this->width; x++) {
			this->data[i] = (float(rand()) / float(RAND_MAX) ) * 1; 
			i++;
		}	
	}
}

Layer::~Layer(void){
	if (this->data) {
		delete this->data;
	}
}

void Layer::setHighlight(int x, int y) {
	Sector *p = (Sector*)this->parent;
	if (x < 0) if (p->sibiling[Direction::LEFT]) { p->sibiling[Direction::LEFT]->data->setHighlight(x + Sector::size, y); return; } else return;
	if (y < 0) if (p->sibiling[Direction::NORTH]) { p->sibiling[Direction::NORTH]->data->setHighlight(x, y + Sector::size); return; }	else return;
	if(x >= this->width	) if (p->sibiling[Direction::RIGHT]) { p->sibiling[Direction::RIGHT ]->data->setHighlight(x - Sector::size, y); return; }	else return;
	if(y >= this->height) if (p->sibiling[Direction::SOUTH]) { p->sibiling[Direction::SOUTH ]->data->setHighlight(x, y - Sector::size); return; }	else return;
	this->highlight_index = (y*this->width) + x;
}