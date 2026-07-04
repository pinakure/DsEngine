#include "gauge.hpp"
#include "kore.hpp"
#include "gpu.hpp"

std::vector<Gauge*> Gauge::instance;

Animation Gauge::bubbles;
Sprite	  Gauge::bottle;

bool Gauge::initialize() {
	// Load bubbles
	try {
		Sprite bubble_spritesheet("bubbles");
		Gauge::bubbles = Animation(bubble_spritesheet, 8, 64, 0, 0, 39, 0, false);
		Gauge::bubbles.bidirectional = false;
		Gauge::bottle = Sprite("bottle");
		return true;
	} catch (int X){X;
		return false;
	}
}

Gauge::Gauge(int x, int y, double min, double max, double value, int r, int g, int b) {
	this->x = x;
	this->y = y-64;
	this->min = min;
	this->max = max;
	this->value = value;
	this->setColor(r, g, b);
}

void destroy(Gauge *g) {
	try {
		Gauge::instance.erase(std::find(Gauge::instance.begin(), Gauge::instance.end(), g));		
	} catch (int X) { X;
		return;
	}
	delete g;	
}

Gauge *Gauge::create(int x, int y, double min, double max, double value, int r, int g, int b) {
	Gauge *gauge = new Gauge(x, y, min, max, value, r, g, b);
	Gauge::instance.push_back(gauge);
	return gauge;
}

void Gauge::globalUpdate(double delta) {
	//animate bubbles	
	Gauge::bubbles.run(0.25f * float(delta));
	
	// Update each instance
	for (std::vector<Gauge*>::iterator it = Gauge::instance.begin(); it < Gauge::instance.end(); it++) {
		(*it)->update(delta);
	}
}

void Gauge::globalDraw() {
	for (std::vector<Gauge*>::iterator it = Gauge::instance.begin(); it < Gauge::instance.end(); it++) {
		(*it)->draw();
	}
}

void Gauge::update(double delta) {
	// update local instance
	this->value += delta * (double(TRUERAND(5))/10.0);
	if (this->value > this->max) {
		//DEBUG
		int t = this->r;
		this->r = this->g;
		this->g = this->b;
		this->b = t;
		this->value = this->min;
	}
	if (this->value < this->min)this->value = this->min;
	this->height = (
		(this->value - this->min) 
		/ 
		(this->max - this->min)
	) * 64;
}

void Gauge::setColor(int r, int g, int b) {
	this->r = r;
	this->g = g;
	this->b = b;
}

void Gauge::draw() {
	al_set_target_bitmap(Gpu::overlay.bitmap);
	
	al_draw_bitmap_region(
		Gauge::bubbles.frame[Gauge::bubbles.current_frame],
		0,
		0,
		8,
		this->height,
		this->x+4,
		this->y+77-height,
		0
	);

	al_draw_filled_rectangle(
		this->x + 4,
		this->y + 76-height,
		this->x + 11,
		this->y + 77,
		al_map_rgba(
			this->r, 
			this->g, 
			this->b, 
			64
		)
	);

	al_draw_bitmap(
		Gauge::bottle.picture.bitmap,
		this->x,
		this->y,
		0
	);
}