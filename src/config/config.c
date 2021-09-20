#include <assert.h>
#include <cjson/cJSON.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

static display_config_t display_configs[MAX_DISPLAYS], display_configs_backup[MAX_DISPLAYS];
static program_config_t program_config;

static bool display_configs_changed = false;

display_config_t *cfg_get_display_configs() { return display_configs; }

program_config_t *cfg_get_program_config() { return &program_config; }

static char *load_from_file()
{
	FILE *file = fopen(CONFIG_FILE_PATH, "r");

	assert(file);

	char *buf;
	unsigned int size;

	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	rewind(file);

	buf = malloc(size);

	fread(buf, size, 1, file);

	fclose(file);

	return buf;
}

static void write_to_file(cJSON *json)
{
	FILE *file = fopen(CONFIG_FILE_PATH, "w+");

	assert(file);

	printf("CFG: Writing configuration to file\n");

	fputs(cJSON_Print(json), file);
	fclose(file);
}

void cfg_notify_program_config_changed()
{
	char *buf = load_from_file();
	char val_buf[16];

	cJSON *main = cJSON_Parse(buf);
	cJSON *program_config_json = cJSON_GetObjectItemCaseSensitive(main, "program-config");

	assert(main && program_config_json);

	cJSON *max_displays = cJSON_GetObjectItemCaseSensitive(program_config_json, "max-displays");
	cJSON *update_interval
		= cJSON_GetObjectItemCaseSensitive(program_config_json, "update-interval");
	cJSON *i2c1 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c1");
	cJSON *i2c2 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c2");
	cJSON *i2c3 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c3");
	cJSON *i2c4 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c4");
	cJSON *i2c5 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c5");
	cJSON *i2c6 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c6");

	assert(max_displays && update_interval && i2c1 && i2c2 && i2c3 && i2c4 && i2c5 && i2c6);

	sprintf(val_buf, "%d", program_config.max_displays);
	cJSON_SetValuestring(max_displays, val_buf);
	sprintf(val_buf, "%d", program_config.update_interval);
	cJSON_SetValuestring(update_interval, val_buf);
	sprintf(val_buf, "%d", program_config.i2c1);
	cJSON_SetValuestring(i2c1, val_buf);
	sprintf(val_buf, "%d", program_config.i2c2);
	cJSON_SetValuestring(i2c2, val_buf);
	sprintf(val_buf, "%d", program_config.i2c3);
	cJSON_SetValuestring(i2c3, val_buf);
	sprintf(val_buf, "%d", program_config.i2c4);
	cJSON_SetValuestring(i2c4, val_buf);
	sprintf(val_buf, "%d", program_config.i2c5);
	cJSON_SetValuestring(i2c5, val_buf);
	sprintf(val_buf, "%d", program_config.i2c6);
	cJSON_SetValuestring(i2c6, val_buf);

	printf("CFG: Program configuration changed\n");

	write_to_file(main);

	free(buf);
}

void cfg_notify_display_configs_changed()
{
	if (display_configs_changed)
		return;

	display_configs_changed = true;
	memcpy(display_configs_backup, display_configs, sizeof(display_configs));

	printf("CFG: Display configuration changed, created backup\n");
}

