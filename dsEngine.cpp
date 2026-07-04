#include "core/gpu.hpp"
#include "core/kore.hpp"
#include "core/player.hpp"
#include "core/world.hpp"

#define WORLD_SIZE 65535

Player *player;

#include "core/gauge.hpp"

Gauge *magic_gauge;
Gauge *stamina_gauge;
Gauge *vitality_gauge;

bool game_init() {
	if (!Gauge::initialize()) return false;
	int mon = (Sector::size * (WORLD_SIZE/2) * World::cell_size)+(Sector::size/2);
	player = new Player(mon, mon, Direction::SOUTH, 64, 64);
	World::origin = new Sector(
		mon/(Sector::size*World::cell_size), 
		mon/(Sector::size*World::cell_size), 
		mon/(Sector::size*World::cell_size)
	);
	player->sector = World::origin;

	vitality_gauge  = Gauge::create(  4, Gpu::height - 40, 0, 100, 0, 128,  0,   0);
	magic_gauge		= Gauge::create( 20, Gpu::height - 40, 0, 100, 0,  64,  0, 128);
	stamina_gauge	= Gauge::create( 36, Gpu::height - 40, 0, 100, 0, 128, 64,   0);
	return true;
}

void game_deinit() {

}

void game_draw_loop(bool lower) {
	Gauge::globalDraw();
	player->draw(!lower ? Gpu::overlay : Gpu::buffer, lower);
}

void game_logic_loop(double delta) {
	Gauge::globalUpdate(delta);
	player->update(delta);		
}

int main(int argc, char **argv){
	if (Kore::initialize(game_init)) {
		Kore::loop(game_logic_loop, game_draw_loop);
		Kore::shutdown(game_deinit);
		return 0;
	}
	return 1;
}