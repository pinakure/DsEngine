#ifndef CORE_ENVIRONMENT
#define CORE_ENVIRONMENT

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

class Splash {
private:
public:
	int x;
	int y;
	int q = 0;
	bool done = false;
	Splash(int x, int y) { this->x = x;this->y = y; }
	void update(double delta=1.0);
	void draw();
};

class Raindrop {
private:
public:
	double x;
	double y;
	double z;
    double last_x;
	double last_y;
	double last_z;

	Raindrop(__int64 x, __int64 y, __int64 z);
	void draw();
};

#include <vector>

class Environment {
private:
	static std::vector<Raindrop> raindrop;
	static std::vector<Splash>   splash;

	
public:
	static void setRain(int q, int speed);
	static bool initializeRain();
	static bool initializeFog();
	static void updateRain(double delta);
	static void updateSplash(double delta);
	static void drawFog();
	static void drawRain();
	static void drawSplash();
	static double rain_speed;
	
	static ALLEGRO_BITMAP *fog;
	static int  rain;
	static bool initialize();
	static void draw();
	static void update(double delta);
	static void shutdown();
	
};

#endif