static void update_display_config_values()
{
	char *buf = load_from_file();
	char val_buf[16];

	cJSON *main = cJSON_Parse(buf);
	cJSON *display_config_array = cJSON_GetObjectItemCaseSensitive(main, "display-configs");
	cJSON *display_config_item = NULL;

	assert(main && display_config_array);

	int i = 0;

	cJSON_ArrayForEach(display_config_item, display_config_array)
	{
		cJSON *sunset_hour = cJSON_GetObjectItemCaseSensitive(display_config_item, "sunset-hour");
		cJSON *sunset_min = cJSON_GetObjectItemCaseSensitive(display_config_item, "sunset-min");
		cJSON *sunrise_hour = cJSON_GetObjectItemCaseSensitive(display_config_item, "sunrise-hour");
		cJSON *sunrise_min = cJSON_GetObjectItemCaseSensitive(display_config_item, "sunrise-min");
		cJSON *night_brightness
			= cJSON_GetObjectItemCaseSensitive(display_config_item, "night-brightness");
		cJSON *day_brightness
			= cJSON_GetObjectItemCaseSensitive(display_config_item, "day-brightness");

		assert(sunset_hour && sunset_min && sunrise_hour && sunrise_min && night_brightness
			&& day_brightness);

		sprintf(val_buf, "%d", display_configs[i].sunset_hour);
		cJSON_SetValuestring(sunset_hour, val_buf);
		sprintf(val_buf, "%d", display_configs[i].sunset_min);
		cJSON_SetValuestring(sunset_min, val_buf);
		sprintf(val_buf, "%d", display_configs[i].sunrise_hour);
		cJSON_SetValuestring(sunrise_hour, val_buf);
		sprintf(val_buf, "%d", display_configs[i].sunrise_min);
		cJSON_SetValuestring(sunrise_min, val_buf);
		sprintf(val_buf, "%d", display_configs[i].night_brightness);
		cJSON_SetValuestring(night_brightness, val_buf);
		sprintf(val_buf, "%d", display_configs[i].day_brightness);
		cJSON_SetValuestring(day_brightness, val_buf);

		i++;
	}

	write_to_file(main);

	free(buf);
}

void cfg_notify_display_configs_applied()
{
	if (!display_configs_changed)
		return;

	display_configs_changed = false;

	printf("CFG: Display configuration change applied\n");

	update_display_config_values();
}

void cfg_notify_display_configs_cancelled()
{
	if (!display_configs_changed)
		return;

	memcpy(display_configs, display_configs_backup, sizeof(display_configs_backup));
	display_configs_changed = false;

	printf("CFG: Display configuration change cancelled, reverted to backup\n");
}

static void load_config()
{
	char *buf = load_from_file();

	cJSON *main = cJSON_Parse(buf);
	cJSON *display_config_array = cJSON_GetObjectItemCaseSensitive(main, "display-configs");
	cJSON *display_config_item = NULL;
	cJSON *program_config_json = cJSON_GetObjectItemCaseSensitive(main, "program-config");

	assert(main && display_config_array && program_config_json);

	int i = 0;

	cJSON_ArrayForEach(display_config_item, display_config_array)
	{
		cJSON *sunset_hour = cJSON_GetObjectItemCaseSensitive(display_config_item, "sunset-hour");
		cJSON *sunset_min = cJSON_GetObjectItemCaseSensitive(display_config_item, "sunset-min");
		cJSON *sunrise_hour = cJSON_GetObjectItemCaseSensitive(display_config_item, "sunrise-hour");
		cJSON *sunrise_min = cJSON_GetObjectItemCaseSensitive(display_config_item, "sunrise-min");
		cJSON *night_brightness
			= cJSON_GetObjectItemCaseSensitive(display_config_item, "night-brightness");
		cJSON *day_brightness
			= cJSON_GetObjectItemCaseSensitive(display_config_item, "day-brightness");

		assert(sunset_hour && sunset_min && sunrise_hour && sunrise_min && night_brightness
			&& day_brightness);

		display_configs[i].sunset_hour = atoi(cJSON_GetStringValue(sunset_hour));
		display_configs[i].sunset_min = atoi(cJSON_GetStringValue(sunset_min));
		display_configs[i].sunrise_hour = atoi(cJSON_GetStringValue(sunrise_hour));
		display_configs[i].sunrise_min = atoi(cJSON_GetStringValue(sunrise_min));
		display_configs[i].night_brightness = atoi(cJSON_GetStringValue(night_brightness));
		display_configs[i].day_brightness = atoi(cJSON_GetStringValue(day_brightness));

		i++;
	}

	cJSON *max_displays = cJSON_GetObjectItemCaseSensitive(program_config_json, "max-displays");
	cJSON *update_interval
		= cJSON_GetObjectItemCaseSensitive(program_config_json, "update-interval");
	cJSON *i2c1 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c1");
	cJSON *i2c2 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c2");
	cJSON *i2c3 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c3");
	cJSON *i2c4 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c4");
	cJSON *i2c5 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c5");
	cJSON *i2c6 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c6");

	assert(max_displays && update_interval && i2c1 && i2c2 && i2c3 && i2c4 && i2c5 && i2c6);

	program_config.max_displays = atoi(cJSON_GetStringValue(max_displays));
	program_config.update_interval = atoi(cJSON_GetStringValue(update_interval));
	program_config.i2c1 = atoi(cJSON_GetStringValue(i2c1));
	program_config.i2c2 = atoi(cJSON_GetStringValue(i2c2));
	program_config.i2c3 = atoi(cJSON_GetStringValue(i2c3));
	program_config.i2c4 = atoi(cJSON_GetStringValue(i2c4));
	program_config.i2c5 = atoi(cJSON_GetStringValue(i2c5));
	program_config.i2c6 = atoi(cJSON_GetStringValue(i2c6));

	printf("CFG: Configuration loaded from file\n");

	free(buf);
}

