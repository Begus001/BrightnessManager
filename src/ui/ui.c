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

static display_config_t *display_configs;
static program_config_t *program_config;

static unsigned int current_monitor_index = 0;
static bool current_monitor_changed = false;

static void refresh_ui_display_settings()
{
	current_monitor_changed = true;
	gtk_spin_button_set_value(spbt_sunset_hour, display_configs[current_monitor_index].sunset_hour);
	gtk_spin_button_set_value(spbt_sunset_min, display_configs[current_monitor_index].sunset_min);
	gtk_spin_button_set_value(
		spbt_sunrise_hour, display_configs[current_monitor_index].sunrise_hour);
	gtk_spin_button_set_value(spbt_sunrise_min, display_configs[current_monitor_index].sunrise_min);
	gtk_spin_button_set_value(
		spbt_night_brightness, display_configs[current_monitor_index].night_brightness);
	gtk_spin_button_set_value(
		spbt_day_brightness, display_configs[current_monitor_index].day_brightness);
	current_monitor_changed = false;
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

	refresh_ui_display_settings();
}

void btApplyToAll_clicked_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: btApplyToAll_clicked\n");
}

void spbtSunsetHour_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (current_monitor_changed)
		return;

	cfg_notify_display_configs_changed();

	display_configs[current_monitor_index].sunset_hour
		= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	printf("UI: spbtSunsetHour_value_changed %d\n",
		display_configs[current_monitor_index].sunset_hour);
}

void spbtSunsetMinute_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (current_monitor_changed)
		return;

	cfg_notify_display_configs_changed();

	display_configs[current_monitor_index].sunset_min
		= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	printf("UI: spbtSunsetMinute_value_changed %d\n",
		display_configs[current_monitor_index].sunset_min);
}

void spbtSunriseHour_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (current_monitor_changed)
		return;

	cfg_notify_display_configs_changed();

	display_configs[current_monitor_index].sunrise_hour
		= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	printf("UI: spbtSunriseHour_value_changed %d\n",
		display_configs[current_monitor_index].sunrise_hour);
}

void spbtSunriseMinute_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (current_monitor_changed)
		return;

	cfg_notify_display_configs_changed();

	display_configs[current_monitor_index].sunrise_min
		= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	printf("UI: spbtSunriseMinute_value_changed %d\n",
		display_configs[current_monitor_index].sunrise_min);
}

void spbtNightBrightness_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (current_monitor_changed)
		return;

	cfg_notify_display_configs_changed();

	display_configs[current_monitor_index].night_brightness
		= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	printf("UI: spbtNightBrightness_value_changed %d\n",
		display_configs[current_monitor_index].night_brightness);
}

void spbtDayBrightness_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (current_monitor_changed)
		return;

	cfg_notify_display_configs_changed();

	display_configs[current_monitor_index].day_brightness
		= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	printf("UI: spbtDayBrightness_value_changed %d\n",
		display_configs[current_monitor_index].day_brightness);
}

void spbtMonitor_value_changed_cb(GtkWidget *widget, gpointer data)
{
	current_monitor_index = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget)) - 1;

	refresh_ui_display_settings();

	printf("UI: spbtMonitor_value_changed %d\n", current_monitor_index);
}

void spbtMaxMonitors_value_changed_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: spbtMaxMonitors_value_changed\n");
}

void spbtUpdateInterval_value_changed_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: spbtUpdateInterval_value_changed\n");
}

void spbtI2C1_value_changed_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: spbtI2C1_value_changed\n");
}

void spbtI2C2_value_changed_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: spbtI2C2_value_changed\n");
}

void spbtI2C3_value_changed_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: spbtI2C3_value_changed\n");
}

void spbtI2C4_value_changed_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: spbtI2C4_value_changed\n");
}

void spbtI2C5_value_changed_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: spbtI2C5_value_changed\n");
}

void spbtI2C6_value_changed_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: spbtI2C6_value_changed\n");
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

	display_configs = cfg_get_display_configs();
	program_config = cfg_get_program_config();

	gtk_builder_connect_signals(builder, NULL);

	gtk_window_present(win_main);
	gtk_main();
}
