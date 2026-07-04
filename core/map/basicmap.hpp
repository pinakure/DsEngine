#ifndef CLASSES_BASICMAP
#define CLASSES_BASICMAP
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!NOT WORKING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
class BasicMap {
	protected:
		Camera						camera;
		TileSet						tile_set;
		vector<Layer*>				layer;			//!< List of every Layer which the map is composed from
		vector<Entity*>				entity;			//!< List of all entities which populate the map
		Layer						*collision;	//!< Layer containing the collission tile indexes 


	public:
		World						*world;
		bool						redraw;			//!< If true, Map::draw() will be called on the game loop

		String						name;
		MapInfo_Orientation			orientation;
		float						version;		//!< Map version (should be 1.0)
		int							transparency;
		int							background;
		int							width;			//!< Horizontal size, measured in tiles (it must be 2 or bigger)
		int							height;			//!< Vertical size, measured in tiles (it must be 2 or bigger)
		int							tile_width;		//!< Width of the map grid tiles, in pixels
		int							tile_height;	//!< Height of the map grid tiles, in pixels

		Layer *getCollisionLayer(void);
		int getScrollX(void);
		int getScrollY(void);
		Tile *getTile(size_t index);

		void logic(int delta);				  	//!< Calls Entity::logic for each one of the entities in the list, and updates the camera 
		void render(BITMAP *bmp, int X = 0, int Y = 0, int layerIndex = -1, int ox = 0, int oy = 0);//!< Draws map layer with given index onto given bitmap starting at given coordinates, measured in pixels. @note If layer index is negative, the whole map is painted instead.
		void draw(BITMAP*);	//!< Draws layers, entities and some debug info, if enabled
		
		virtual void loadEntity(EntityInfo *li);		//!< Loads the entity defined at given entityInfo pointer
		virtual void compile(void);

		
		bool isSolid(int x, int y, int z = 0);	//!< Check if given coordinates are walkable or not in given Map instance
		void removeTargets(Entity *tgt);
};

#endif