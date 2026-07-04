#include "kore.hpp"
#include "gpu.hpp"
#include "sector.hpp"
#include "world.hpp"

int		Sector::size	= 64;
Sprite*	Sector::tileset = nullptr;
Animation Sector::animations[64];//<- NOT related with Sector::size
std::vector<ALLEGRO_BITMAP*> Sector::tile;

std::vector<Sector*> Sector::list;

Sector *Sector::find(int x, int y, int z) {
	for (std::vector<Sector*>::iterator it = Sector::list.begin(); it != Sector::list.end(); ++it) {
		Sector *s = *it;
		if (
			s->subjective_x == x &&
			s->subjective_y == y &&
			s->subjective_z == z 
		) return s;
	}
	return nullptr;
}

void Sector::add(Sector *sector) {
	Sector::list.push_back(sector);	
}

Sector *Sector::getSibiling(Direction direction) {
	
	int x = this->subjective_x + DIRECTION_DELTA_X[direction];
	int y = this->subjective_y + DIRECTION_DELTA_Y[direction];
	int z = this->subjective_z + DIRECTION_DELTA_Z[direction];
	
	if (!this->sibiling[ direction ]) {
		// Look first into indexed array
		Sector *existing_sector = Sector::find(x,y,z);
		if(existing_sector) {
			existing_sector->relink();//re-attach sector
			return existing_sector;
		}

		Sector *new_sector = new Sector(x,y,z);
	}
	return this->sibiling[direction];
}

void Sector::detach(Direction direction) {
	this->sibiling[direction] = nullptr;
}

Sector* Sector::get(Direction direction) {
	return this->sibiling[direction];
}

#include "terrain.hpp"


Sector::Sector(int x, int y, int z) {
	this->subjective_x = x;
	this->subjective_y = y;
	this->subjective_z = z;	

	__int64 *dat = new __int64[Sector::size * Sector::size/* ( * sector size ) = 3D */];
	__int64 *grs = new __int64[Sector::size * Sector::size/* ( * sector size ) = 3D */];
	this->data = new Layer("GROUND", Sector::size, Sector::size, 1.0, dat, Sector::size*Sector::size, x, y , z, this);
	this->grass= new Layer("GRASS" , Sector::size, Sector::size, 1.0, grs, Sector::size*Sector::size, x, y , z, this);
	this->grass->fill(-1);
	this->data->randomize();
	this->data->numbers = true;
	delete dat;
	delete grs;
	Sector::add(this);
	Terrain terrain(18);
	
	static int last_density = MIN_DENSITY + ((MAX_DENSITY - MIN_DENSITY)/2);
	static int last_moisture = 50;
	float ld = last_density  /3;	
	float lm = last_moisture /3;
	//this->density = MIN_DENSITY + int(TRUERAND(float(MAX_DENSITY - MIN_DENSITY)));
	this->density = ld + TRUERAND(MAX_DENSITY - ld);
	this->moisture= lm + TRUERAND(100.0f - lm); 
	
	last_density = this->density;
	
	this->relink();
	
	for (int terrains = 0; terrains < density; terrains++) {
		//terrain.createIsland(this->data, TRUERAND(Sector::size), TRUERAND(Sector::size), 0);
		terrain.createIsland(this->data, TRUERAND(Sector::size-1), TRUERAND(Sector::size-1), 1);
		int x = TRUERAND(Sector::size-3);
		int y = TRUERAND(Sector::size-3);
		terrain.createIsland(this->data, x,	   y, 2);
		terrain.createIsland(this->data, x+1,	   y, 2);
		terrain.createIsland(this->data, x,	 y+1, 2);
		terrain.createIsland(this->data, x+1,  y+1, 2);
		//terrain.createIsland(this->data, TRUERAND(Sector::size),TRUERAND(Sector::size), 2);
		//terrain.createIsland(this->data, TRUERAND(Sector::size),TRUERAND(Sector::size), 3);
	}	
	terrain.fixBorders(this->data);
	terrain.fill(this->data, 2, this->moisture);
	last_moisture= this->moisture;
	//terrain.randomize(this->data, 2, 2048);
	terrain.fillRestricted(this->data, 3,111);
	terrain.fillRestricted(this->data, 3,127, this->grass);
	terrain.fillSand(this->data, 0);
	this->grass->color = false;
	int r = this->data->r;
	int g = this->data->g;
	int b = this->data->b;
	this->setColor(r, g, b);
#define ALTER_COLOR  r += ((rand() % 3) - 1); g += ((rand() % 3) - 1); b += ((rand() % 3) - 1);
	// 0 - 1 = -1
	// 1 - 1 =  0 
	// 2 - 1 = +1
	if (this->sibiling[Direction::NORTH])this->sibiling[Direction::NORTH]->setColor(r, g, b); ALTER_COLOR;
	if (this->sibiling[Direction::SOUTH])this->sibiling[Direction::SOUTH]->setColor(r, g, b); ALTER_COLOR;
	if (this->sibiling[Direction::LEFT])this->sibiling[Direction::LEFT]->setColor(r, g, b); ALTER_COLOR;
	if (this->sibiling[Direction::RIGHT])this->sibiling[Direction::RIGHT]->setColor(r, g, b); ALTER_COLOR;
}

