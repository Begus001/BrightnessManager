#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "../config/config.h"
#include "../ui/ui.h"
#include "brightness_manager.h"

static pthread_t worker;

static display_config_t *display_configs;
static program_config_t *program_config;

bool enabled = false;

void bm_set_enabled(bool val) { enabled = val; }

static void set_brightness_cmd(unsigned int i2c, unsigned int brightness)
{
	char cmd[64];

	sprintf(cmd, "ddccontrol -r 0x10 -w %d dev:/dev/i2c-%d 2>/dev/null", brightness, i2c);

	FILE *pipe = popen(cmd, "r");

	assert(pipe && "Couldn't open pipe to process (setting brightness failed)");

	pclose(pipe);
}

void bm_set_brightness(unsigned int display, unsigned int brightness)
{
	if (brightness > 100)
		brightness = 100;
	else if (brightness < 0)
		brightness = 0;

	printf("BM: Setting brightness %d for display %d\n", brightness, display + 1);

	unsigned int *p = (unsigned int *)program_config + I2C_OFFSET;
	set_brightness_cmd(p[display], brightness);
}

static unsigned int calculate_brightness_taper_sunset(
	double t, double taper, unsigned int display, bool invert)
{
	if (!invert)
		return (t / taper)
			* (display_configs[display].day_brightness - display_configs[display].night_brightness)
			+ display_configs[display].night_brightness;
	else
		return ((taper - t) / taper)
			* (display_configs[display].day_brightness - display_configs[display].night_brightness)
			+ display_configs[display].night_brightness;
}

static void update()
{
	printf("BM: Updating brightness\n");

	time_t now = time(NULL);

	double times[MAX_DISPLAYS][2];

	for (int i = 0; i < program_config->max_displays; i++) {
		struct tm buf = *(localtime(&now));

		buf.tm_hour = display_configs[i].sunset_hour;
		buf.tm_min = display_configs[i].sunset_min;
		buf.tm_sec = 0;

		times[i][0] = difftime(mktime(&buf), now);
		if (times[i][0] < 0) {
			times[i][0] += 60 * 60 * 24;
		}

		buf.tm_hour = display_configs[i].sunrise_hour;
		buf.tm_min = display_configs[i].sunrise_min;
		buf.tm_sec = 0;

		times[i][1] = difftime(mktime(&buf), now);
		if (times[i][1] < 0) {
			times[i][1] += 60 * 60 * 24;
		}

		if (times[i][0] <= program_config->fade_duration * 60) {
			printf("BM: Tapering to night brightness\n");
			bm_set_brightness(
				i, calculate_brightness_taper_sunset(times[i][0], program_config->fade_duration * 60, i, false));
		} else if (times[i][1] <= program_config->fade_duration * 60) {
			printf("BM: Tapering to day brightness\n");
			bm_set_brightness(
				i, calculate_brightness_taper_sunset(times[i][1], program_config->fade_duration * 60, i, true));
		} else if (times[i][0] < times[i][1]) {
			printf("BM: Daytime\n");
			bm_set_brightness(i, display_configs[i].day_brightness);
		} else {
			printf("BM: Nighttime\n");
			bm_set_brightness(i, display_configs[i].night_brightness);
		}
	}
}

static void *brightness_worker()
{
	time_t start = time(NULL);
	while (true) {
		usleep(1000);
		if (enabled) {
			if (difftime(time(NULL), start) >= program_config->update_interval) {
				start = time(NULL);
				update();
			}
		}
	}

	return NULL;
}

void bm_init()
{
	enabled = true;

	display_configs = cfg_get_display_configs();
	program_config = cfg_get_program_config();

	update();

	pthread_create(&worker, NULL, brightness_worker, NULL);
}
