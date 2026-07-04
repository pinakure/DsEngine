#include <main.hpp>

#define gpu engine->gpu

Map::Map(void){
	name = "empty";
	debug("Map", "~b%p Creating ~e%s~5 map...\n", this, name.c_str());
	orientation = ORIENTATION_ORTHOGONAL;
	version = 0.0f;
	transparency = COLOR_TRANSPARENT;
	collission = NULL;
	background = COLOR_BLACK;
	width = 0;
	height = 0;
	tileWidth = 0;
	tileHeight = 0;
	redraw = false;
	engine->setPlayer(NULL);	
//	environment.initialize();
	world = new World_2DShooter();	
}

/** 
@param	x Horizontal coordinate, measured in pixels
@param y Vertical coordinate, measured in pixels
@param z Longitudinal coordinate, measured in pixels
@return true if collision data exists on coordinates given, false if coordinates given are 'walkable'
**/
bool Map::isSolid(int x, int y, int z){
	x += tileWidth;
	y += tileHeight;
	
	if(x<0)return true;
	if(y<0)return true;
	//if(z<0)return true;
	
	
	int sx = x % tileWidth;
	int sy = y % tileHeight;
	
	int id = collission->get(x / tileWidth, y / tileHeight);
	if(id > -1){
		BITMAP *b = tileSet.getTile(id)->getBitmap();
		
		if(getpixel(b, sx, sy) != COLOR_TRANSPARENT) return true;
	}
	return false;
}

// NOTE: Use player=NULL to refer to 1UP player
void killPlayer(Player *player){
	
	Map *map = engine->getMap();

	Entity *e;
	
	// Use NULL or no parameter at all to select map->player (which is 1UP) 
	if(!player) player = engine->the_player;
	
	if (player == engine->the_player){
		engine->ui->pauseFastRun();
	}

	// Call player death animation / effect
	if(player) player->death_sequence();

	// Remove inserted collidable entity references from space
	Space *s;
	FOREACH(map->world->spaces, si, so){
		s = &map->world->spaces[si];
		
		FOREACH(s->bodies, bi, bo){
			e = s->bodies[bi];
			if(e == player){
				s->bodies.erase(s->bodies.begin() + bi);
				break;
			}
		}
	}

	// Remove camera target pointer
	if(map->camera.entityTarget == player) map->camera.entityTarget = NULL;
	
	// TODO: select another player
	if (engine->ui->the_player == player) engine->ui->the_player = NULL;
	
	// Set 1UP Pointer to null, to make spawners notice a new player shall be spawned
	if (engine->the_player == player) engine->the_player = NULL;
}

void Map::removeTargets(Entity *tgt){
	Entity *e;
	FOREACH(entity, i,o){
		e = entity[i];
		if(e->target.entity == tgt) e->target.entity = NULL;
	}
}

/** 
@param delta Time elapsed since last time this routine was called
**/

