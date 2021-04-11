
#include "audio.h"
#include "embedded.h"
#include "utils.h"
#include "dbgmsg.h"
#include <limits.h>
#include <SDL2/SDL.h>

sound_t g_sound_pew = {0};
sound_t g_sound_boom = {0};
sound_t g_sound_die = {0};

static unsigned int s_audio_device_number = 0;
static SDL_AudioDeviceID* s_audio_device_id_array = NULL;
static unsigned int s_audio_device_next_index = 0;

int init_g_audio(void)
{
	SDL_AudioSpec audio_spec;

	#define BUILD_SOUND(sound_, wav_, wav_size_) \
		do { \
			SDL_RWops* rw = SDL_RWFromConstMem(wav_, wav_size_); \
			if (rw == NULL) \
			{ \
				error_sdl2_fail("SDL_RWFromConstMem on \"" #wav_ "\""); \
				return -1; \
			} \
			if (SDL_LoadWAV_RW(rw, 0, &audio_spec, \
				&sound_.buffer, &sound_.length) != &audio_spec) \
			{ \
				error_sdl2_fail( \
					"SDL_LoadWAV_RW on \"" #wav_ "\"'s SDL_RWops"); \
				return -1; \
			} \
		} while(0)

	BUILD_SOUND(g_sound_pew, g_wav_pew, g_wav_pew_size);
	BUILD_SOUND(g_sound_boom, g_wav_boom, g_wav_boom_size);
	BUILD_SOUND(g_sound_die, g_wav_die, g_wav_die_size);

	#undef BUILD_SOUND

	/* Reducing the size of the buffer slices sent to the hardware seems to
	 * eliminate the random delays that occured on one's machine. */
	audio_spec.samples = 512;

	s_audio_device_number = 8;
	s_audio_device_id_array = xmalloc(
		s_audio_device_number * sizeof(SDL_AudioDeviceID));
	for (unsigned int i = 0; i < s_audio_device_number; i++)
	{
		s_audio_device_id_array[i] = SDL_OpenAudioDevice(
			NULL, 0, &audio_spec, NULL, 0);
		if (s_audio_device_id_array[i] == 0)
		{
			error_sdl2_fail("SDL_OpenAudioDevice");
			return -1;
		}
	}

	return 0;
}

static void free_sound(sound_t* sound)
{
	SDL_FreeWAV(sound->buffer);
	sound->buffer = NULL;
	sound->length = 0;
}

void cleanup_g_audio(void)
{
	free_sound(&g_sound_pew);
	free_sound(&g_sound_boom);
	free_sound(&g_sound_die);

	for (unsigned int i = 0; i < s_audio_device_number; i++)
	{
		SDL_CloseAudioDevice(s_audio_device_id_array[i]);
	}
}

void play_sound(sound_t sound)
{
	unsigned int min_queued_audio_size = UINT_MAX;
	unsigned int min_index;

	#define SEARCH_MIN_SIZE_QUEUED(inf_, sup_) \
		do { \
			for (unsigned int i = inf_; \
				i < sup_ && min_queued_audio_size != 0; i++) \
			{ \
				unsigned int i_queued_audio_size = \
					SDL_GetQueuedAudioSize(s_audio_device_id_array[i]); \
				if (min_queued_audio_size > i_queued_audio_size) \
				{ \
					min_queued_audio_size = i_queued_audio_size; \
					min_index = i; \
				} \
			} \
		} while(0)

	SEARCH_MIN_SIZE_QUEUED(s_audio_device_next_index, s_audio_device_number);
	SEARCH_MIN_SIZE_QUEUED(0, s_audio_device_next_index);

	#undef SEARCH_MIN_SIZE_QUEUED

	SDL_ClearQueuedAudio(s_audio_device_id_array[min_index]);
	int success_uwu = SDL_QueueAudio(s_audio_device_id_array[min_index],
		sound.buffer, sound.length);
	SDL_PauseAudioDevice(s_audio_device_id_array[min_index], SDL_FALSE);
	if (min_index == s_audio_device_next_index)
	{
		s_audio_device_next_index =
			(s_audio_device_next_index+1) % s_audio_device_number;
	}
}
