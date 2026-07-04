#include "player.hpp"
#include "camera.hpp"
#include "gpu.hpp"
#include "kore.hpp"
#include "world.hpp"

extern void dumpInteger64Var(__int64 var, const char *name, int x, int y, int align = ALLEGRO_ALIGN_LEFT);



Player::Player(int x, int y, Direction direction, int width, int height) {
	this->x = x;
	this->y = y;
	this->direction = direction;
	Sprite down("down");
	Sprite left("left");
	Sprite right("right");
	Sprite up("up");
	Sprite down_lower("down_lower");
	Sprite left_lower("left_lower");
	Sprite right_lower("right_lower");
	Sprite up_lower("up_lower");
	this->animation[Direction::NORTH] = Animation(up   , width, height, 0, 0, 3, 14);
	this->animation[Direction::SOUTH] = Animation(down , width, height, 0, 0, 3, 14);
	this->animation[Direction::RIGHT] = Animation(right, width, height, 0, 0, 3, 14);
	this->animation[Direction::LEFT ] = Animation(left , width, height, 0, 0, 3, 14);
	this->lower_animation[Direction::NORTH] = Animation(up_lower   , width, height, 0, 0, 3, 14);
	this->lower_animation[Direction::SOUTH] = Animation(down_lower , width, height, 0, 0, 3, 14);
	this->lower_animation[Direction::RIGHT] = Animation(right_lower, width, height, 0, 0, 3, 14);
	this->lower_animation[Direction::LEFT ] = Animation(left_lower , width, height, 0, 0, 3, 14);
	this->animation[Direction::NORTH].bidirectional = false;
	this->animation[Direction::SOUTH].bidirectional = false; 
	this->animation[Direction::RIGHT].bidirectional = false;
	this->animation[Direction::LEFT ].bidirectional = false;
	this->lower_animation[Direction::NORTH].bidirectional = false;
	this->lower_animation[Direction::SOUTH].bidirectional = false; 
	this->lower_animation[Direction::RIGHT].bidirectional = false;
	this->lower_animation[Direction::LEFT ].bidirectional = false;
	this->sector = World::origin;
	World::setOrigin(this->sector);
	this->determineGround();
	while (this->ground->type <= Terrains::SHORE){
		this->x++;
		this->y++;
		this->determineGround();	
	}
	Camera::move(this->x, this->y, this->z);
}

void Player::determineGround() {	
	int sector_size = Sector::size*World::cell_size;
	int px = int(((int(this->x)+(Gpu::width /2))) % sector_size) / World::cell_size;
	int py = int(((int(this->y)+(Gpu::height/2))+(World::cell_size/2)) % sector_size) / World::cell_size;
	//this->sector->data->highlight(px, py);
	this->ground = &Material::preset[
		Material::toPreset(
			Material::identify(
				this->sector->data->get(px,py)
			)
		)
	];
	for(Direction i = Direction::NORTH; i < Direction::DIRECTION_MAX; i=(Direction)((int)i+1)) {
		this->sibiling[i] = &Material::preset[
			Material::toPreset(
				Material::identify(
					this->sector->data->get(px+DIRECTION_DELTA_X[i], py+DIRECTION_DELTA_Y[i])
				)
			)
		];
	}
}

#include "apu.hpp"

void Player::makeFootStepNoise() {
	int dice = TRUERAND(4);
	this->walk_timer = 0.0;
	switch (this->ground->type) {
	    case Terrains::SEA:
		case Terrains::SHORE:
			switch (dice) {
			    case 0: Apu::play(SoundEffect::SFX_FOOTSTEP_B);
			    case 1: Apu::play(SoundEffect::SFX_FOOTSTEP_C);
			    case 2: Apu::play(SoundEffect::SFX_FOOTSTEP_D);
			    case 3: Apu::play(SoundEffect::SFX_FOOTSTEP_C);
			} dice = TRUERAND(4);
	    case Terrains::SAND:
	    case Terrains::MOUNTAIN:
			switch (dice) {
			    case 0: Apu::play(SoundEffect::SFX_FOOTSTEP_0);
			    case 1: Apu::play(SoundEffect::SFX_FOOTSTEP_1);
			    case 2: Apu::play(SoundEffect::SFX_FOOTSTEP_2);
			    case 3: Apu::play(SoundEffect::SFX_FOOTSTEP_3);
			} return;
	    case Terrains::TALL_GRASS:
			switch (dice) {
			    case 0: Apu::play(SoundEffect::SFX_FOOTSTEP_4);
			    case 1: Apu::play(SoundEffect::SFX_FOOTSTEP_5);
			    case 2: Apu::play(SoundEffect::SFX_FOOTSTEP_6);
			    case 3: Apu::play(SoundEffect::SFX_FOOTSTEP_7);
			} return;
	    case Terrains::DEEP_GRASS:
	    case Terrains::GRASS:
			switch (dice) {
			    case 0: Apu::play(SoundEffect::SFX_FOOTSTEP_7);
			    case 1: Apu::play(SoundEffect::SFX_FOOTSTEP_8);
			    case 2: Apu::play(SoundEffect::SFX_FOOTSTEP_9);
			    case 3: Apu::play(SoundEffect::SFX_FOOTSTEP_A);
			} return;
	
	}
}	

