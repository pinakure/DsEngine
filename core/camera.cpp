#include "camera.hpp"
#include "world.hpp"
#include "sector.hpp"
#define max(a,b) (a>b?a:b)
#define min(a,b) (a>b?b:a)
double Camera::x = 0.0;
double Camera::y = 0.0;
double Camera::z = 0.0;
double Camera::target_x = -1.0;
double Camera::target_y = -1.0;
double Camera::target_z = -1.0;
Position Camera::sector(0,0,0);
Position Camera::relative(0,0,0);
Position Camera::absolute(0,0,0);

void Camera::transformToRelative(Position &absolute) {
	absolute.set(
		int(absolute.x) % (World::cell_size * Sector::size),
		int(absolute.y) % (World::cell_size * Sector::size),
		int(absolute.z) % (World::cell_size * Sector::size)
	);
}

void Camera::transformToSectorIndex(Position &absolute) {
	absolute.set(
		int(absolute.x) / (World::cell_size * Sector::size),
		int(absolute.y) / (World::cell_size * Sector::size),
		int(absolute.z) / (World::cell_size * Sector::size)
	);
}

void Camera::transformToMapIndex(Position &absolute) {
	Camera::transformToRelative(absolute);
	absolute.set(
		int(absolute.x) / (World::cell_size),
		int(absolute.y) / (World::cell_size),
		int(absolute.z) / (World::cell_size)
	);
}

void Camera::set(double x, double y, double z) {
	Camera::target_x = x;
	Camera::target_y = y;
	Camera::target_z = z;
}

void Camera::update() {
	// Exit if no target is set
	if((Camera::target_x == -1.0)
	|| (Camera::target_y == -1.0)
	|| (Camera::target_y == -1.0)
	) {
		return;
	}
	
	// Follow target only if it is outside radius around target
	float speed_x = max(Camera::target_x, Camera::x)
				   - min(Camera::target_x, Camera::x);
	float speed_y = max(Camera::target_y, Camera::y)
				   - min(Camera::target_y, Camera::y);
	float speed_z = max(Camera::target_z, Camera::z)
				   - min(Camera::target_z, Camera::z);
	speed_x /= 15;
	speed_y /= 4;
	speed_z /= 15;
	double radius = 8.0;	
	if (Camera::x < Camera::target_x-radius) Camera::x += speed_x;
	if (Camera::y < Camera::target_y-radius) Camera::y += speed_y;
	if (Camera::z < Camera::target_z-radius) Camera::z += speed_z;
	if (Camera::x > Camera::target_x+radius) Camera::x -= speed_x;
	if (Camera::y > Camera::target_y+radius) Camera::y -= speed_y;
	if (Camera::z > Camera::target_z+radius) Camera::z -= speed_z;
	Camera::move(Camera::x, Camera::y, Camera::z);
	// Check if camera is already inside radius around target
	if((Camera::x > Camera::target_x-radius)
	&& (Camera::x < Camera::target_x+radius)
	&& (Camera::y > Camera::target_y-radius)
	&& (Camera::y < Camera::target_y+radius)
	&& (Camera::z > Camera::target_z-radius)
	&& (Camera::z < Camera::target_z+radius)
	){ 
		// Indeed, reached target area
		Camera::target_x = -1.0;
		Camera::target_y = -1.0;
		Camera::target_z = -1.0;
	}	
}

void Camera::move(double x, double y, double z) {
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (z < 0) z = 0;
	Camera::x = x;
	Camera::y = y;
	Camera::z = z;
	Camera::absolute.set(
		Camera::x,
		Camera::y,
		Camera::z
	);
	Camera::relative.set(
		Camera::x / World::cell_size,
		Camera::y / World::cell_size,
		Camera::z / World::cell_size
	);
	Camera::sector.set(
		Camera::relative.x / Sector::size,
		Camera::relative.y / Sector::size,
		Camera::relative.z / Sector::size
	);
}
	
