#include <stdbool.h>
#include <string.h>

#include "config.h"

static display_config_t display_configs[MAX_DISPLAYS], display_configs_backup[MAX_DISPLAYS];
static program_config_t program_config;

static bool display_configs_changed = false;

static void save_to_file()
{

}

static void load_config_file()
{
	
}

static void set_defaults()
{
	
}

display_config_t *cfg_get_display_configs() { return display_configs; }

program_config_t *cfg_get_program_config() { return &program_config; }

void cfg_notify_display_configs_changed()
{
	if (display_configs_changed)
		return;

	display_configs_changed = true;
	memcpy(display_configs_backup, display_configs, sizeof(display_configs));

	printf("CFG: Display config changed, created backup\n");
}

void cfg_notify_display_configs_applied()
{
	if (!display_configs_changed)
		return;
	
	display_configs_changed = false;

	printf("CFG: Display config change applied\n");
}

void cfg_notify_display_configs_cancelled()
{
	if (!display_configs_changed)
		return;

	memcpy(display_configs, display_configs_backup, sizeof(display_configs_backup));
	display_configs_changed = false;

	printf("CFG: Display config change cancelled, reverted to backup\n");
}

void cfg_init()
{
	// TODO: load from file
	for (int i = 0; i < MAX_DISPLAYS; i++) {
		display_configs[i].sunset_hour = 20;
		display_configs[i].sunset_min = 00;
		display_configs[i].sunrise_hour = 06;
		display_configs[i].sunrise_min = 00;
		display_configs[i].night_brightness = 0;
		display_configs[i].day_brightness = 100;
	}

	program_config.max_displays = 3;
	program_config.update_interval = DEFAULT_UPDATE_INTERVAL;
	program_config.i2c1 = 0;
	program_config.i2c2 = 0;
	program_config.i2c3 = 0;
	program_config.i2c4 = 0;
	program_config.i2c5 = 0;
	program_config.i2c6 = 0;
}
