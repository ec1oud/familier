#include "Application.h"
#include <QSessionManager>
#include "SettingsManager.h"

Application::Application(int &argc, char *argv[]) :
		QApplication (argc, argv)
{
}

void Application::commitData (QSessionManager &sm)
{
	if (sm.allowsInteraction())
	{
		_mainWindow->commitData (sm);
	}
	else
	{
		// we did not get permission to interact, then
		// do something reasonable instead
	}
}

void Application::saveState (QSessionManager &sm)
{
	SettingsManager::saveState (sm, _mainWindow->getOpenedItemsAsUrls ());
}

void Application::setMainWindow (MainWindow *mainWindow)
{
	_mainWindow = mainWindow;
}
