#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "../config/config.h"
#include "../ui/ui.h"
#include "brightness_manager.h"

static pthread_t worker;

static display_config_t *display_configs;
static program_config_t *program_config;

bool enabled = false;

void bm_set_enabled(bool val) { enabled = val; }

static void update()
{
	printf("test\n");
	fflush(stdout);
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

static void set_brightness(unsigned int i2c, unsigned int brightness)
{
	char cmd[64];

	sprintf(cmd, "sudo ddccontrol -r 0x10 -w %d dev:/dev/i2c-%d 2>/dev/null", brightness, i2c);

	FILE *pipe = popen(cmd, "r");

	assert(pipe);

	pclose(pipe);
}

void bm_set_manual_brightness(unsigned int display, unsigned int brightness)
{
	unsigned int *p = (unsigned int *)program_config + I2C_OFFSET;
	set_brightness(p[display - 1], brightness);
}

void bm_init()
{
	enabled = false;

	display_configs = cfg_get_display_configs();
	program_config = cfg_get_program_config();

	pthread_create(&worker, NULL, brightness_worker, NULL);
}
