#ifndef CONFIG_H
#define CONFIG_H

#include <gtk/gtk.h>

#define MAX_DISPLAYS 6

#define CFG_PATH "/etc/brightness-manager/"
#define CFG_FILENAME "config.json"
#define UI_FILENAME "winMain.glade"
#define PIPE_FILE "bm-pipe"

#define PROC_COMM_NAME "brightness-mana"

#define PIPE_OPEN_MSG "open\n"

#define DEFAULT_SUNSET_HOUR "20"
#define DEFAULT_SUNSET_MIN "0"
#define DEFAULT_SUNRISE_HOUR "6"
#define DEFAULT_SUNRISE_MIN "0"
#define DEFAULT_NIGHT_BRIGHTNESS "0"
#define DEFAULT_DAY_BRIGHTNESS "100"
#define DEFAULT_MAX_DISPLAYS "6"
#define DEFAULT_UPDATE_INTERVAL "15"
#define DEFAULT_FADE_DURATION "60"
#define DEFAULT_OPEN_HIDDEN "0"
#define DEFAULT_I2C "0"

#define I2C_OFFSET 4

typedef struct {
	unsigned int sunset_hour;
	unsigned int sunset_min;
	unsigned int sunrise_hour;
	unsigned int sunrise_min;
	unsigned int night_brightness;
	unsigned int day_brightness;
} display_config_t;

typedef struct {
	unsigned int max_displays;
	unsigned int update_interval;
	unsigned int fade_duration;
	unsigned int open_hidden;
	unsigned int i2c1;
	unsigned int i2c2;
	unsigned int i2c3;
	unsigned int i2c4;
	unsigned int i2c5;
	unsigned int i2c6;
} program_config_t;

bool cfg_is_window_hidden();
void cfg_set_window_hidden(bool val);

display_config_t *cfg_get_display_configs();
program_config_t *cfg_get_program_config();

void cfg_notify_program_config_changed();

void cfg_apply_to_all(int display_id_to_copy);

void cfg_notify_display_configs_changed();
void cfg_notify_display_configs_applied();
void cfg_notify_display_configs_cancelled();

void cfg_auto_detect_i2c();

char *cfg_get_ui_path();
char *cfg_get_config_dir();

void cfg_init();

#endif
