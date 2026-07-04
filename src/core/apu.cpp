#include <string>
#include "apu.hpp"
#include "kore.hpp"

ALLEGRO_SAMPLE *Apu::sample[SoundEffect::MAX_SOUNDEFFECT + 1] = {
	/* FootStep */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	/* Raindrop */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	/* Splash   */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr
};
/*
	SFX_FOOTSTEP_0, // Sand Grass Step #1
	SFX_FOOTSTEP_1, // Sand Grass Step #2
	SFX_FOOTSTEP_2,	// Sand Grass Step #3
	SFX_FOOTSTEP_3,	// Sand Grass Step #4
	SFX_FOOTSTEP_4,	// Big Grass Step #1
	SFX_FOOTSTEP_5,	// Big Grass Step #2
	SFX_FOOTSTEP_6,	// Big Grass Step #3
	SFX_FOOTSTEP_7,	// Grass Step #1 // Big Grass Step #4
	SFX_FOOTSTEP_8,	// Grass Step #2
	SFX_FOOTSTEP_9,	// Grass Step #3
	SFX_FOOTSTEP_A,	// Grass Step #4
	SFX_FOOTSTEP_B,	// Wet Step #1
	SFX_FOOTSTEP_C,	// Wet Step #2
	SFX_FOOTSTEP_D,	// Wet Step #3
	SFX_FOOTSTEP_E,	// Wet Step #4
	SFX_FOOTSTEP_F,	// Big Splatter
*/
const char *Apu::sfx_filename[SoundEffect::MAX_SOUNDEFFECT] = {
	"SFX_FOOTSTEP_0",
	"SFX_FOOTSTEP_1",
	"SFX_FOOTSTEP_2",
	"SFX_FOOTSTEP_3",
	"SFX_FOOTSTEP_4",
	"SFX_FOOTSTEP_5",
	"SFX_FOOTSTEP_6",
	"SFX_FOOTSTEP_7",
	"SFX_FOOTSTEP_8",
	"SFX_FOOTSTEP_9",
	"SFX_FOOTSTEP_A",
	"SFX_FOOTSTEP_B",
	"SFX_FOOTSTEP_C",
	"SFX_FOOTSTEP_D",
	"SFX_FOOTSTEP_E",
	"SFX_FOOTSTEP_F",
	"SFX_RAINDROP_0",
	"SFX_RAINDROP_1",
	"SFX_RAINDROP_2",
	"SFX_RAINDROP_3",
	"SFX_RAINDROP_4",
	"SFX_RAINDROP_5",
	"SFX_RAINDROP_6",
	"SFX_RAINDROP_7",
	"SFX_RAINDROP_8",
	"SFX_RAINDROP_9",
	"SFX_RAINDROP_A",
	"SFX_RAINDROP_B",
	"SFX_RAINDROP_C",
	"SFX_RAINDROP_D",
	"SFX_RAINDROP_E",
	"SFX_RAINDROP_F",
	"SFX_SPLASH_0",
	"SFX_SPLASH_1",
	"SFX_SPLASH_2",
	"SFX_SPLASH_3",
	"SFX_SPLASH_4",
	"SFX_SPLASH_5",
	"SFX_SPLASH_6",
	"SFX_SPLASH_7",
	"SFX_SPLASH_8",
	"SFX_SPLASH_9",
	"SFX_SPLASH_A",
	"SFX_SPLASH_B",
	"SFX_SPLASH_C",
	"SFX_SPLASH_D",
	"SFX_SPLASH_E",
	"SFX_SPLASH_F"
};

void Apu::shutdown() {
	for (int s = (SoundEffect)0; s < SoundEffect::MAX_SOUNDEFFECT; s++) {
		Apu::unload(SoundEffect(s));
	}
	Kore::runlevel--;
}

bool Apu::initialize() {
	try {
		if (!al_install_audio()) return false;
		if (!al_init_acodec_addon()) {
			al_uninstall_audio();
			return false;
		}
		if (!al_reserve_samples(SoundEffect::MAX_SOUNDEFFECT)) {
			al_uninstall_audio();
			return false;
		}   
    
		// Load sounds
		for (int s = (SoundEffect)0; s < SoundEffect::MAX_SOUNDEFFECT; s++) {
		    if (!Apu::load((SoundEffect)s)) return false;
		}
		Kore::runlevel++;
		return true;		
	} catch (int e) {e;
	    return false;
	}
}

bool Apu::load(SoundEffect sound) {
	std::string str = std::string("data/sfx/")+ Apu::sfx_filename[int(sound)] + std::string(".wav");
    ALLEGRO_SAMPLE *sample = al_load_sample( str.c_str() );
	if (!sample) {
		return false;
	}
	Apu::sample[sound] = sample;
	return true;
}

void Apu::play(SoundEffect sound) {
	if (!Apu::sample[sound]) return;
	float panning = 0.5;
	al_play_sample(
		Apu::sample[sound],
		0.5f + (TRUERAND(15) / 100.0f),
		(TRUERAND(20)/10.0f)-1.0f,
		0.75f+(TRUERAND(25)/100.0f),// (TRUERAND(20)/10.0f)-1.0
		ALLEGRO_PLAYMODE_ONCE,
		NULL
    );
}

void Apu::unload(SoundEffect sound) {
	if (!Apu::sample[sound]) return;
	al_destroy_sample(Apu::sample[sound]);
	Apu::sample[sound] = nullptr;
}
	