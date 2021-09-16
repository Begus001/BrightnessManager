#include <gtk/gtk.h>

#include "display/brightness_manager.h"
#include "ui/ui.h"

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	bm_init();
	ui_init();

	return 0;
}
