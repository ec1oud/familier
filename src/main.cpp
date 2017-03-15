#include "MainWindow.h"
#include "Application.h"

int main(int argc, char *argv[])
{
	Application a(argc, argv);
	MainWindow w;
	a.setMainWindow (&w);
	w.show();
	w.startupBasedOnSettings ();
	return a.exec();
}