void Sector::setColor(int r, int g, int b) {
	this->data->r = r;
	this->data->g = g;
	this->data->b = b;
	this->grass->r = r;
	this->grass->g = g;
	this->grass->b = b;
}

void Sector::relink() {
	for (int direction = Direction::NORTH; direction < Direction::DIRECTION_MAX; direction++) {
		Sector *sibiling= Sector::find(
			this->subjective_x + DIRECTION_DELTA_X[ direction ],
			this->subjective_y + DIRECTION_DELTA_Y[ direction ],
			this->subjective_z + DIRECTION_DELTA_Z[ direction ]
		);
		if( sibiling ) {
			//re-attach sector
			sibiling->sibiling[OppositeDirection[ direction ]] = this;
			this->sibiling[ direction ] = sibiling;			
		}
	}
}
	
void Sector::destroy(bool cascade) {
	this->deleted = true;
	for (int i = 0; i < 6; i++) {
		Sector *t = this->sibiling[i];	
		if (cascade) {
			// Set mirror connection to null
			t->sibiling[OppositeDirection[(Direction)i]] = nullptr;
			if(!t->deleted)
				t->destroy(true);
		}
		this->sibiling[i] = nullptr;		
	}	
}

Sector::~Sector() {
	this->destroy(false);	
}

bool Sector::validMapIndex(int map_index) {
	return ((map_index >= 0) && (map_index < (Sector::size * Sector::size)));
}

#define RADAR_X (Gpu::width/2)
#define RADAR_Y (Gpu::height/8)
#define RADAR_S 4

void Sector::drawRadar() {
	int rx = RADAR_X + (this->subjective_x * RADAR_S)-(Camera::sector.x*4);
	int ry = RADAR_Y + (this->subjective_y * RADAR_S)-(Camera::sector.y*4);
		
	//al_draw_filled_rectangle(RADAR_X-6, RADAR_Y-6, RADAR_X + 6, RADAR_Y + 6, al_map_rgba(0,0,0,128));

	al_set_target_bitmap(Gpu::overlay.bitmap);
		al_draw_rectangle(rx, ry, rx + RADAR_S, ry + RADAR_S, al_map_rgba(!fullyOutOfCamera() ? 255 : 128, 180,0,255), 1.0f);
	al_set_target_bitmap(Gpu:: buffer.bitmap);
	if (this == World::origin) {
		al_draw_filled_rectangle(rx, ry, rx + RADAR_S, ry + RADAR_S, al_map_rgba(0, 255, 0, 96));
	} else {
		al_draw_filled_rectangle(rx, ry, rx + RADAR_S, ry + RADAR_S, al_map_rgba(128, 180, 0, 96));
	}		
}

void Sector::draw() {
	if ((!this->fullyOutOfCamera()) && (this->need_redraw)) {

		this->data->draw(
			World::cell_size + (this->subjective_x * Sector::size * World::cell_size),
			World::cell_size + (this->subjective_y * Sector::size * World::cell_size)
		);
		al_set_target_bitmap(Gpu::overlay.bitmap);
		this->grass->draw(
			World::cell_size + (this->subjective_x * Sector::size * World::cell_size),
			World::cell_size + (this->subjective_y * Sector::size * World::cell_size)
		);
		al_set_target_bitmap(Gpu::buffer.bitmap);
		
		this->need_redraw = false;

		this->drawRadar();
		
		int py = Gpu::height - 64;
		
		if (this->sibiling[Direction::NORTH])
			this->sibiling[Direction::NORTH]->draw();
		if (this->sibiling[Direction::LEFT])
			this->sibiling[Direction::LEFT]->draw();
		if (this->sibiling[Direction::SOUTH])
			this->sibiling[Direction::SOUTH]->draw();
		if (this->sibiling[Direction::RIGHT])
			this->sibiling[Direction::RIGHT]->draw();
		//if (this->sibiling[Direction::DOWN])
			//this->sibiling[Direction::DOWN]->draw();
	}
}

