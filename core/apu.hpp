#ifndef CORE_APU
#define CORE_APU

typedef enum E_SoundEffect {
	SFX_FOOTSTEP_0,
	SFX_FOOTSTEP_1,
	SFX_FOOTSTEP_2,
	SFX_FOOTSTEP_3,
	SFX_FOOTSTEP_4,
	SFX_FOOTSTEP_5,
	SFX_FOOTSTEP_6,
	SFX_FOOTSTEP_7,
	SFX_FOOTSTEP_8,
	SFX_FOOTSTEP_9,
	SFX_FOOTSTEP_A,
	SFX_FOOTSTEP_B,
	SFX_FOOTSTEP_C,
	SFX_FOOTSTEP_D,
	SFX_FOOTSTEP_E,
	SFX_FOOTSTEP_F,
	SFX_RAINDROP_0,
	SFX_RAINDROP_1,
	SFX_RAINDROP_2,
	SFX_RAINDROP_3,
	SFX_RAINDROP_4,
	SFX_RAINDROP_5,
	SFX_RAINDROP_6,
	SFX_RAINDROP_7,
	SFX_RAINDROP_8,
	SFX_RAINDROP_9,
	SFX_RAINDROP_A,
	SFX_RAINDROP_B,
	SFX_RAINDROP_C,
	SFX_RAINDROP_D,
	SFX_RAINDROP_E,
	SFX_RAINDROP_F,
	SFX_SPLASH_0,
	SFX_SPLASH_1,
	SFX_SPLASH_2,
	SFX_SPLASH_3,
	SFX_SPLASH_4,
	SFX_SPLASH_5,
	SFX_SPLASH_6,
	SFX_SPLASH_7,
	SFX_SPLASH_8,
	SFX_SPLASH_9,
	SFX_SPLASH_A,
	SFX_SPLASH_B,
	SFX_SPLASH_C,
	SFX_SPLASH_D,
	SFX_SPLASH_E,
	SFX_SPLASH_F,
	MAX_SOUNDEFFECT,
}SoundEffect;

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

class Apu {
private:
public:
	static const char *sfx_filename[SoundEffect::MAX_SOUNDEFFECT];
	static ALLEGRO_SAMPLE *sample[SoundEffect::MAX_SOUNDEFFECT+1];
	static bool initialize();
	static bool load(SoundEffect sound);
	static void play(SoundEffect sound);
	static void unload(SoundEffect sound);
	static void shutdown();
};

#endif