#ifndef CORE_LAYER
#define CORE_LAYER

#define DRAW_COLOR_FILTER
#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>

class Layer {
	public:
		char				name[64] = "";			
		int					width = 0;
		int					height = 0;
		__int64				*data = nullptr;
		double				parallax = 0.0;
		int					r = 128;
		int					g = 128;
		int					b = 128;
		int					x = 0;
		int					y = 0;
		int					z = 0;
		int					highlight_index = -1;
		bool				color = true;
		bool				numbers= false;
		void				*parent;
		bool                grid = true;//requres color=true

		Layer(void *parent);
		Layer(const char *name, int width, int height, double parallax, __int64 *data, size_t data_size, int x, int y, int z, void *parent);
		void fill(int index);
		void load(const char *name, int width, int height, double parallax, __int64 *data, size_t data_size);
		void set(int x, int y, __int64 value);		
		__int64 get(int x, int y);		
		ALLEGRO_COLOR getColor(int x, int y, int opacity=255);		
		void randomize();
		void draw(int camera_offset_x=0, int camera_offset_y=0);		
		~Layer(void);
		void setHighlight(int x, int y);
};

#endif
