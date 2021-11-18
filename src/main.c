#include <gtk/gtk.h>

#include "display/brightness_manager.h"
#include "ui/ui.h"
#include "config/config.h"

int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);
	gtk_init(&argc, &argv);

	cfg_init();
	bm_init();
	ui_init();

	return 0;
}
