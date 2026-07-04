#ifndef CORE_DIRECTION
#define CORE_DIRECTION

typedef enum E_DirectionTypes {
	NORTH,
	SOUTH,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	DIRECTION_MAX,
}Direction;


const Direction OppositeDirection[6] = { 
	Direction::SOUTH, 
	Direction::NORTH,
	Direction::RIGHT,
	Direction::LEFT,
	Direction::DOWN,
	Direction::UP,
};

const Direction RotateDirection[6] = { 
	Direction::LEFT,
	Direction::RIGHT,
	Direction::SOUTH, 
	Direction::NORTH,
	Direction::UP,
	Direction::DOWN,
};

//									  N	 S  L  R  U	 D		
//									  T  T  F  T  P  N
const double DIRECTION_DELTA_X[6] = { 0, 0,-1, 1, 0, 0 };
const double DIRECTION_DELTA_Y[6] = {-1, 1, 0, 0, 0, 0 };
const double DIRECTION_DELTA_Z[6] = { 0, 0, 0, 0, 1,-1 };

#endif