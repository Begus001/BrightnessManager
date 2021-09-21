#include <stdbool.h>
#include <stdio.h>

#include "../config/config.h"
#include "../display/brightness_manager.h"
#include "ui.h"

static GtkWindow *win_main;
static GtkBuilder *builder;

static GtkSpinButton *spbt_sunset_hour;
static GtkSpinButton *spbt_sunset_min;
static GtkSpinButton *spbt_sunrise_hour;
static GtkSpinButton *spbt_sunrise_min;
static GtkSpinButton *spbt_night_brightness;
static GtkSpinButton *spbt_day_brightness;
static GtkSpinButton *spbt_monitor;

static GtkSpinButton *spbt_manual_monitor;

static GtkSpinButton *spbt_max_monitors;
static GtkSpinButton *spbt_update_interval;
static GtkSpinButton *spbt_i2c1;
static GtkSpinButton *spbt_i2c2;
static GtkSpinButton *spbt_i2c3;
static GtkSpinButton *spbt_i2c4;
static GtkSpinButton *spbt_i2c5;
static GtkSpinButton *spbt_i2c6;

static display_config_t *display_configs;
static program_config_t *program_config;

static unsigned int current_monitor_index = 0;
static bool config_changed = false;

static void refresh_config()
{
	config_changed = true;

	gtk_spin_button_set_value(spbt_sunset_hour, display_configs[current_monitor_index].sunset_hour);
	gtk_spin_button_set_value(spbt_sunset_min, display_configs[current_monitor_index].sunset_min);
	gtk_spin_button_set_value(
		spbt_sunrise_hour, display_configs[current_monitor_index].sunrise_hour);
	gtk_spin_button_set_value(spbt_sunrise_min, display_configs[current_monitor_index].sunrise_min);
	gtk_spin_button_set_value(
		spbt_night_brightness, display_configs[current_monitor_index].night_brightness);
	gtk_spin_button_set_value(
		spbt_day_brightness, display_configs[current_monitor_index].day_brightness);

	gtk_spin_button_set_value(spbt_max_monitors, program_config->max_displays);
	gtk_spin_button_set_value(spbt_update_interval, program_config->update_interval);
	gtk_spin_button_set_value(spbt_i2c1, program_config->i2c1);
	gtk_spin_button_set_value(spbt_i2c2, program_config->i2c2);
	gtk_spin_button_set_value(spbt_i2c3, program_config->i2c3);
	gtk_spin_button_set_value(spbt_i2c4, program_config->i2c4);
	gtk_spin_button_set_value(spbt_i2c5, program_config->i2c5);
	gtk_spin_button_set_value(spbt_i2c6, program_config->i2c6);

	GtkAdjustment *adj = gtk_spin_button_get_adjustment(spbt_monitor);
	gtk_adjustment_set_upper(adj, program_config->max_displays);
	adj = gtk_spin_button_get_adjustment(spbt_manual_monitor);
	gtk_adjustment_set_upper(adj, program_config->max_displays);

	config_changed = false;
}

void winMain_destroy_cb(GtkWidget *widget, gpointer data) { gtk_main_quit(); }

void btApply_clicked_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: btApply_clicked\n");

	cfg_notify_display_configs_applied();
}

void btCancel_clicked_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: btCancel_clicked\n");

	cfg_notify_display_configs_cancelled();

	refresh_config();
}

void btApplyToAll_clicked_cb(GtkWidget *widget, gpointer data)
{
	cfg_apply_to_all(current_monitor_index);
	
	printf("UI: btApplyToAll_clicked\n");
}

void spbtSunsetHour_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	cfg_notify_display_configs_changed();

	display_configs[current_monitor_index].sunset_hour
		= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	printf("UI: spbtSunsetHour_value_changed %d\n",
		display_configs[current_monitor_index].sunset_hour);
}

void spbtSunsetMinute_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	cfg_notify_display_configs_changed();

	display_configs[current_monitor_index].sunset_min
		= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	printf("UI: spbtSunsetMinute_value_changed %d\n",
		display_configs[current_monitor_index].sunset_min);
}

void spbtSunriseHour_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	cfg_notify_display_configs_changed();

	display_configs[current_monitor_index].sunrise_hour
		= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	printf("UI: spbtSunriseHour_value_changed %d\n",
		display_configs[current_monitor_index].sunrise_hour);
}

