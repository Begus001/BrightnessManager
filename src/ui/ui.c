#include <libayatana-appindicator/app-indicator.h>
#include <stdbool.h>
#include <stdio.h>

#include "../config/config.h"
#include "../display/brightness_manager.h"
#include "ui.h"

static GtkApplication *app;

static GtkWindow *win_main;
static GtkBuilder *builder;

static AppIndicator *indicator;
static GtkMenu *indicator_menu;
static GtkMenuItem *indicator_menu_item;

static GtkSwitch *sw_enabled;

static GtkSpinButton *spbt_sunset_hour;
static GtkSpinButton *spbt_sunset_min;
static GtkSpinButton *spbt_sunrise_hour;
static GtkSpinButton *spbt_sunrise_min;
static GtkSpinButton *spbt_night_brightness;
static GtkSpinButton *spbt_day_brightness;
static GtkSpinButton *spbt_monitor;

static GtkSpinButton *spbt_manual_monitor;
static GtkSpinButton *spbt_manual_brightness;
static GtkButton *bt_apply_manual;

static GtkSpinButton *spbt_max_monitors;
static GtkSpinButton *spbt_update_interval;
static GtkCheckButton *cb_open_hidden;
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

static void refresh_max_monitors()
{
	GtkSpinButton *spbts[6];
	spbts[0] = spbt_i2c1;
	spbts[1] = spbt_i2c2;
	spbts[2] = spbt_i2c3;
	spbts[3] = spbt_i2c4;
	spbts[4] = spbt_i2c5;
	spbts[5] = spbt_i2c6;

	for (int i = 0; i < MAX_DISPLAYS; i++) {
		if (i < program_config->max_displays)
			gtk_widget_set_sensitive(GTK_WIDGET(spbts[i]), true);
		else
			gtk_widget_set_sensitive(GTK_WIDGET(spbts[i]), false);
	}

	GtkAdjustment *adj = gtk_spin_button_get_adjustment(spbt_manual_monitor);
	gtk_adjustment_set_upper(adj, program_config->max_displays);
}

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
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb_open_hidden), program_config->open_hidden);
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

	refresh_max_monitors();

	config_changed = false;
}

void ui_show()
{
	gtk_widget_show_all(GTK_WIDGET(win_main));
	cfg_set_window_hidden(false);
	gtk_menu_item_set_label(indicator_menu_item, "Hide");
}

void ui_hide()
{
	gtk_widget_hide(GTK_WIDGET(win_main));
	cfg_set_window_hidden(true);
	gtk_menu_item_set_label(indicator_menu_item, "Show");
}

static void toggle_hide()
{
	bool hidden = cfg_is_window_hidden();
	if (hidden) {
		ui_show();
	} else {
		ui_hide();
	}
}

bool winMain_delete_event_cb(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	printf("UI: Hiding window\n");
	toggle_hide();
	return true;
}

void btApply_clicked_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: btApply_clicked\n");

	cfg_notify_display_configs_applied();

	bm_update();
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

bool pad_zeroes_cb(GtkSpinButton *spbt, gpointer data)
{
	char buf[3];
	sprintf(buf, "%02d", gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spbt)));
	gtk_entry_set_text(GTK_ENTRY(spbt), buf);
	return true;
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

	refresh_max_monitors();

	printf("UI: spbtMaxMonitors_value_changed\n");
}

void spbtFadeDuration_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	program_config->fade_duration = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	cfg_notify_program_config_changed();

	printf("UI: spbtFadeDuration_value_changed\n");
}

void spbtUpdateInterval_value_changed_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	program_config->update_interval = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

	cfg_notify_program_config_changed();

	printf("UI: spbtUpdateInterval_value_changed\n");
}

