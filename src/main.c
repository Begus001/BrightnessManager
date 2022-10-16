#include <gtk/gtk.h>

#include "display/brightness_manager.h"
#include "ui/ui.h"
#include "config/config.h"

static void startup(GtkApplication *app, gpointer data)
{
	cfg_init();
	bm_init();
	ui_init(app);
}

static void activate(GtkApplication *app, gpointer data)
{
	ui_show();
}

int main(int argc, char *argv[])
{
	GtkApplication *app = gtk_application_new("net.ddns.begus.brightness-manager", G_APPLICATION_FLAGS_NONE);

	g_signal_connect(app, "startup", G_CALLBACK(startup), NULL);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return 0;
}
