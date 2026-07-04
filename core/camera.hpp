#ifndef CORE_CAMERA
#define CORE_CAMERA

#include "position.hpp"

class Camera {
private:
public:
	static Position sector;
	static Position relative;
	static Position absolute;
	static double x;
	static double y;
	static double z;
	static double target_x;
	static double target_y;
	static double target_z;
	static void set(double x, double y, double z);
	static void move(double x, double y, double z);
	static void transformToRelative(Position &absolute);
	static void transformToSectorIndex(Position &absolute);
	static void transformToMapIndex(Position &absolute);
	static void update();
};

typedef struct E_OutOfCameraStatus {
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool north = false;
	bool south = false;
}OutOfCameraStatus;

#endif