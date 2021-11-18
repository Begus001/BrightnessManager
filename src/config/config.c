#include "cJSON.h"
#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../ui/ui.h"
#include "config.h"

static display_config_t display_configs[MAX_DISPLAYS], display_configs_backup[MAX_DISPLAYS];
static program_config_t program_config;

static pthread_t unhide_listener;

static bool display_configs_changed = false;

static char *cfg_file_path;
static char *cfg_file_dir;
static char *ui_path;
static char *pipe_file;

static bool hiding_enabled = true;

display_config_t *cfg_get_display_configs() { return display_configs; }

program_config_t *cfg_get_program_config() { return &program_config; }

static char *load_from_file()
{
	FILE *file = fopen(cfg_file_path, "r");

	assert(file && "Couldn't load file from config path");

	char *buf;
	unsigned int size;

	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	rewind(file);

	buf = malloc(size);

	fread(buf, size, 1, file);

	fclose(file);

	printf("CFG: Loaded configuration from file %s\n", cfg_file_path);

	return buf;
}

static void write_to_file(cJSON *json)
{
	FILE *file = fopen(cfg_file_path, "w+");

	if (!file) {
		fprintf(stderr, "\e[91mERROR: Couldn't load config file\nDid you forget to run the application as root?\n\e[0m");
		exit(1);
	}

	printf("CFG: Writing configuration to file %s\n", cfg_file_path);

	fputs(cJSON_Print(json), file);
	fclose(file);
}

void cfg_notify_program_config_changed()
{
	char *buf = load_from_file();
	char val_buf[16];

	cJSON *main = cJSON_Parse(buf);
	cJSON *program_config_json = cJSON_GetObjectItemCaseSensitive(main, "program-config");

	assert(main && program_config_json && "JSON parse error");

	cJSON *max_displays = cJSON_GetObjectItemCaseSensitive(program_config_json, "max-displays");
	cJSON *update_interval
		= cJSON_GetObjectItemCaseSensitive(program_config_json, "update-interval");
	cJSON *fade_duration = cJSON_GetObjectItemCaseSensitive(program_config_json, "fade-duration");
	cJSON *i2c1 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c1");
	cJSON *i2c2 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c2");
	cJSON *i2c3 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c3");
	cJSON *i2c4 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c4");
	cJSON *i2c5 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c5");
	cJSON *i2c6 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c6");

	assert(max_displays && update_interval && fade_duration && i2c1 && i2c2 && i2c3 && i2c4 && i2c5
		&& i2c6 && "JSON parse error");

	sprintf(val_buf, "%d", program_config.max_displays);
	cJSON_SetValuestring(max_displays, val_buf);
	sprintf(val_buf, "%d", program_config.update_interval);
	cJSON_SetValuestring(update_interval, val_buf);
	sprintf(val_buf, "%d", program_config.fade_duration);
	cJSON_SetValuestring(fade_duration, val_buf);
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

void cfg_apply_to_all(int display_id_to_copy)
{
	cfg_notify_display_configs_changed();

	for (int i = 0; i < MAX_DISPLAYS; i++) {
		if (i == display_id_to_copy)
			continue;

		display_configs[i] = display_configs[display_id_to_copy];
	}
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

	assert(main && display_config_array && "JSON parse error");

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
			&& day_brightness && "JSON parse error");

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

	assert(main && display_config_array && program_config_json && "JSON parse error");

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
			&& day_brightness && "JSON parse error");

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
	cJSON *fade_duration = cJSON_GetObjectItemCaseSensitive(program_config_json, "fade-duration");
	cJSON *i2c1 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c1");
	cJSON *i2c2 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c2");
	cJSON *i2c3 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c3");
	cJSON *i2c4 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c4");
	cJSON *i2c5 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c5");
	cJSON *i2c6 = cJSON_GetObjectItemCaseSensitive(program_config_json, "i2c6");

	assert(max_displays && update_interval && fade_duration && i2c1 && i2c2 && i2c3 && i2c4 && i2c5 && i2c6
		&& "JSON parse error");

	program_config.max_displays = atoi(cJSON_GetStringValue(max_displays));
	program_config.update_interval = atoi(cJSON_GetStringValue(update_interval));
	program_config.fade_duration = atoi(cJSON_GetStringValue(fade_duration));
	program_config.i2c1 = atoi(cJSON_GetStringValue(i2c1));
	program_config.i2c2 = atoi(cJSON_GetStringValue(i2c2));
	program_config.i2c3 = atoi(cJSON_GetStringValue(i2c3));
	program_config.i2c4 = atoi(cJSON_GetStringValue(i2c4));
	program_config.i2c5 = atoi(cJSON_GetStringValue(i2c5));
	program_config.i2c6 = atoi(cJSON_GetStringValue(i2c6));

	printf("CFG: Parsed configuration\n");

	free(buf);
}

static void create_default_config_file()
{
	cJSON *main = cJSON_CreateObject();
	cJSON *display_config_array = cJSON_CreateArray();
	cJSON *display_config_items[MAX_DISPLAYS];
	cJSON *program_config_json = cJSON_CreateObject();

	assert(main && display_config_array && program_config_json && "Error creating JSON config");

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
	cJSON_AddStringToObject(program_config_json, "fade-duration", DEFAULT_FADE_DURATION);
	cJSON_AddStringToObject(program_config_json, "i2c1", DEFAULT_I2C);
	cJSON_AddStringToObject(program_config_json, "i2c2", DEFAULT_I2C);
	cJSON_AddStringToObject(program_config_json, "i2c3", DEFAULT_I2C);
	cJSON_AddStringToObject(program_config_json, "i2c4", DEFAULT_I2C);
	cJSON_AddStringToObject(program_config_json, "i2c5", DEFAULT_I2C);
	cJSON_AddStringToObject(program_config_json, "i2c6", DEFAULT_I2C);

	write_to_file(main);

	printf("CFG: Created file %s\n", cfg_file_path);

	load_config();

	cJSON_free(main);
	cJSON_free(display_config_array);

	for (int i = 0; i < MAX_DISPLAYS; i++) {
		cJSON_free(display_config_items[i]);
	}
}