void Map::logic(int delta){
	if (key[KEY_K]) if (engine->the_player){ engine->the_player->body_status.head = 0; engine->the_player->body_status.overall = 0; };

	this->script.run(engine->console);
	// ----------------------------------------------------------------------------
	// SHOT PROCESSOR
	// ----------------------------------------------------------------------------
	// Process shot particles and discard non-alive ones	
	PROFILE_START();
	
	if(!new_shots.empty()){
		FOREACH(new_shots, b, bb){
			shots.push_back(new_shots[b]);
		}
		new_shots.clear();
	}
		
	FOREACH(shots, b, bb){
		Shot *s = shots[b];
		s->update();			// Move particle and update speeds
		s->logic();				// Check specific behavior
	}
			
	restart_delete_entity:

	FOREACH(shots, b, bb){
		if(!shots[b]->isAlive) {
			delete(shots[b]);
			shots.erase(shots.begin() + b);
			goto restart_delete_entity;
		}
	}
	PROFILE_END("shots");

	// ----------------------------------------------------------------------------
	// EXPLOSION PROCESSOR
	// ----------------------------------------------------------------------------
	// Process explosion particles and discard non-alive ones
	PROFILE_START();

	if(!new_explosions.empty()){
		FOREACH(new_explosions, b, bb){
			explosions.push_back(new_explosions[b]);
		}
		new_explosions.clear();
	}

	FOREACH(explosions, b, bb){
		Explosion *ex = explosions[b];
		ex->update();		// Move particle and update speeds
		ex->globalLogic();	// Check general collissions
		ex->logic();		
	}
	
	restart_delete_explosion:
	
	FOREACH(explosions, b, bb){
		if(!explosions[b]->isAlive) {
			delete(explosions[b]);
			explosions.erase(explosions.begin() + b);
			goto restart_delete_explosion;
		}
	}
	PROFILE_END("explosions");
	
	// ----------------------------------------------------------------------------
	// DEBRIS PROCESSOR
	// ----------------------------------------------------------------------------
	// Process debris particles and discard non-alive ones
	PROFILE_START();
	
	if(!new_debris.empty()){
		FOREACH(new_debris, bb, bbo){
			debris.push_back(new_debris[bb]);
		}
		new_debris.clear();
	}
	
	FOREACH(debris, b, bb){
		Debris *d = debris[b];
		d->update();
		d->logic();
	}
	
	restart_delete_debris:
	
	FOREACH(debris, b, bb){
		if(!debris[b]->isAlive) {
			delete(debris[b]);
			debris.erase(debris.begin() + b);
			goto restart_delete_debris;
		}
	}
	PROFILE_END("debris");
	
	// ----------------------------------------------------------------------------
	// ENTITY PROCESSOR
	// ----------------------------------------------------------------------------
	// Process entities
	PROFILE_START();
	
	Entity *e;

	if (engine->the_player) engine->the_player->logic(delta, this);

	FOREACH(entity, i, o){
		e = entity[i];
		
		if(e==engine->the_player)continue;
		e->logic(delta, this);
		
		// When player is unset, map looks for the player to fill the variable
		
		if(e->type == ENTITY_PLAYER){
			if (!engine->the_player){
				Player* p = static_cast<Player*>(e);
				assert(p);
				// But it just tells the camera "the target is the player" once
				// To allow changing the camera target with an 'alive' player
				engine->setPlayer(p);
				engine->ui->resumeFastRun();
				engine->ui->the_player = engine->the_player;
				camera.entityTarget = engine->ui->the_player;
				gpu->setPostProcessor(POSTPROCESSOR_CLEAR);					
			}
			if(e->isDead){
				engine->ui->updateBody();
				removeTargets(e);
				killPlayer(static_cast<Player*>(e));
				continue;
				//goto restart_entity_processing;
			}			
		}

		if(e->redraw) {
			redraw = true;
			e->redraw = false;
		}
	}
	// Remove dead entities
	vector<Entity*> new_entities;
	vector<Entity*>::iterator en;
	for(en = entity.begin(); en != entity.end(); en++){
		e = *en;
		if(!e->isDead)new_entities.push_back(e);
		else {			
			removeTargets(e);
			delete e;
		}
	}
	entity = new_entities;
	PROFILE_END("entity");
	
	//environment.update(delta);
	PROFILE_START();
	world->update(delta, this);
	PROFILE_END("world");
	
	PROFILE_START();
	Entity::animateLiquids();
	PROFILE_END("liquids");
	
	
	PROFILE_START();
	camera.logic(delta);
	PROFILE_END("camera");	
}

/** 
@param *li Pointer to EntityInfo containing the details for generating a new entity
**/

