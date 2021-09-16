#include <gtk/gtk.h>
#include <stdio.h>

#include "ui.h"
#include "../display/brightness_manager.h"

GtkWindow *win_main;
GtkBuilder *builder;

void winMain_destroy_cb(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

void btApply_clicked_cb(GtkWidget *widget, gpointer data)
{
	printf("btApply_clicked\n");
}

void swEnabled_state_set_cb(GtkWidget *widget, gpointer data)
{
	gboolean state;
	gtk_switch_set_state(GTK_SWITCH(widget), state = !gtk_switch_get_state(GTK_SWITCH(widget)));

	bm_set_enabled(state);

	printf("swEnabled_state_set %d\n", state);
}

void btApplyManual_clicked_cb(GtkWidget *widget, gpointer data)
{
	printf("btApplyManual_clicked\n");
}

void spbtManualBrightness_activate_cb(GtkWidget *widget, gpointer data)
{
	printf("spbtManualBrightness_activate\n");
}

void ui_init()
{
	builder = gtk_builder_new_from_file("src/ui/winMain.glade");
	win_main = GTK_WINDOW(gtk_builder_get_object(builder, "winMain"));

	gtk_builder_connect_signals(builder, NULL);

	gtk_window_present(win_main);
	gtk_main();
}