void Player::wander(double delta) {
	this->wander_counter -= delta;
	if (this->wander_counter < 0) {
		this->wander_counter = TRUERAND(1512.0);
		this->direction = (Direction) int(TRUERAND(4));
	}
	#ifdef FORCE_PLAYER_SOUTH_RIGHT
		//if (direction == LEFT)direction = RIGHT;
		//if (direction == NORTH)direction = SOUTH;
	#endif
}

void Player::update(double delta) {
	
	//Guess which material is the player on and set corresponding animation (underwater or ground)
	this->determineGround();

	// ----------------------------------------------
	// DEBUGGING - Move player around automatically (remove!!)
	#ifdef PLAYER_WANDER
	this->wander(delta);
	#endif
	// ----------------------------------------------
	
	// Animate
	this->animation[this->direction].run(delta * 0.25);
	this->lower_animation[this->direction].run(delta * 0.25);
	
	// Move
	Material *m = this->sibiling[this->direction];
	if ((m != nullptr) && (m->walkable)) this->x += delta * this->speed * DIRECTION_DELTA_X[this->direction]; else this->direction = RotateDirection[this->direction];
	m = this->sibiling[this->direction];
	if( (m != nullptr) && (m->walkable)) this->y += delta * this->speed * DIRECTION_DELTA_Y[this->direction]; else this->direction = RotateDirection[this->direction];
	if (this->x <= 0)this->direction = Direction::RIGHT;
	if (this->y <= 0)this->direction = Direction::SOUTH;
	
	// Update walk noise
	this->walk_timer += this->speed;
	if (this->walk_timer >= 10.0) {
		this->makeFootStepNoise();
	}

	// Update relative sector position
	int sector_size = Sector::size * World::cell_size;
	this->theoric_sector.set(
		(this->x + (Gpu::width / 2)) / sector_size,
		(this->y + (Gpu::height / 2)) / sector_size,
		1
	);
	
	// Teleport to next sector if entering 
	if (this->sector->subjective_x < int(this->theoric_sector.x)) 
		World::setOrigin(World::origin->getSibiling(Direction::RIGHT));
	else if (this->sector->subjective_x > int(this->theoric_sector.x)) 
		World::setOrigin(World::origin->getSibiling(Direction::LEFT));
	else if (this->sector->subjective_y < int(this->theoric_sector.y))
		World::setOrigin(World::origin->getSibiling(Direction::SOUTH));
	else if (this->sector->subjective_y > int(this->theoric_sector.y))
		World::setOrigin(World::origin->getSibiling(Direction::NORTH));
	
	// Update current sector ref
	this->sector = World::origin;
	// ----------------------------------------------------
	// DEBUGGING
	// Update camera to point on this player (remove!!)
	Camera::set(this->x, this->y, this->z);
	// ----------------------------------------------------
}

void Player::draw(Surface &destination, bool lower) {
	Gpu::printCentered("Player", Gpu::width / 2, (Gpu::height/2)-48);
	Gpu::print(
		(	"(" +
			std::to_string(this->sector->subjective_x) + "," +
			std::to_string(this->sector->subjective_y) + "," +
			std::to_string(this->sector->subjective_z) + ")"
		).c_str(), 
		0, 
		0
	);
	
	Gpu::print(this->ground->toString(), 56, Gpu::height - 44);
	
	Animation *a = nullptr;
	if (lower)	a = this->lower_animation;
	else		a = this->animation;
	
	// Draw the player
	int z = this->z-(int(Camera::z) % World::cell_size);
	int x = this->x-(int(Camera::x) % World::cell_size);
	int y = this->y-(int(Camera::y) % World::cell_size);
	
	int fsize = a->frame.size();
	a[this->direction].draw(
		destination,
		(this->x - Camera::x) + ((Gpu::width  / 2) - (a->width / 2)),
		(this->y - Camera::y) + ((Gpu::height / 2) - (a->height/ 2))
	);
}