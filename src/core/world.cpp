#include "world.hpp"
#include "gpu.hpp"
#include <allegro5/allegro.h>

#include "material.hpp"
#include "terrain.hpp"

Sector* World::origin = nullptr;
int		World::cell_size = 32;

#include "kore.hpp"

void World::shutdown() {
	Kore::runlevel--;
}

bool World::initialize() {

	Material::initialize();

	if (!Sector::initialize()) {
		al_show_native_message_box(Gpu::screen, "Error", "Error", "Failed to initialize sector!", NULL, ALLEGRO_MESSAGEBOX_ERROR);			
		return false;
	}


	for (int z = 0; z < 1; z++) {
		for (int y = 0; y < 1; y++) {
			for (int x = 0; x < 1; x++) {			
				Sector *s = new Sector(x, y, z);
			}
			Gpu::printCentered(std::to_string(y).c_str(), Gpu::width / 2, 0);
			Gpu::flip();
		}
		
	}
	World::origin = Sector::find(0, 0, 0);
	Kore::runlevel++;
	return true;
}

void World::deinitialize() {
	origin->destroy(true);
	Sector::deinitialize();
}

void World::update(double delta) {
	for (int i = 0; i < 64; i++) {
		Sector::animations[i].run(delta/16.0);
	}
}

void World::drawSectorInfo() {
	#define GAUGE_X 8
	#define GAUGE_OPACITY 128.0f
	#define GAUGE_WIDTH 128
	#define GAUGE_HEIGHT 4
	#define MOISTURE_GAUGE_Y (Gpu::height-(8+(GAUGE_HEIGHT*1)+0))
	#define DENSITY_GAUGE_Y  (Gpu::height-(8+(GAUGE_HEIGHT*2)+2))
	al_draw_filled_rectangle(
		GAUGE_X,
		DENSITY_GAUGE_Y,
		GAUGE_X + GAUGE_WIDTH+1,
		MOISTURE_GAUGE_Y + (GAUGE_HEIGHT)+1,
		al_map_rgba(32,32,32, 230)
	);
				
	al_draw_rectangle(
		GAUGE_X,
		MOISTURE_GAUGE_Y,
		GAUGE_X + GAUGE_WIDTH,
		MOISTURE_GAUGE_Y + GAUGE_HEIGHT,
		al_map_rgb(100,180,180),
		1.0f
	);
	al_draw_rectangle(
		GAUGE_X,
		DENSITY_GAUGE_Y,
		GAUGE_X + GAUGE_WIDTH,
		DENSITY_GAUGE_Y + GAUGE_HEIGHT,
		al_map_rgb(180,180,100),
		1.0f
	);
	int w = float(World::origin->moisture / 100.0f) * float(GAUGE_WIDTH -2);
	al_draw_filled_rectangle(
		GAUGE_X + 1,
		MOISTURE_GAUGE_Y + 1,
		GAUGE_X + w,
		MOISTURE_GAUGE_Y + (GAUGE_HEIGHT - 2),
		al_map_rgb( 30, 200, 255)
	);
	w  = float(World::origin->density / float(MAX_DENSITY-MIN_DENSITY)) * float(GAUGE_WIDTH -2);
	al_draw_filled_rectangle(
		GAUGE_X + 1,
		DENSITY_GAUGE_Y + 1,
		GAUGE_X + w,
		DENSITY_GAUGE_Y + (GAUGE_HEIGHT - 2),
		al_map_rgb(200, 200, 25)
	);
			
}