void cfg_auto_detect_i2c()
{
	FILE *pipe;
	char cmd_output[4096] = "";
	char buf[128] = "";

	printf("CFG: Auto-detecting I2C busses\n");

	pipe = popen("sudo ddccontrol -p 2>/dev/null | grep \"Device: dev:/dev/i2c-\"", "r");

	assert(pipe && "Couldn't open pipe to process (auto-detection failed)");

	while (fgets(buf, sizeof(buf), pipe)) {
		strcat(cmd_output, buf);
	}

	char *p = strtok(cmd_output, "-\n");

	int i = 1, k = I2C_OFFSET, n = 0;
	do {
		if (!(i % 3)) {
			printf("  %d: %s\n", k - 1, p);
			unsigned int *cfg_ptr = (unsigned int *)&program_config;
			cfg_ptr[k++] = atoi(p);
			n++;
		}

		p = strtok(NULL, "-\n");
		i++;
	} while (p);

	program_config.max_displays = n;

	cfg_notify_program_config_changed();

	pclose(pipe);
}

static void get_config_path()
{
	cfg_file_dir = malloc(sizeof(CFG_PATH));
	strcpy(cfg_file_dir, CFG_PATH);

	cfg_file_path = malloc(strlen(cfg_file_dir) + sizeof(CFG_FILENAME));
	strcpy(cfg_file_path, cfg_file_dir);
	strcat(cfg_file_path, CFG_FILENAME);

	ui_path = malloc(strlen(cfg_file_dir) + sizeof(UI_FILENAME));
	strcpy(ui_path, cfg_file_dir);
	strcat(ui_path, UI_FILENAME);

	pipe_file = malloc(strlen(cfg_file_dir) + sizeof(PIPE_FILE));
	strcpy(pipe_file, cfg_file_dir);
	strcat(pipe_file, PIPE_FILE);

	if (access(cfg_file_dir, F_OK)) { // If path exists -> false
		printf("CFG: Couldn't access system paths, reverting to portable mode\n");
		cfg_file_path = CFG_FILENAME;
		pipe_file = PIPE_FILE;
		ui_path = UI_FILENAME;
	}

	printf("CFG: Paths:\n");
	printf("  cfg_file_path: %s\n", cfg_file_path);
	printf("  ui_path:       %s\n", ui_path);
	printf("  pipe_file:     %s\n", pipe_file);
	printf("\n");
}

static void *unhide_worker()
{
	int num = 0;
	char buf[1024];
	int fd = open(pipe_file, O_RDONLY);
	while (true) {
		do {
			num = read(fd, buf, sizeof(buf));
			usleep(100000);
		} while (num <= 0);

		if (!strcmp(buf, PIPE_OPEN_MSG)) {
			printf("CFG: Got opening signal\n");
			ui_show_win_main();
		}
		memset(buf, 0, sizeof(buf));
	}

	close(fd);

	return NULL;
}

static void check_pipe_file()
{
	if (access(pipe_file, F_OK)) {
		if (mkfifo(pipe_file, 0660)) {
			printf("CFG: Could not access or create pipe file at %s, window hiding disabled\n",
				pipe_file);
			hiding_enabled = false;
			return;
		}
	}
}

char *cfg_get_ui_path() { return ui_path; }

bool cfg_hiding_is_enabled() { return hiding_enabled; }

static bool check_already_running()
{
	char buf[32] = "";
	char cmd_output[1024] = "";
	char pid[1024];

	sprintf(pid, "%d\n", getpid());

	FILE *pipe = popen("pgrep " PROC_COMM_NAME, "r");

	assert(pipe && "Couldn't open pipe to process (check instance failed)");

	printf("CFG: Checking if another instance already running\n");

	while (fgets(buf, sizeof(buf), pipe)) {
		strcat(cmd_output, buf);
	}

	if (!strcmp(cmd_output, pid)) {
		printf("CFG: No running instance found\n");
		return false;
	} else if (!strcmp(cmd_output, "\n")) {
		printf("CFG: No running instance found\n");
		return false;
	} else {
		printf("CFG: Found running instance\n");
		return true;
	}

	fclose(pipe);
}

static void send_show_signal()
{
	if (access(pipe_file, F_OK)) {
		printf("CFG: Could not find pipe file, close the running instance and start a new one "
			   "instead\n");
		return;
	}

	printf("CFG: Sending open signal to running instance\n");

	int fd = open(pipe_file, O_WRONLY);
	write(fd, PIPE_OPEN_MSG, sizeof(PIPE_OPEN_MSG));
	close(fd);
}

void cfg_init()
{
	get_config_path();
	check_pipe_file();

	if (check_already_running()) {
		send_show_signal();
		usleep(1000000);
		if (check_already_running()) { // Stupid bug where GTK crashes due to SEGV when calling
									   // gtk_widget_show, so it's just checking if it crashed and
									   // launching another instance
			exit(0);
		}
	}

	pthread_create(&unhide_listener, NULL, unhide_worker, NULL);

	FILE *file = fopen(cfg_file_path, "r+");

	if (file == NULL) {
		create_default_config_file();
		cfg_auto_detect_i2c();
	}

	load_config();
}
