/* DIRECT3D GPU Backend */
#include <stdio.h>
#include "gpu.hpp"
#include "kore.hpp"
#include <allegro5/allegro_ttf.h>
double				Gpu::fps = 0;
int					Gpu::frames = 0;
int					Gpu::width  = 0;
int					Gpu::height = 0;
bool				Gpu::fullscreen = GPU_DEFAULT_FULLSCREEN;
float				Gpu::zoom = 2.0;
Surface	 			Gpu::midlayer;
Surface	 			Gpu::buffer;
Surface	 			Gpu::overlay;
ALLEGRO_DISPLAY*	Gpu::screen = NULL;
ALLEGRO_COLOR		Gpu::font_color;
ALLEGRO_FONT*		Gpu::font = nullptr;
bool				Gpu::redraw = false;
int					Gpu::frame_rate = 60;
		
void Gpu::clear(int r, int g, int b) {
	al_set_target_bitmap(Gpu::overlay.bitmap);
	al_clear_to_color(al_map_rgba(0,0,0,0));   
	al_set_target_bitmap(Gpu::midlayer.bitmap);
	al_clear_to_color(al_map_rgba(0,0,0,0));   
	al_set_target_bitmap(Gpu::buffer.bitmap);
	al_clear_to_color(al_map_rgb(r,g,b));   
}

void Gpu::shutdown() {
	Kore::runlevel--;
}

int Gpu::initialize(int width, int height, bool fullscreen) {	
	Gpu::width  = width;
	Gpu::height = height;
	Gpu::fullscreen = fullscreen;
	
	if(!al_init()) {
		al_show_native_message_box(Gpu::screen, "Error", "Error", "Failed to initialize allegro!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return false;
    }

	if(fullscreen) al_set_new_display_flags(ALLEGRO_FULLSCREEN);   
	Gpu::screen = al_create_display(width*Gpu::zoom, height*Gpu::zoom);
	if(!Gpu::screen) {
		al_show_native_message_box(Gpu::screen, "Error", "Error", "Failed to initialize screen!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return false;
    }
	al_set_new_display_refresh_rate(60);
	
	if(!al_init_image_addon()) {
		al_show_native_message_box(Gpu::screen, "Error", "Error", "Failed to initialize al_init_image_addon!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return false;
    }
	
	// Initialize font
	al_init_font_addon();
	al_init_ttf_addon();
	Gpu::font = al_load_ttf_font("data/fonts/small.ttf",16,0 );
	if (!Gpu::font){
		al_show_native_message_box(Gpu::screen, "Error", "Error", "Failed to initialize font!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
	    return false;
	}
	Gpu::setFontColor(0, 255, 0, 200);

   

	Gpu::buffer = Surface(Gpu::width, Gpu::height, 0, 0);
	if (!Gpu::buffer.isInitialized()) {
		al_show_native_message_box(Gpu::screen, "Error", "Error", "Failed to initialize backbuffer!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return false;
    }
	Gpu::midlayer= Surface(Gpu::width, Gpu::height, 0, 0);
	if (!Gpu::midlayer.isInitialized()) {
		al_show_native_message_box(Gpu::screen, "Error", "Error", "Failed to initialize midlayer backbuffer!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return false;
    }
	Gpu::overlay= Surface(Gpu::width, Gpu::height, 0, 0);
	if (!Gpu::overlay.isInitialized()) {
		al_show_native_message_box(Gpu::screen, "Error", "Error", "Failed to initialize overlay backbuffer!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return false;
    }
	Gpu::clear(0, 0, 0);
    Gpu::flip();
	
	al_init_primitives_addon();
	
	Kore::runlevel++;
	return true;
}

void Gpu::wait(double time) {
	al_rest(time);
}

#include <allegro5\allegro_color.h>

void Gpu::preflip() {
	// Push midlayer over gpu buffer
	al_set_target_bitmap(Gpu::buffer.bitmap);
	al_draw_bitmap(Gpu::midlayer.bitmap, 0, 0, 0);
	// Push overlay over gpu buffer
	al_draw_bitmap(Gpu::overlay.bitmap, 0, 0, 0);
	// Clean overlay (reusable since this call)
	al_set_target_bitmap(Gpu::overlay.bitmap);
	al_clear_to_color(al_map_rgba(0,0,0,0)); 	
	al_set_target_bitmap(Gpu::midlayer.bitmap);
	al_clear_to_color(al_map_rgba(0,0,0,0)); 	
}

void Gpu::flip(){
	al_set_target_backbuffer(Gpu::screen);
	al_draw_scaled_bitmap(Gpu::buffer.bitmap,  0, 0, Gpu::width, Gpu::height, 0, 0, Gpu::width * Gpu::zoom, Gpu::height * Gpu::zoom, 0);
	al_draw_scaled_bitmap(Gpu::midlayer.bitmap,0, 0, Gpu::width, Gpu::height, 0, 0, Gpu::width * Gpu::zoom, Gpu::height * Gpu::zoom, 0);
	al_draw_scaled_bitmap(Gpu::overlay.bitmap, 0, 0, Gpu::width, Gpu::height, 0, 0, Gpu::width * Gpu::zoom, Gpu::height * Gpu::zoom, 0);
	//al_draw_tinted_scaled_bitmap(Gpu::overlay.bitmap, al_map_rgba(0,0,0,0.5), 0, 0, Gpu::width, Gpu::height, 0, 0, Gpu::width * Gpu::zoom, Gpu::height * Gpu::zoom, 0);
	al_flip_display();
	Gpu::frames++;
}

void Gpu::cleanup() {
	if (Gpu::screen) {
		al_destroy_display(Gpu::screen);
		Gpu::screen = nullptr;
	}
	al_shutdown_primitives_addon();
	
}

void Gpu::setFontColor(int r, int g, int b, int a) {
	Gpu::font_color = al_map_rgba(r,g,b,a);
}

void Gpu::print(const char *text, int x, int y) {
   al_draw_text(Gpu::font, al_map_rgba(0,0,0,64), x+1, y+1, ALLEGRO_ALIGN_LEFT, text);
   al_draw_text(Gpu::font, Gpu::font_color, x, y, ALLEGRO_ALIGN_LEFT, text);
}

void Gpu::printRight(const char *text, int x, int y) {
   al_draw_text(Gpu::font, al_map_rgba(0,0,0,64), x+1, y+1, ALLEGRO_ALIGN_RIGHT, text);
   al_draw_text(Gpu::font, Gpu::font_color, x, y, ALLEGRO_ALIGN_RIGHT, text);
}

void Gpu::printCentered(const char *text, int x, int y) {
   al_draw_text(Gpu::font, al_map_rgba(0,0,0,64), x+1, y+1, ALLEGRO_ALIGN_CENTRE, text);
   al_draw_text(Gpu::font, Gpu::font_color, x, y, ALLEGRO_ALIGN_CENTRE, text);
}

ALLEGRO_BITMAP *Gpu::createImage(int width, int height) {
	ALLEGRO_BITMAP *bmp = al_create_bitmap(width, height);
	assert(bmp != NULL);
	return bmp;
}