void Map::loadEntity(EntityInfo *ei){
	Entity *e;


	Map *map = engine->getMap();



#define ENTITYCASE(class_type, type)	case type:\
										e = new class_type();\
										break;
	switch(ei->type){
		ENTITYCASE(GenericEntity		, ENTITY_GENERIC);
		ENTITYCASE(Player				, ENTITY_PLAYER);
		ENTITYCASE(EntityNpc			, ENTITY_NPC);
		ENTITYCASE(EntityMob			, ENTITY_MOB);
		ENTITYCASE(Spawner				, ENTITY_SPAWNER);		
		ENTITYCASE(EntityTrigger		, ENTITY_TRIGGER);
		ENTITYCASE(EntityLogic			, ENTITY_LOGIC);
		ENTITYCASE(EntityTimer			, ENTITY_TIMER);
		ENTITYCASE(EntityMultiple		, ENTITY_MULTIPLE);		
		ENTITYCASE(WaterControl			, ENTITY_WATERCONTROL);
		ENTITYCASE(LightingControl		, ENTITY_LIGHTINGCONTROL);
		ENTITYCASE(LavaControl			, ENTITY_LAVACONTROL);
		ENTITYCASE(PhysicsControl		, ENTITY_PHYSICSCONTROL);		
		ENTITYCASE(EntityItem			, ENTITY_ITEM);
		ENTITYCASE(EntityToken			, ENTITY_TOKEN);
		ENTITYCASE(EntityHealth			, ENTITY_HEALTH);		
		ENTITYCASE(ScriptBlock			, ENTITY_SCRIPTBLOCK);
		ENTITYCASE(DialogBlock			, ENTITY_DIALOGBLOCK);
		ENTITYCASE(PathBlock			, ENTITY_PATHBLOCK);		
		ENTITYCASE(EntityPush			, ENTITY_PUSH);
		ENTITYCASE(EntityDebris			, ENTITY_DEBRIS);
		ENTITYCASE(EntityExplosion		, ENTITY_EXPLOSION);
		ENTITYCASE(EntityLadder			, ENTITY_LADDER);
		ENTITYCASE(EntityDoor			, ENTITY_DOOR);
		ENTITYCASE(EntitySwitch			, ENTITY_SWITCH);
		ENTITYCASE(EntityTeleport		, ENTITY_TELEPORT);
		ENTITYCASE(EntityShot			, ENTITY_SHOT);
		ENTITYCASE(EntityLaser			, ENTITY_LASER);
		ENTITYCASE(EntityTurret			, ENTITY_TURRET);
		ENTITYCASE(EntityPlatform		, ENTITY_PLATFORM);
		ENTITYCASE(EntitySoundStream	, ENTITY_SOUNDSTREAM);
		ENTITYCASE(EntityExplosive		, ENTITY_EXPLOSIVE);				
		ENTITYCASE(EntityStaticLight	, ENTITY_STATICLIGHT);
		ENTITYCASE(EntityBreakSurface	, ENTITY_BREAKSURFACE);
		ENTITYCASE(EntityMovable		, ENTITY_MOVABLE);
		ENTITYCASE(EntityPosition		, ENTITY_POSITION);
		default:
			debug("Map", "~cERROR: Wrong entity type ~e'%d'~c specified!\n", ei->type); 
			return;
	}
#undef ENTITYCASE

	e->name = ei->name;
	e->typeName = getEntityNameByType(ei->type);
	
	// Pass properties to the entity instance
	Property *p;
	for(int i=0, o=ei->properties.count; i<o; i++){
		p = ei->properties.get(i);
		e->addProperty(p->name.c_str(), p->value.c_str());
	}
	
	// Read common properties
	e->readCommonProperties();
	
	// Initialize entity
	e->init(ei->position, ei->size);
	
	
	// Add entity to entity stack
	entity.push_back(e);

	// Add entity body to the collision handler
	world->addEntity(e);
	
	// Delete properties, not needed anymore
	while(ei->properties.count) ei->properties.remove(0);

	

	// Update entity targets
	for(int i = 0, o = map->entity.size();i<o; i++) {
		Entity *le = map->entity[i];
		// Skip those whose entity was already found!
		if(le->target.entity) continue;

		// Avoid linking with self entity		
		if(le->target.name.compare("") && le->target.name.compare(le->name)){
			Entity::solveTargetByName(le->target.name.c_str(), le->target);
		} else {
			debug("ENTITY", "~cTarget for entity ~a%s~b(UUID:%d) ~cis recursive!", le->name.c_str(), le->uuid);
		}
	}
}


