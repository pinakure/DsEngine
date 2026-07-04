#include "environment.hpp"
#include "world.hpp"
#include "gpu.hpp"
#include "kore.hpp"

ALLEGRO_BITMAP*			Environment::fog = nullptr;
std::vector<Raindrop>	Environment::raindrop;
std::vector<Splash>	    Environment::splash;
int                     Environment::rain = 100;
double                  Environment::rain_speed = 4.0;

Raindrop::Raindrop(
	__int64 x,
	__int64 y,
	__int64 z
) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->last_x = x;
	this->last_y = y;
	this->last_z = z;
}

void Raindrop::draw() {
	al_set_target_bitmap(Gpu::buffer.bitmap);
	al_draw_line(
		this->x - Camera::x + (this->last_z),
		this->y - Camera::y - (this->last_z*3.5),
		this->x - Camera::x + (this->z),
		this->y - Camera::y - (this->z*3.5),
		al_map_rgba(0, 80, 100, 32),
		1.0f
	);
}

bool Environment::initializeRain() {
	__int64 rx=0;
	__int64 ry=0;
	__int64 rz=0;
	for (int i = 0; i < Environment::rain*5; i++) {
		rx = TRUERAND(Gpu::width) + Camera::x;
		ry = TRUERAND(Gpu::height) + Camera::y;
		rz = TRUERAND(255);
	    Environment::raindrop.push_back(Raindrop(rx, ry, rz));
	}
	return true;
}

bool Environment::initializeFog() {
	float global_alpha = 128.0f;
	try {
		Environment::fog = Gpu::createImage(Gpu::width, Gpu::height);
		al_set_target_bitmap(Environment::fog);
		al_clear_to_color(al_map_rgba(0, 0, 0, 0));
		for (int t = 0; t < World::cell_size; t++) {
			al_draw_rectangle(
				t, 
				t, 
				(Gpu::width-1)-(t), 
				(Gpu::height-1)-(t), 
				al_map_rgba(0,0,0, global_alpha), 
				1.0f
			);
		}
		for (int alpha = 0; alpha < World::cell_size; alpha++) {
			al_draw_rectangle(
				alpha+World::cell_size, 
				alpha+World::cell_size, 
				(Gpu::width-1)-(alpha+World::cell_size), 
				(Gpu::height-1)-(alpha+World::cell_size), 
				al_map_rgba(0,0,0, global_alpha - ((alpha/float(World::cell_size))*global_alpha)), 
				1.0f
			);
		}
	    return true;
	} catch (int x) { x;
	    return false;
	}
}

bool Environment::initialize() {
	try {
		if (initializeFog()
			//&& initializeRain()
			&& true) {
			Kore::runlevel++;
			return true;
		}
	} catch (int X){X;}
	return false;
}

void Environment::shutdown() {
	Kore::runlevel--;
}

void Environment::drawFog() {
	al_draw_bitmap(fog, 0, 0, 0);
}


void Environment::drawRain() {
	for (std::vector<Raindrop>::iterator it = Environment::raindrop.begin();
		it < Environment::raindrop.end();
		it++) {
		(*it).draw();
	}
	Environment::drawSplash();
}

void Environment::drawSplash() {
	for (std::vector<Splash>::iterator it = Environment::splash.begin();
		it < Environment::splash.end();
		it++)
	{
		Splash *s = &(*it);
		s->draw();
	}
}


void Splash::update(double delta) {
	if(this->q < 15.0 - TRUERAND(5)) {
		this->q += 0.5*delta;
	} else {
		this->done = true;
	}
}

void Splash::draw() {
	al_draw_circle(
		float(this->x - Camera::x),
		float(this->y - Camera::y),
		float(this->q)/3.0f,
		al_map_rgba(0, 48, 100, 64.0-double((float(this->q)/(15.0f))*64.0f)),
		1.0f
	);
}

void Environment::setRain(int q, int speed){
	Environment::raindrop = std::vector<Raindrop>();
	int dice = 0;
	if (q <= 45) dice = TRUERAND(8)/4;
	else if (q <= 55) dice = TRUERAND(4)/2;
	else if (q <= 70) dice = TRUERAND(2);
	if (dice > 0) return;
	//Environment::rain_speed = speed;
	Environment::initializeRain();
}

void Environment::updateSplash(double delta) {
	std::vector<Splash> alive_list;
	for (std::vector<Splash>::iterator it = Environment::splash.begin();
		it < Environment::splash.end();
		it++)
	{
		(*it).update(delta);
		if (!(*it).done) {
			alive_list.push_back(*it);
		}		
	}
	Environment::splash = alive_list;
}

#include "apu.hpp"

void Environment::updateRain(double delta) {
	for (std::vector<Raindrop>::iterator it = Environment::raindrop.begin();
		it < Environment::raindrop.end();
		it++) {
		if ((*it).z > 0.0){
			 (*it).last_z = (*it).z ;
			 (*it).z-= Environment::rain_speed*delta;
		} else {
			//Apu::play(SoundEffect::SFX_RAINDROP_0);
			Environment::splash.push_back(Splash((*it).x, (*it).y));
	        (*it).x = TRUERAND(Gpu::width) + Camera::x;
		    (*it).y = TRUERAND(Gpu::height) + Camera::y;
			(*it).z = 255+TRUERAND(255);
			(*it).last_x = (*it).x;
			(*it).last_y = (*it).y;
			(*it).last_z = (*it).z;		    
		}
	}
	Environment::updateSplash(delta);
}


void Environment::draw() {
	Environment::drawRain();
	Environment::drawFog();
}

void Environment::update(double delta) {
	Environment::updateRain(delta);
}