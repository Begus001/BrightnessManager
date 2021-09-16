#include <stdlib.h>
#include <string.h>
#include "display.h"

bm_display_t *bm_display_new()
{
	bm_display_t *new_display = malloc(sizeof(bm_display_t));
	memset(new_display, 0, sizeof(bm_display_t));
	return new_display;
}

void bm_display_destroy(bm_display_t *self)
{
	free(self);
}

void bm_display_set_sunset(bm_display_t *self, time_t sunset)
{
	self->sunset = sunset;
}

void bm_display_set_sunrise(bm_display_t *self, time_t sunrise)
{
	self->sunrise = sunrise;
}

void bm_display_set_day_brightness(bm_display_t *self, unsigned int brightness)
{
	self->day_brightness = brightness;
}

void bm_display_set_night_brightness(bm_display_t *self, unsigned int brightness)
{
	self->night_brightness = brightness;
}

time_t bm_display_get_sunset(bm_display_t *self)
{
	return self->sunset;
}

time_t bm_display_get_sunrise(bm_display_t *self)
{
	return self->sunrise;
}

unsigned int bm_display_get_day_brightness(bm_display_t *self)
{
	return self->day_brightness;
}

unsigned int bm_display_get_night_brightness(bm_display_t *self)
{
	return self->night_brightness;
}
