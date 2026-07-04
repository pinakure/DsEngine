#ifndef CORE_POSITION
#define CORE_POSITION

class Position {
public:
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
	Position() {}
	Position(double x, double y, double z) {
		this->set(x,y,z);
	}
	void set(double x, double y, double z) {
		this->z = z;
		this->x = x;
		this->y = y;
	}
};

#endif