bool Sector::initialize() {
	Sector::tileset = new Sprite("atlas");
	int animation_index = 0;
	// Extract water animations
	Sector::animations[animation_index++] = Animation(*Sector::tileset, World::cell_size, World::cell_size, 0x0, 4, 0x7, 4, false);
	Sector::animations[animation_index++] = Animation(*Sector::tileset, World::cell_size, World::cell_size, 0x8, 4, 0xf, 4, false);
	// Extract water transitions	
	for (int y = 0; y < 4; y++) {
		for (int n = 0; n < 4; n++) {
			Sector::animations[animation_index] = Animation(
				*Sector::tileset,					
				World::cell_size,
				World::cell_size,					
				n*4,
				y,
				(n*4)+3,
				y,
				false
			);
			animation_index++;
		}
	}
	// Extract water-sand transitions
	for (int y = 5; y < 16; y++) {
		for (int n = 0; n < 2; n++) {
			Sector::animations[animation_index] = Animation(
				*Sector::tileset,					
				World::cell_size,
				World::cell_size,					
				n*8,
				y,
				(n*8)+7,
				y,
				false
			);
			animation_index++;
		}
	}
	int i = 0;
	// Extract static tileset
	for (int y = 16; y < 32; y++) {
		for (int x = 0; x < 16; x++) {
			ALLEGRO_BITMAP *bmp = al_create_bitmap(World::cell_size, World::cell_size);			
			al_set_target_bitmap(bmp);
			al_draw_bitmap_region(
				Sector::tileset->picture.bitmap,
				x * World::cell_size,
				y * World::cell_size,
				World::cell_size,
				World::cell_size,
				0,
				0,
				0);
			Sector::tile.push_back(bmp);
			i++;
		}
	}	
	delete Sector::tileset;
	Sector::tileset = nullptr;
	return true;
}

void Sector::deinitialize() {
	if (Sector::tileset) {
		delete Sector::tileset;
	}
}

bool Sector::fullyOutOfCamera() {
	int sector_size = Sector::size * World::cell_size;
	__int64 sx = this->subjective_x * sector_size;
	__int64 sy = this->subjective_y * sector_size;
	if ((sx + sector_size < Camera::x))return true;
	else if ((sx > Camera::x+Gpu::width))return true;
	if ((sy + sector_size < Camera::y))return true;
	else if ((sy > Camera::y+Gpu::height))return true;
	return false;
}


bool Sector::partiallyOutOfCamera(OutOfCameraStatus &status) {
	// This functions assumes a PREINITIALIZED status object.
	int sector_size = Sector::size * World::cell_size;
	__int64 sx = this->subjective_x * sector_size;
	__int64 sy = this->subjective_y * sector_size;
	
	if (sx + sector_size < Camera::x+Gpu::width) {
		status.right = true;
		if (sy + sector_size < Camera::y + Gpu::height) 
			status.south = true;
		if (sy > Camera::y) 
			status.north = true;
		if (sx > Camera::x)status.left = true;
		return true;
	} 
	if (sx > Camera::x) {
		status.left = true;
		if (sy + sector_size < Camera::y + Gpu::height) 
			status.south = true;
		if (sy > Camera::y) 
			status.north = true;
		if (sx + sector_size < Camera::x + Gpu::width) status.right = true;
		return true;
	} 
	if (sy + sector_size < Camera::y + Gpu::height) {
		status.south = true;
		if (sx + sector_size < Camera::x + Gpu::width) 
			status.right = true;
		if (sx > Camera::x) 
			status.left = true;
		if (sy > Camera::y) status.north = true;
		return true;
	} 
	if (sy > Camera::y) {
		status.north = true;
		if (sx + sector_size < Camera::x + Gpu::width) 
			status.right = true;
		if (sx > Camera::x) 
			status.left = true;
		if (sy + sector_size < Camera::y + Gpu::height) status.south = true;
		return true;
	}
	return false;
}

void Sector::update(double delta) {
	// process entities
	// move periferic entities to neighborhood when leaving this sector and entering a existing sibiling 
	// move periferic entities to neighborhood when leaving this sector and create sibiling where not existing

}

void Sector::requestRedraw() {
	if (this->need_redraw)return;
	this->need_redraw = true;
	if (this->sibiling[Direction::NORTH] && !this->sibiling[Direction::NORTH]->need_redraw)this->sibiling[Direction::NORTH]->requestRedraw();
	if (this->sibiling[Direction::SOUTH] && !this->sibiling[Direction::SOUTH]->need_redraw)this->sibiling[Direction::SOUTH]->requestRedraw();
	if (this->sibiling[Direction::LEFT]  && !this->sibiling[Direction::LEFT]->need_redraw)this->sibiling[Direction::LEFT]->requestRedraw();
	if (this->sibiling[Direction::RIGHT] && !this->sibiling[Direction::RIGHT]->need_redraw)this->sibiling[Direction::RIGHT]->requestRedraw();
	if (this->sibiling[Direction::UP]    && !this->sibiling[Direction::UP]->need_redraw)this->sibiling[Direction::UP]->requestRedraw();
	if (this->sibiling[Direction::DOWN]  && !this->sibiling[Direction::DOWN]->need_redraw)this->sibiling[Direction::DOWN]->requestRedraw();
}