static void create_default_config_file()
{
	cJSON *main = cJSON_CreateObject();
	cJSON *display_config_array = cJSON_CreateArray();
	cJSON *display_config_items[MAX_DISPLAYS];
	cJSON *program_config_json = cJSON_CreateObject();

	assert(main && display_config_array && program_config_json);

	printf("CFG: Creating default configuration\n");

	cJSON_AddItemToObject(main, "display-configs", display_config_array);
	cJSON_AddItemToObject(main, "program-config", program_config_json);

	for (int i = 0; i < MAX_DISPLAYS; i++) {
		display_config_items[i] = cJSON_CreateObject();
		cJSON_AddStringToObject(display_config_items[i], "sunset-hour", DEFAULT_SUNSET_HOUR);
		cJSON_AddStringToObject(display_config_items[i], "sunset-min", DEFAULT_SUNSET_MIN);
		cJSON_AddStringToObject(display_config_items[i], "sunrise-hour", DEFAULT_SUNRISE_HOUR);
		cJSON_AddStringToObject(display_config_items[i], "sunrise-min", DEFAULT_SUNRISE_MIN);
		cJSON_AddStringToObject(
			display_config_items[i], "night-brightness", DEFAULT_NIGHT_BRIGHTNESS);
		cJSON_AddStringToObject(display_config_items[i], "day-brightness", DEFAULT_DAY_BRIGHTNESS);

		cJSON_AddItemToArray(display_config_array, display_config_items[i]);
	}

	cJSON_AddStringToObject(program_config_json, "max-displays", DEFAULT_MAX_DISPLAYS);
	cJSON_AddStringToObject(program_config_json, "update-interval", DEFAULT_UPDATE_INTERVAL);
	cJSON_AddStringToObject(program_config_json, "i2c1", DEFAULT_I2C);
	cJSON_AddStringToObject(program_config_json, "i2c2", DEFAULT_I2C);
	cJSON_AddStringToObject(program_config_json, "i2c3", DEFAULT_I2C);
	cJSON_AddStringToObject(program_config_json, "i2c4", DEFAULT_I2C);
	cJSON_AddStringToObject(program_config_json, "i2c5", DEFAULT_I2C);
	cJSON_AddStringToObject(program_config_json, "i2c6", DEFAULT_I2C);

	write_to_file(main);

	cJSON_free(main);
	cJSON_free(display_config_array);

	for (int i = 0; i < MAX_DISPLAYS; i++) {
		cJSON_free(display_config_items[i]);
	}
}

void cfg_init()
{
	FILE *file = fopen(CONFIG_FILE_PATH, "r+");

	if (file == NULL) {
		create_default_config_file();
	}

	load_config();
}