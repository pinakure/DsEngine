#ifndef CLASSES_TILED_MAP
#define CLASSES_TILED_MAP

#include <classes/entities/entity.hpp>
#include <classes/types/world/world.hpp>

/*! @brief 
 * Abstraction of the elements which are contained in a map, its geometry,
 * properties and the entities which populate the space defined inside. 
 */
class Map {
	private:
	
	public:
		Camera				camera;			//!< Camera instance which should follow the player entity
		String				name;			//!< Name for the map (It can be left empty)
		MapInfo_Orientation	orientation;	//!< Kind of map representation used
		float				version;		//!< Map version (should be 1.0)
		
		Layer				*collission;	//!< Layer containing the collission tile indexes 
		World				*world;
		//Environment			environment;
		
		int					transparency;	//!< Encoded color value which will be translated to transparent color 
		int					background;		//!< Encoded color value of the background
		
		int					width;			//!< Horizontal size, measured in tiles (it must be 2 or bigger)
		int					height;			//!< Vertical size, measured in tiles (it must be 2 or bigger)
		int					tileWidth;		//!< Width of the map grid tiles, in pixels
		int					tileHeight;		//!< Height of the map grid tiles, in pixels
		
		bool				redraw;			//!< If true, Map::draw() will be called on the game loop
		
		TileSet				tileSet;		//!< TileSet instance which holds all map tiles, sorted linearly (it cannot hold empty spaces)
		vector<Layer*>		layer;			//!< List of every Layer which the map is composed from
		vector<Entity*>		entity;			//!< List of all entities which populate the map
		vector<Explosion*>	explosions;
		vector<Explosion*>	new_explosions;
		deque<Shot*>		shots;
		deque<Shot*>		new_shots;
		vector<Debris*>		debris;
		vector<Debris*>		new_debris;

		Map(void);
		
		int load_silent(const char* fileName);	//!< Shhh
		int load(const char* fileName);		//!< Loads given file, which is suposed to hold valid map data
		void loadEntity(EntityInfo *li);		//!< Loads the entity defined at given entityInfo pointer
	

		void removeTargets(Entity *tgt);


		void compile(void);

		bool isSolid(int x, int y, int z=0);	//!< Check if given coordinates are walkable or not in given Map instance
		void logic(int delta);				  	//!< Calls Entity::logic for each one of the entities in the list, and updates the camera 
		void render(BITMAP *bmp,int X=0, int Y=0, int layerIndex=-1, int ox=0, int oy= 0);//!< Draws map layer with given index onto given bitmap starting at given coordinates, measured in pixels. @note If layer index is negative, the whole map is painted instead.
		void draw(BITMAP*);	//!< Draws layers, entities and some debug info, if enabled
		
		// Debugging!
		void drawCollissionLayer(BITMAP *b);	//!< Scans visible area, then plots a vibrant color pixel when a collission point is found

		~Map(void);
};

void killPlayer(Player *player = NULL);

#endif