void spbtSunriseMinute_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	cfg_notify_display_configs_changed();

	display_configs[current_monitor_index].sunrise_min
		= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	printf("UI: spbtSunriseMinute_value_changed %d\n",
		display_configs[current_monitor_index].sunrise_min);
}

void spbtNightBrightness_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	cfg_notify_display_configs_changed();

	display_configs[current_monitor_index].night_brightness
		= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	printf("UI: spbtNightBrightness_value_changed %d\n",
		display_configs[current_monitor_index].night_brightness);
}

void spbtDayBrightness_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	cfg_notify_display_configs_changed();

	display_configs[current_monitor_index].day_brightness
		= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	printf("UI: spbtDayBrightness_value_changed %d\n",
		display_configs[current_monitor_index].day_brightness);
}

void spbtMonitor_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	current_monitor_index = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget)) - 1;

	refresh_config();

	printf("UI: spbtMonitor_value_changed %d\n", current_monitor_index);
}

void spbtMaxMonitors_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	program_config->max_displays = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	cfg_notify_program_config_changed();

	gtk_spin_button_set_value(spbt_monitor, 1);
	gtk_spin_button_set_value(spbt_manual_monitor, 1);

	printf("UI: spbtMaxMonitors_value_changed\n");
}

void spbtUpdateInterval_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	program_config->update_interval = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	cfg_notify_program_config_changed();

	printf("UI: spbtUpdateInterval_value_changed\n");
}

void spbtI2C1_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	program_config->i2c1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	cfg_notify_program_config_changed();

	printf("UI: spbtI2C1_value_changed\n");
}

void spbtI2C2_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	program_config->i2c2 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	cfg_notify_program_config_changed();

	printf("UI: spbtI2C2_value_changed\n");
}

void spbtI2C3_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	program_config->i2c3 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	cfg_notify_program_config_changed();

	printf("UI: spbtI2C3_value_changed\n");
}

void spbtI2C4_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	program_config->i2c4 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	cfg_notify_program_config_changed();

	printf("UI: spbtI2C4_value_changed\n");
}

void spbtI2C5_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	program_config->i2c5 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	cfg_notify_program_config_changed();

	printf("UI: spbtI2C5_value_changed\n");
}

void spbtI2C6_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	program_config->i2c6 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	cfg_notify_program_config_changed();

	printf("UI: spbtI2C6_value_changed\n");
}

void btAutoDetect_clicked_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: btAutoDetect_clicked\n");

	cfg_auto_detect_i2c();

	refresh_config();
}

void swEnabled_state_set_cb(GtkWidget *widget, gpointer data)
{
	gboolean state;
	gtk_switch_set_state(GTK_SWITCH(widget), state = !gtk_switch_get_state(GTK_SWITCH(widget)));

	bm_set_enabled(state);

	printf("UI: swEnabled_state_set %d\n", state);
}

void btApplyManual_clicked_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: btApplyManual_clicked\n");
}

void spbtManualBrightness_activate_cb(GtkWidget *widget, gpointer data)
{
	btApplyManual_clicked_cb(widget, data);
}

void ui_init()
{
	builder = gtk_builder_new_from_file("src/ui/winMain.glade");
	win_main = GTK_WINDOW(gtk_builder_get_object(builder, "winMain"));

	spbt_sunset_hour = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtSunsetHour"));
	spbt_sunset_min = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtSunsetMinute"));
	spbt_sunrise_hour = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtSunriseHour"));
	spbt_sunrise_min = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtSunriseMinute"));
	spbt_night_brightness = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtNightBrightness"));
	spbt_day_brightness = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtDayBrightness"));
	spbt_monitor = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtMonitor"));

	spbt_manual_monitor = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtManualMonitor"));

	spbt_max_monitors = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtMaxMonitors"));
	spbt_update_interval = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtUpdateInterval"));
	spbt_i2c1 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtI2C1"));
	spbt_i2c2 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtI2C2"));
	spbt_i2c3 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtI2C3"));
	spbt_i2c4 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtI2C4"));
	spbt_i2c5 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtI2C5"));
	spbt_i2c6 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtI2C6"));

	display_configs = cfg_get_display_configs();
	program_config = cfg_get_program_config();

	gtk_builder_connect_signals(builder, NULL);

	refresh_config();

	gtk_window_present(win_main);
	gtk_main();
}
