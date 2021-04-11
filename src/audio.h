
#ifndef GRAVINYON_HEADER_AUDIO_
#define GRAVINYON_HEADER_AUDIO_

#include <SDL2/SDL.h>

struct sound_t
{
	Uint8* buffer;
	Uint32 length;
};
typedef struct sound_t sound_t;

extern sound_t g_sound_pew;
extern sound_t g_sound_boom;
extern sound_t g_sound_die;

int init_g_audio(void);

void cleanup_g_audio(void);

void play_sound(sound_t sound);

#endif /* GRAVINYON_HEADER_AUDIO_ */