void World::drawMiniMap() {
	al_set_target_bitmap(Gpu::overlay.bitmap);
	int minimap_x = Gpu::width - (Sector::size+8);
	int minimap_y = Gpu::height - (Sector::size+8);
	for (int y = -8; y < Sector::size + 8; y++) {
	    for (int x = -8; x < Sector::size + 8; x++) {
			al_draw_pixel(
				minimap_x + x, 
				minimap_y + y, 
				World::origin->data->getColor(x, y, 200)
			);
		}
	}
	int px = int((Camera::absolute.x + (Gpu::width / 2)) / World::cell_size) % Sector::size;
	int py = int((Camera::absolute.y + (Gpu::height/ 2)) / World::cell_size) % Sector::size;
	al_draw_filled_circle(
		px + minimap_x,
		py + minimap_y,
		2,
		al_map_rgba(255, 0, 0, 200)
	);
	al_draw_filled_rectangle(
		px + minimap_x-1,
		py + minimap_y-1,
		px + minimap_x+1,
		py + minimap_y+1,
		al_map_rgba(255, 255, 255, 200)
	);
	al_draw_rectangle(
		minimap_x-8,
		minimap_y-8,
		minimap_x+Sector::size+8,
		minimap_y+Sector::size+8,
		al_map_rgba(64,48,32,200),
		1.0f
	);
}

void World::drawRadar(Sector *o ,int recursivity) {
	if (recursivity <= 0)return;
	if (o->sibiling[Direction::NORTH]) {
		o->sibiling[Direction::NORTH]->drawRadar();
		World::drawRadar(o->sibiling[Direction::NORTH], recursivity-1);
	}
	if (o->sibiling[Direction::SOUTH]) {
		o->sibiling[Direction::SOUTH]->drawRadar();
		World::drawRadar(o->sibiling[Direction::SOUTH], recursivity-1);
	}
	if (o->sibiling[Direction::RIGHT]) {
		o->sibiling[Direction::RIGHT]->drawRadar();
		World::drawRadar(o->sibiling[Direction::RIGHT], recursivity-1);
	}
	if (o->sibiling[Direction::LEFT]) {
		o->sibiling[Direction::LEFT]->drawRadar();
		World::drawRadar(o->sibiling[Direction::LEFT], recursivity-1);
	}
}

#include "environment.hpp"

void World::setOrigin(Sector *o) {
	World::origin = o;
	float rain = o->moisture;
	float rain_speed = 2.0f+(float(o->density)/float(MAX_DENSITY))*4.0f;
	Environment::setRain(rain, rain_speed);
}

void World::draw() {
	al_set_target_bitmap(Gpu::buffer.bitmap);
	
	World::origin->requestRedraw();
	World::origin->draw();

	World::drawRadar(World::origin, 4);

	OutOfCameraStatus status;
	if (World::origin->fullyOutOfCamera()) {
		Gpu::printCentered("<FULLY OUT OF CAMERA>", Gpu::width / 2, 0);		
	} else if (World::origin->partiallyOutOfCamera(status)) {
		Sector *sector;
		if (status.north) {
			sector = origin->getSibiling(NORTH);
			if (sector) {
				sector->getSibiling(LEFT);
				sector->getSibiling(RIGHT);
			}
			Gpu::printCentered("<OUT OF CAMERA: NORTH>", Gpu::width / 2, 0);
		} 
		if (status.south) {
			sector = origin->getSibiling(SOUTH);			
			if (sector) {
				sector->getSibiling(LEFT);
				sector->getSibiling(RIGHT);
			}
			Gpu::printCentered("<OUT OF CAMERA: SOUTH>", Gpu::width / 2, 0);
		}
		if (status.left) {
			sector = origin->getSibiling(LEFT);
			if (sector) {
				sector->getSibiling(NORTH);
				sector->getSibiling(SOUTH);
			}
			Gpu::printCentered("<OUT OF CAMERA: LEFT>", Gpu::width / 2, (status.north || status.south) ? 16 : 0);
		} 
		if (status.right) {
			sector = origin->getSibiling(RIGHT);
			if (sector) {
				sector->getSibiling(NORTH);
				sector->getSibiling(SOUTH);
			}
			Gpu::printCentered("<OUT OF CAMERA: RIGHT>", Gpu::width / 2, (status.north || status.south) ? 16 : 0);
		}
	}
}