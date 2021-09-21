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

void bm_init()
{
	enabled = false;

	display_configs = cfg_get_display_configs();
	program_config = cfg_get_program_config();

	pthread_create(&worker, NULL, brightness_worker, NULL);
}
