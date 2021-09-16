#ifndef DISPLAY_H
#define DISPLAY_H

#include <time.h>

typedef struct bm_display {
	time_t sunset;
	time_t sunrise;
	unsigned int day_brightness;
	unsigned int night_brightness;
} bm_display_t;

bm_display_t *bm_display_new();
void bm_display_destroy(bm_display_t *self);

void bm_display_set_sunset(bm_display_t *self, time_t sunset);
void bm_display_set_sunrise(bm_display_t *self, time_t sunrise);
void bm_display_set_day_brightness(bm_display_t *self, unsigned int brightness);
void bm_display_set_night_brightness(bm_display_t *self, unsigned int brightness);

time_t bm_display_get_sunset(bm_display_t *self);
time_t bm_display_get_sunrise(bm_display_t *self);
unsigned int bm_display_get_day_brightness(bm_display_t *self);
unsigned int bm_display_get_night_brightness(bm_display_t *self);

#endif
