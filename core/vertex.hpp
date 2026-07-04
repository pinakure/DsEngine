#ifndef CORE_VERTEX
#define CORE_VERTEX
#include <allegro5/allegro_color.h>
#define FLOAT float
typedef struct s_Vertex {
    FLOAT x, y, z;
    ALLEGRO_COLOR color;    //add color to vertex structure
	FLOAT u, v;
}Vertex;


#endif