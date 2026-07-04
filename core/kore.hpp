#ifndef CORE_KORE
#define CORE_KORE

#include <allegro5/allegro.h>

#define KORE_DEFAULT_TIMESCALE 1.0f

typedef struct s_Mouse {
	int x = 0;
	int y = 0;
	int z = 0;
	int w = 0;
	int dx = 0;
	int dy = 0;
	int dz = 0;
	int dw = 0;
}Mouse;


#define LONGLONG long

class Kore {
private:
	static ALLEGRO_EVENT_QUEUE *queue;
	static double	timescale;
	static bool		running;
	static LONGLONG seed;
	static double clock_frequency;
public:
	static int runlevel;
	static Mouse mouse;
	static bool initializeEvents();
	static bool initializeInput();
	static bool initializeSeed();
	static bool initialize( bool(*callback)() );
	static void shutdown( void(*callback)());
	static void loop( void(*logic_callback)(double),void(*draw_callback)(bool));
	static bool draw_time;
	static bool logic_time;
	static void processEvents();
	static void drawDebugInfo();
};

#define TRUERAND(a) (((float(rand())/float(RAND_MAX)) * float(a)))

#endif