/** 
@param fileName File containing the map data which has to fill this Map instance
@return true if loading was correct, false if it can not be loaded
**/

int Map::load_silent(const char* fileName){
	int i;
	MapInfo mi;
	
	String path  = "data/";
	path.append(fileName);
		
	if(!mi.load(path.c_str())) {
		engine->console->printf("~cERROR: Unable to extract MapInfo from file %s!\n", path.c_str());
		return false;
	}
	
	// Read map details
	name = mi.name;
	orientation = mi.orientation;
	version = mi.version;
	transparency = mi.transparency;
	background = mi.background;
	width = mi.width;
	height = mi.height;
	tileWidth = mi.tileWidth;
	tileHeight = mi.tileHeight;
	
	// Merge tilesets contained in each tileSetInfo
	for(i=0; i< mi.tilesetInfo.count; i++){
		tileSet.load(mi.tilesetInfo.get(i));
	}
		
	// Add layers described in each layerInfo
	for(i=0; i< mi.layerInfo.count; i++){
		//~ if(!mi.layerInfo.get(i)->name.Compare("collission")){
		if(!strcmp(mi.layerInfo.get(i)->name, "collission")){
			collission = new Layer(mi.layerInfo.get(i), this);
		} else {
			layer.push_back( new Layer( mi.layerInfo.get(i) , this));
		}
	}

	
	if(collission) world->load(collission);
	
	redraw = true;
	return true;
}

