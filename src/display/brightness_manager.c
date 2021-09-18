#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "../config/config.h"
#include "../ui/ui.h"
#include "brightness_manager.h"

static pthread_t worker;

gboolean bm_enabled = false;

void bm_set_enabled(gboolean val) { bm_enabled = val; }

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
		if (bm_enabled) {
			if (difftime(time(NULL), start) >= DEFAULT_UPDATE_INTERVAL) {
				start = time(NULL);
				update();
			}
		}
	}

	return NULL;
}

void bm_init()
{
	bm_enabled = false;

	pthread_create(&worker, NULL, brightness_worker, NULL);
}
