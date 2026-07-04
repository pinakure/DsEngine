#ifndef CORE_GAUGE
#define CORE_GAUGE


#include <vector>
#include "animation.hpp"

class Gauge {
private:
public:
	int r=0;
	int g=0;
	int b=128;
	int x;
	int y;
	int height;
	double value;
	double max;
	double min;

	static std::vector<Gauge*> instance;

	Gauge(int x, int y, double min=0, double max=1, double value=0,int r=0, int g=128, int b=0);;
	static Animation bubbles;
	static Sprite	 bottle;
	static bool initialize();
	
	static void globalUpdate(double delta);
	static void globalDraw();

	static Gauge *create(int x, int y, double min=0, double max=1, double value=0, int r=0, int g=128, int b=0);
	static void destroy(Gauge *g);

	void update(double delta);
	void draw();
	void setColor(int r, int g, int b);
};

#endif