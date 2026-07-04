#include "kore.hpp"
#include "world.hpp"
#include "environment.hpp"
#include "gpu.hpp"
#include "apu.hpp"
#include <string>
#include <ctime>

int     Kore::runlevel  = 0;
bool	Kore::running	= false;
double	Kore::timescale	= KORE_DEFAULT_TIMESCALE;
double	Kore::clock_frequency = 0.0;
Mouse	Kore::mouse;
ALLEGRO_EVENT_QUEUE *Kore::queue = nullptr;

LONGLONG Kore::seed = 1337;

bool Kore::initializeSeed() {
	if(Kore::seed<0){
		srand(int(std::time(NULL)));
	} else {
		srand(Kore::seed);
	}
	runlevel++;
	return true;
}

bool Kore::initializeInput() {
	if (!al_install_keyboard()) return false;
	Kore::runlevel++;
	if (!al_install_mouse()) return false;
	Kore::runlevel++;
	return true;
}

bool Kore::initializeEvents() {
	try {
		Kore::queue = al_create_event_queue();
		if (!Kore::queue) return false;
		al_register_event_source(Kore::queue, al_get_display_event_source(Gpu::screen));
		al_register_event_source(Kore::queue, al_get_keyboard_event_source());
		al_register_event_source(Kore::queue, al_get_mouse_event_source());
		Kore::runlevel++;
		return true;
	} catch (int x) {
		x;
		return false;
	}
}


bool Kore::initialize(bool(*callback)()) {
	if (Kore::initializeSeed() //runlevel 1
	&& Gpu::initialize(683,384)//runlevel 2
	&& Kore::initializeInput() //runlevel 4
	&& Kore::initializeEvents()//runlevel 5
	&& Apu::initialize()       //runlevel 6
	&& World::initialize()     //runlevel 7
	&& Environment::initialize()//runlevel 8
	) {
		if (!callback()) return false;
		Kore::runlevel++;//runlevel 9
		Kore::running = true;
		return true;
	}
	return false;
}
	
void Kore::shutdown(void(*callback)()) {
	switch (Kore::runlevel) {
	    default:
	    case 9: callback();	
		case 8: Environment::shutdown();
		case 7: World::shutdown();
		case 6: Apu::shutdown();
		case 5: al_destroy_event_queue(Kore::queue); Kore::queue = nullptr;
		case 4: al_uninstall_mouse();
		case 3: al_uninstall_keyboard();
		case 2: Gpu::shutdown();
		case 1: Kore::running = false;
	}
	Kore::runlevel = 0;
}
void Kore::processEvents() {
	ALLEGRO_EVENT event;
	al_wait_for_event(queue, &event);
	switch(event.type){
		case ALLEGRO_EVENT_TIMER:
			Gpu::redraw = true;
			return;
		case ALLEGRO_EVENT_KEY_UP:
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			running = false;
			return;
		case ALLEGRO_EVENT_MOUSE_AXES:
			Kore::mouse.x  = event.mouse.x;
			Kore::mouse.y  = event.mouse.y;
			Kore::mouse.z  = event.mouse.z;
			Kore::mouse.w  = event.mouse.w;
			Kore::mouse.dx = event.mouse.dx;
			Kore::mouse.dy = event.mouse.dy;
			Kore::mouse.dz = event.mouse.dz;
			Kore::mouse.dw = event.mouse.dw;
			return;
	}	
}

