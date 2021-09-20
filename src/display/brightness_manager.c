#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "../config/config.h"
#include "../ui/ui.h"
#include "brightness_manager.h"

static pthread_t worker;

unsigned int update_interval = 1;
bool enabled = false;

void bm_set_update_interval(unsigned int val) { update_interval = val; }

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
			if (difftime(time(NULL), start) >= update_interval) {
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

	pthread_create(&worker, NULL, brightness_worker, NULL);
}
