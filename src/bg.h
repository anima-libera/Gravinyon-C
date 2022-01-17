
#ifndef GRAVINYON_HEADER_BG_
#define GRAVINYON_HEADER_BG_

typedef void generic_bg_data_t;

struct bg_description_t
{
	generic_bg_data_t* (*init)(void);
	void (*cleanup)(generic_bg_data_t* data);
	void (*iter)(generic_bg_data_t* data);
	void (*render)(generic_bg_data_t* data);
};
typedef struct bg_description_t bg_description_t;

extern const bg_description_t g_bg_description_black;
extern const bg_description_t g_bg_description_blue_clouds_and_stars;

#endif /* GRAVINYON_HEADER_BG_ */
