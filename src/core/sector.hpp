#ifndef CORE_SECTOR
#define CORE_SECTOR

#include "direction.hpp"
#include "camera.hpp"
#include "sprite.hpp"
#include "Animation.hpp"
#include "layer.hpp"

#include <vector>

#define MAX_DENSITY  2048
#define MIN_DENSITY  0

typedef enum E_SectorAnimation {
	WATER_MID,
	WATER_DARK,
	WATER_MID_ALT,
	WATER_LIGHT,
	NW_DARK_MID,
	NE_DARK_MID,
	SW_DARK_MID,
	SE_DARK_MID,
	NW_MID_DARK,
	NE_MID_DARK,
	SW_MID_DARK,
	SE_MID_DARK,
	
	NW_MID_LIGHT,
	NE_MID_LIGHT,
	SW_MID_LIGHT,
	SE_MID_LIGHT,
	NW_LIGHT_MID,
	NE_LIGHT_MID,
	SW_LIGHT_MID,
	SE_LIGHT_MID,
	
	NW_MID_LAND,
	NE_MID_LAND,
	SW_MID_LAND,
	SE_MID_LAND,
	NW_LAND_MID,
	NE_LAND_MID,
	SW_LAND_MID,
	SE_LAND_MID,
	WASTELAND_CLIFFS,
	DESSERT_CLIFFS,
	JUNGLE_CLIFFS,
	COUNTY_CLIFFS,
	GENERIC_CLIFFS,
	GRASS_CLIFFS,
	SW_CLIFFS,
	SE_CLIFFS,
	EAST_WATER_LAND,
	WEST_WATER_LAND,
	SOUTH_WATER_LAND,
	NORTH_WATER_LAND,
	RESERVED_A,
	RESERVED_B,
	SECTORANIMATION_END
}SectorAnimation;


class Sector {
private:
	static std::vector<Sector*> list;

public:
	static	void add(Sector *sector);
	static	Sector *find(int x, int y, int z);
	Sector *sibiling[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };	
	bool	need_redraw = true;
	bool	deleted = false;
	int		subjective_x = 0;
	int		subjective_y = 0;
	int		subjective_z = 0;
	int		moisture     = 50;
	int		density		 = 10240;

	Layer	*data  = nullptr;
	Layer	*grass = nullptr;

	static int size;
	static int volume;

	bool partiallyOutOfCamera(OutOfCameraStatus &status);
	bool fullyOutOfCamera();

	void	relink();
	void	requestRedraw();
	void	setColor(int r, int g, int b);
	static Animation animations[64];
	static std::vector<ALLEGRO_BITMAP*> tile;
	static Sprite *tileset;

	Sector(int x = 0, int y = 0, int z = 0);
	void	destroy(bool cascade=false);
	~Sector();
	Sector* get(Direction direction);
	void	detach(Direction direction);
	void	draw();
	void	drawRadar();
	void	update(double delta);
	Sector*	getSibiling(Direction direction);
	static	bool initialize();
	static	void deinitialize();
	bool validMapIndex(int index);
};

#endif