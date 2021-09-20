#ifndef BRIGHTNESS_MANAGER_H
#define BRIGHTNESS_MANAGER_H

#include <gtk/gtk.h>
#include <stdbool.h>

#include "../config/config.h"

void bm_init();

void bm_set_update_interval(unsigned int val);
void bm_set_enabled(bool val);

#endif
