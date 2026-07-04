#ifndef CORE_ANIMATION
#define CORE_ANIMATION

#include <vector>
#include "surface.hpp"
#include "sprite.hpp"

#include "direction.hpp"

class Animation {
private:
public:
	bool		bidirectional   = true;
	int			width			= 0;
	int			height			= 0;
	double		current_frame	= 0;
	Direction	direction		= Direction::UP;
	std::vector<ALLEGRO_BITMAP*> frame;

	Animation();
	Animation(Sprite &sprite, int width, int height,int sx=0, int sy=0, int dx=1, int dy=1, bool vertical=false);
	int		extractFrames(Sprite &sprite, int source_x=0, int source_y=0, int dest_x=1, int dest_y=1, bool read_vertically = false);
	void	draw(Surface &image, int x = 0, int y = 0);
	void	run(double delta=1.0);
};

#endif 