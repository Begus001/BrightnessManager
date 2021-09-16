#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "brightness_manager.h"
#include "display.h"

gboolean bm_enabled = false;

static pthread_t worker;

void bm_set_enabled(gboolean val)
{
	bm_enabled = val;
}

static void *brightness_worker()
{
	while (true) {
		usleep(100000);
		if (bm_enabled) {
			// code
		}
	}

	return NULL;
}

void bm_init()
{
	bm_enabled = false;
	pthread_create(&worker, NULL, brightness_worker, NULL);
}