void Kore::loop(void(*logic_callback)(double), void(*draw_callback)(bool)) {
	ALLEGRO_TIMER *timer = NULL;
	timer = al_create_timer(1.0 / Gpu::frame_rate);

	al_register_event_source(queue, al_get_timer_event_source(timer));
	al_start_timer(timer);
 
	double initial_delta = 2.0;
	double delta = initial_delta;
	
	al_hide_mouse_cursor(Gpu::screen);
	
	while (Kore::running) {
		Kore::processEvents();
		if(Gpu::redraw && al_is_event_queue_empty(queue)) {
			Gpu::redraw = false;
			World::update(delta);
			Environment::update(delta);
			logic_callback(delta);
			World::draw();			
			
			// Draw player lower 
			draw_callback(true);	
			
			
		// DRAW OVER THIS LINE TO DRAW BELOW FOREGROUND SEMITRANSPARENT MAP ELEMENTS 
			
			// Copy overlay to buffer and clean overlay
			Gpu::preflip();			
			
			draw_callback(false);//Draw player upper
		
		// DRAW BELOW THIS LINE TO DRAW OVER MAP ELEMENTS 
		
			Environment::draw();
			World::drawMiniMap();
			World::drawSectorInfo();
			Kore::drawDebugInfo();

		// SEND PICTURE TO MONITOR 
			Gpu::flip();

			//al_save_bitmap("out.png", al_get_target_bitmap());
			Gpu::clear();
			Camera::update();
		}
	}
	al_destroy_timer(timer);
}


void dumpInteger64Var(__int64 var, const char *name, int x, int y, int align=ALLEGRO_ALIGN_LEFT) {
	if (align == ALLEGRO_ALIGN_CENTRE) {
		Gpu::printCentered((std::string(name) + std::string(" : ") + std::to_string(var)).c_str(), x, y);
	} else if (align == ALLEGRO_ALIGN_RIGHT) {
		Gpu::printRight((std::string(name) + std::string(" : ") + std::to_string(var)).c_str(), x, y);
	}  else Gpu::print((std::string(name)+std::string(" : ")+std::to_string(var)).c_str(), x, y);
}

void Kore::drawDebugInfo() {
	//
	// HANDLE MOUSE
	//
	int sectorsz = World::cell_size * Sector::size;
	int mx = (int(Camera::x-World::origin->subjective_x * sectorsz) % sectorsz) / World::cell_size;
	int my = (int(Camera::y-World::origin->subjective_y * sectorsz) % sectorsz) / World::cell_size;
	// Adjust offset tile level
	mx += (((Kore::mouse.x/2) - (-(int(Camera::x) % World::cell_size) ) ) / World::cell_size);
	my += (((Kore::mouse.y/2) - (-(int(Camera::y) % World::cell_size) ) ) / World::cell_size);
	World::origin->data->setHighlight(mx, my);


	dumpInteger64Var(Camera::x			, "CAM-X"		, Gpu::width - 1,  0, ALLEGRO_ALIGN_RIGHT);
	dumpInteger64Var(Camera::y			, "CAM-Y"		, Gpu::width - 1, 16, ALLEGRO_ALIGN_RIGHT);	
	dumpInteger64Var(Kore::mouse.x		, "MOUSE-X"		, Gpu::width - 1, 32, ALLEGRO_ALIGN_RIGHT);
	dumpInteger64Var(Kore::mouse.y		, "MOUSE-Y"		, Gpu::width - 1, 48, ALLEGRO_ALIGN_RIGHT);	
	dumpInteger64Var( mx, "MAP-X"		, Gpu::width - 1, 64, ALLEGRO_ALIGN_RIGHT);
	dumpInteger64Var( my, "MAP-Y"		, Gpu::width - 1, 80, ALLEGRO_ALIGN_RIGHT);	
	dumpInteger64Var(Camera::sector.x-32767	, "CAM-SECTOR-X", Gpu::width - 1, 96, ALLEGRO_ALIGN_RIGHT);
	dumpInteger64Var(Camera::sector.y-32767	, "CAM-SECTOR-Y", Gpu::width - 1,112, ALLEGRO_ALIGN_RIGHT);
	
	Gpu::printCentered(World::origin->sibiling[Direction::NORTH] ? " O " : " | ", Gpu::width / 2, 64);
	Gpu::printCentered(
		World::origin->sibiling[Direction::RIGHT] && World::origin->sibiling[Direction::LEFT] ? 
		"OxO" : World::origin->sibiling[Direction::RIGHT] ? 
		"-xO" : World::origin->sibiling[Direction::LEFT] ? 
		"Ox-" : 
		"-x-", 
		Gpu::width / 2, 
		80
	);
	Gpu::printCentered(World::origin->sibiling[Direction::SOUTH] ? " O " : " | ", Gpu::width / 2, 96);
	
}