void cbOpenHidden_toggled_cb(GtkWidget *widget, gpointer data)
{
	if (config_changed)
		return;

	program_config->open_hidden = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

	cfg_notify_program_config_changed();

	printf("UI: cbOpenHidden_toggled\n");
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

bool swEnabled_state_set_cb(GtkWidget *widget, gboolean state, gpointer user_data)
{
	printf("UI: swEnabled_state_set %d\n", state);

	gtk_switch_set_state(GTK_SWITCH(widget), state);

	bm_set_enabled(state);

	return true;
}

void btApplyManual_clicked_cb(GtkWidget *widget, gpointer data)
{
	printf("UI: btApplyManual_clicked\n");

	swEnabled_state_set_cb(GTK_WIDGET(sw_enabled), false, NULL);

	unsigned int display, brightness;

	display = gtk_spin_button_get_value_as_int(spbt_manual_monitor);
	brightness = gtk_spin_button_get_value_as_int(spbt_manual_brightness);

	bm_set_brightness(display - 1, brightness);
}

void spbtManualMonitor_activate_cb(GtkWidget *widget, gpointer data)
{
	gtk_widget_grab_focus(GTK_WIDGET(bt_apply_manual));
	btApplyManual_clicked_cb(widget, data);
	gtk_widget_grab_focus(widget);
}

void spbtManualBrightness_activate_cb(GtkWidget *widget, gpointer data)
{
	gtk_widget_grab_focus(GTK_WIDGET(bt_apply_manual));
	btApplyManual_clicked_cb(widget, data);
	gtk_widget_grab_focus(widget);
}

void menu_item_activate_cb(GtkMenuItem *item, gpointer data) { toggle_hide(); }

static void quit() { exit(0); }

void setup_app_indicator()
{
	GtkMenuItem *exit_menu_item;

	indicator = app_indicator_new_with_path("brightness-manager", "icon",
		APP_INDICATOR_CATEGORY_APPLICATION_STATUS, cfg_get_config_dir());

	indicator_menu = GTK_MENU(gtk_menu_new());
	indicator_menu_item = GTK_MENU_ITEM(gtk_menu_item_new_with_label("Hide"));
	exit_menu_item = GTK_MENU_ITEM(gtk_menu_item_new_with_label("Exit"));
	gtk_menu_shell_append(GTK_MENU_SHELL(indicator_menu), GTK_WIDGET(indicator_menu_item));
	gtk_menu_shell_append(GTK_MENU_SHELL(indicator_menu), gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(indicator_menu), GTK_WIDGET(exit_menu_item));
	gtk_widget_show_all(GTK_WIDGET(indicator_menu));

	g_signal_connect(indicator_menu_item, "activate", G_CALLBACK(menu_item_activate_cb), NULL);
	g_signal_connect(exit_menu_item, "activate", G_CALLBACK(quit), NULL);

	app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
	app_indicator_set_menu(indicator, GTK_MENU(indicator_menu));
}

void ui_init(GtkApplication *_app)
{
	app = _app;

	char *ui_path;

	if (!access(cfg_get_ui_path(), F_OK)) {
		ui_path = cfg_get_ui_path();
	} else if (!access(UI_FILENAME, F_OK)) {
		ui_path = UI_FILENAME;
	} else {
		fprintf(stderr, "UI: ERROR: Couldn't find winMain.glade\n");
		exit(1);
	}

	builder = gtk_builder_new_from_file(ui_path);

	win_main = GTK_WINDOW(gtk_builder_get_object(builder, "winMain"));

	setup_app_indicator();

	sw_enabled = GTK_SWITCH(gtk_builder_get_object(builder, "swEnabled"));

	spbt_sunset_hour = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtSunsetHour"));
	spbt_sunset_min = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtSunsetMinute"));
	spbt_sunrise_hour = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtSunriseHour"));
	spbt_sunrise_min = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtSunriseMinute"));
	spbt_night_brightness = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtNightBrightness"));
	spbt_day_brightness = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtDayBrightness"));
	spbt_monitor = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtMonitor"));

	spbt_manual_monitor = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtManualMonitor"));
	spbt_manual_brightness
		= GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtManualBrightness"));
	bt_apply_manual = GTK_BUTTON(gtk_builder_get_object(builder, "btApplyManual"));

	spbt_max_monitors = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtMaxMonitors"));
	spbt_update_interval = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbtUpdateInterval"));
	cb_open_hidden = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "cbOpenHidden"));
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

	if (!program_config->open_hidden)
		gtk_window_present(win_main);
	else
		ui_hide();
	gtk_main();
}
