#ifndef CORE_PLAYER
#define CORE_PLAYER


#include "surface.hpp"
#include "animation.hpp"
#include "direction.hpp"
#include "position.hpp"
#include "player_status.hpp"
#include "sector.hpp"
#include "material.hpp"

#define FORCE_PLAYER_SOUTH_RIGHT
#define PLAYER_WANDER

class Player {
private:
public:
	Sector		*sector = nullptr;
	Direction	direction;
	Animation	animation[4];
	Animation	lower_animation[4];
	int			wander_counter = 0;
	double		x=0;
	double		y=0;
	double		z=0;
	double		speed = 0.6f;
	double      walk_timer = 0.0f;
	Position	theoric_sector;
	Material	*ground = nullptr;
	Material	*sibiling[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	Player(int x = 0, int y = 0, Direction direction=UP, int width=32, int height=32);
	void draw(Surface &destination, bool lower=false);
	void update(double delta);
	void determineGround();
	void wander(double delta);
	void makeFootStepNoise();
};

#endif