int Map::load(const char* fileName){
	engine->spu->playMusic(MUSIC_LOADING, true);
	bool old_status = engine->console->enabled;
	engine->console->enabled = false;

	float percent = 0.0f;
	float percentStep = 1.0f;
	char number[15];
			
	
	debug("Map", "Creating map upon file ~7'~e%s~7'...\n", fileName);
	int i;
	MapInfo mi;
	
	// Remove relative paths to keep them under ./data
	// NOTE: This prevents a hidden file for being found (. starting file)
	gpu->setProgressCount(1, "Loading Map");
	
	String *path = new String("data/");
	String *mapFile = new String(fileName);
	
	int offset = 0;
	while(fileName[offset] == '.') {
			if(fileName[offset+1] == '.')
				if(fileName[offset+2] == '/')
					offset+=2;
			offset++;
	}
	mapFile->erase(0, offset);
	path->append(mapFile->c_str());
	
	if(!mi.load(path->c_str())) {
		debug("Map", "~cERROR: Unable to extract MapInfo from file ~e%s~c!\n", path->c_str()); 
		engine->console->enabled = old_status;
		selectMode(ENGINEMODE_TITLE);
		return false;
	}
	delete path;
	delete mapFile;
	
	gpu->progressStage("Analyzing map data");
	
	// Read map details
	name = mi.name;
	orientation = mi.orientation;
	version = mi.version;
	transparency = mi.transparency;
	background = mi.background;
	width = mi.width;
	height = mi.height;
	tileWidth = mi.tileWidth;
	tileHeight = mi.tileHeight;
	// Read properties
	percentStep = 5.0f / float(mi.layerInfo.count);	
	gpu->setProgressCount(mi.properties.count, "Properties");
	for(i=0; i < mi.properties.count; i++){
		mi.properties.get(i)->dump();
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// !! Load Script
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if (!mi.properties.get(i)->name.compare("script")){
			this->script.load(mi.properties.get(i)->value.c_str());
		}
		gpu->progressStage();
	}	
	
	// Merge tilesets contained in each tileSetInfo
	debug("Map", "Merging ~e%d~5 TilesetInfo%s into a single TileSet instance...\n", mi.tilesetInfo.count, PLURAL(mi.tilesetInfo.count));
	gpu->setProgressCount(mi.tilesetInfo.count, "Merging Tilesets");	
	for(i=0; i< mi.tilesetInfo.count; i++){
		tileSet.load(mi.tilesetInfo.get(i));
		gpu->progressStage(mi.tilesetInfo.get(i)->name);	
	}
		
	// Add layers described in each layerInfo
	debug("Map", "Loading ~e%d~5 LayerInfo%s into Layer instances...\n", mi.layerInfo.count, PLURAL(mi.layerInfo.count));

	gpu->setProgressCount(mi.layerInfo.count, "Loading Layers");	
	for(i=0; i< mi.layerInfo.count; i++){
		//~ if(!mi.layerInfo.get(i)->name.Compare("collission")){
		debug("Map", "LAYER ~f#%d ~7> ~e%s~5...\n", i, mi.layerInfo.get(i)->name);
		
		percent+=percentStep;
		
		if(!strcmp(mi.layerInfo.get(i)->name, "collission")){
			collission = new Layer(mi.layerInfo.get(i), this);
			gpu->progressStage("Collision");
		} else {
			layer.push_back( new Layer( mi.layerInfo.get(i) , this));
			String datastr = "Data ";
			_itoa(i, number, 10);
			datastr += number;
			gpu->progressStage(datastr.c_str());
		}	
	}

	if(collission) {
		gpu->setProgressCount(1, "Loading World");	
		world->load(collission);			
	} 
	
	
	// Load entitities described in each EntityInfo
	gpu->setProgressCount(mi.entityInfo.count, "Loading Entities");	
	if(mi.entityInfo.count > 0) {
		debug("Map", "Loading ~e%d~5 EntityInfo%s into Entity instances...\n", mi.entityInfo.count, PLURAL(mi.entityInfo.count));
	
		for(i = 0; i < mi.entityInfo.count; i++) {
			loadEntity(mi.entityInfo.get(i));
		}
		gpu->progressStage();
	}

	
	// Configure camera for this map's geometry
	Position mapGeometry;
	mapGeometry.set( (width-2) * tileWidth, (height-2) * tileHeight, 1.0f);
	camera.setSpace(mapGeometry);
	
	// Enable First redraw cycle
	redraw = true;
	gpu->setProgressCount(1, "Done");	gpu->progressStage(fileName);
	engine->transition->start(gpu->vram, TRANSITION_TYPE_FADE_TO_BLACK);
	engine->transition->wait(TRANSITION_STATUS_OUT);

	engine->ui->setRadarMap(this);

	engine->ui->restart();
	
	engine->console->enabled = old_status;
	selectMode(ENGINEMODE_GAME);	
	return true;
}

/** 
@param *b BITMAP pointer onto which to draw the collissions detected
**/
void Map::drawCollissionLayer(BITMAP *b){	
	if(!config.debug_world->get()) return;
					
	int ox = camera.visible.origin.x;
	int oy = camera.visible.origin.y;
	int lx = camera.visible.xLimit();
	int ly = camera.visible.yLimit();
	
	int c = 0;
	for(int y=oy; y < ly; y++){
		for(int x=ox; x < lx; x++){
			if(!(c%12))if(isSolid(x, y))putpixel(b, x-ox, y-oy, makecol(128-(c%128),255,0)); 
			c++;
		}
	}
}

/** 
@param *bmp BITMAP pointer onto which to draw every layers
@param X Horizontal coordinate where to start the layers
@param Y Vertical coordinate where to start the layers
@param layerIndex Layer index to be drawn. If negative, then the whole set of layers contained in the map is drawn instead.
**/
void Map::render(BITMAP* bmp, int X, int Y, int layerIndex, int ox, int oy){
	Layer *l;
	if(layerIndex >= 0){
		l = layer[layerIndex];
		if(!l) return;
		l->render(bmp, X, Y, ox, oy);
	} else {
		FOREACH(layer, i, o){
			l = layer[ i ];
			if(!l) continue;
			l->render(bmp, X, Y, ox, oy);
		}
	}
}

/** 
@param *bmp BITMAP pointer onto which to draw all stuff
**/

#define TRUE_RAND	(float(rand())/float(RAND_MAX))

void Map::draw(BITMAP* bmp){
	//clear_to_color(bmp, makecol(16,16,16));
	
	for(int i=0, o=layer.size();i<o; i++){
		Layer *l = layer[i];
		if(!l) continue;
		l->draw(bmp, this);
		if(i== o - 2){
			
			// Draw Shots					
			FOREACH(shots, si, sil){
				Shot *s = shots[si];
				if(s)s->draw(bmp, this);
			}
			
			// Draw explosions
			FOREACH(explosions, si, sil){
				Explosion *ex = explosions[si];
				if(ex)ex->draw(bmp, this);
			}

			// Draw Debris
			FOREACH(debris, di, dil){
				Debris *d = debris[di];
				if(d)d->draw(bmp, this);
			}

			// Draw entities, but skip players!
			vector<Entity*> players;
			FOREACH(entity, ei, eil){
				Entity *e = entity[ ei ];
				switch(e->type){
					case ENTITY_PLAYER: players.push_back(entity[ei]); continue;
					case ENTITY_PATHBLOCK: break;
					default:
						/*if(e->position.x+e->halfsize.x < camera.visible.origin.x)continue;
						if(e->position.y+e->halfsize.y < camera.visible.origin.y)continue;
						if(e->position.x-e->halfsize.x > camera.visible.origin.x + camera.visible.size.x)continue;
						if(e->position.y-e->halfsize.y > camera.visible.origin.y + camera.visible.size.y)continue;*/
						break;
				}
				if (e->position.x + e->size.x < camera.visible.origin.x)continue;
				else if (e->position.y + e->size.y < camera.visible.origin.y)continue;
				else if (e->position.y > camera.visible.origin.y + camera.visible.size.y)continue;
				else if (e->position.x > camera.visible.origin.x + camera.visible.size.x)continue;
				e->draw(bmp, this);
			}	
			// Now, draw players, over every existing entity
			FOREACH(players, ei, eil){
				Entity *e = players[ ei ];				
				e->draw(bmp, this);
			}
		} 
	}
	//environment.draw(bmp);

	if(world->enableDraw){
		world->draw(bmp, camera.visible.origin.x, camera.visible.origin.y);
	}
	
	
	// Some debug information maybe
	if(!config.debug_camera->get()) return;

	int c = makecol(255,255,0);
	gpu->print(0, Y_CENTER+40, c, "ORIGIN x: %02.2f  y: %02.2f  z: %02.2f ", camera.visible.origin.x, camera.visible.origin.y, camera.visible.origin.z);
	gpu->print(0, Y_CENTER+50, c, " LIMIT x: %02.2f  y: %02.2f  z: %02.2f ", camera.visible.xLimit(), camera.visible.yLimit(), camera.visible.zLimit());
	gpu->print(0, Y_CENTER+60, c, "  SIZE x: %02.2f  y: %02.2f  z: %02.2f ", camera.visible.size.x, camera.visible.size.y, camera.visible.size.z);
	gpu->print(0, Y_CENTER+70, c, "SCROLL x: %02.2f  y: %02.2f  z: %02.2f ", camera.scroll.x, camera.scroll.y, camera.scroll.z);

	camera.draw(bmp);
}

Map::~Map(void){
	if(collission) delete collission;
	FOREACH(layer, i,o) delete layer[i];
	FOREACH(entity, i,o) if(entity[i])delete entity[i];
	FOREACH(shots, i,o) if(shots[i])delete (shots[i]);
	FOREACH(debris, i,o) if(debris[i])delete (debris[i]);

	engine->setPlayer(NULL);
	engine->ui->the_player = NULL;
	delete world;
	debug("Map", "~b%p Destroying map ~f%s~5...\n", this, name.c_str());
}

void Map::compile(void){
	// Collect every used tile in a Tile vector
	// Create single bitmap, holding a sorted tileset made of every used tile
	//  
}

#undef gpu