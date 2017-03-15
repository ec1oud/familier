#include "HelpMenuHandler.h"
#include <QApplication>
#include <QMessageBox>
#include "SettingsManager.h"

HelpMenuHandler::HelpMenuHandler(Ui::MainWindow *_ui, QObject *parent)
	: QObject(parent), ui(_ui)
{
	connect (ui->action_About_Qt, SIGNAL(triggered()), this, SLOT(aboutQt()));
	connect (ui->action_About, SIGNAL(triggered()), this, SLOT(about()));
}

void HelpMenuHandler::aboutQt()
{
	QApplication::beep();
	QApplication::aboutQt ();
}

void HelpMenuHandler::about()
{
	QApplication::beep();
	QMessageBox::about(dynamic_cast<QWidget*>(this),
			   "About " + SettingsManager::s_FamilierApplicationName,
			   "<b>" + SettingsManager::s_FamilierApplicationName +
			   "</b> is a (traditional) Family Tree (Genealogy) Software. "
			      "Copyright (c) <a href=\"mailto:" +
			      SettingsManager::s_FamilierAuthorEmail + "\">" +
			      SettingsManager::s_FamilierAuthor + "</a> 2009-" +
			      QDate::currentDate().toString("yy") + ".<br><br>" +
			      SettingsManager::s_FamilierApplicationName +
			      " version is <b>" + SettingsManager::s_FamilierVersion +
			      "</b>.<br>" + SettingsManager::s_FamilierApplicationName +
			      " was compiled with Qt version <b>" QT_VERSION_STR "</b>."
			      "<br>You are currently using Qt version <b>" + QString (qVersion()) +
			      "</b>.<br>The build date was <b>" __DATE__ "</b>."
			      "<br><br><a href=\"http://www.onebilliondolts.dom\">One Billion Dolts</a> "
			      "[http://www.onebilliondolts.com/]"
			      "<br><a href=\"http://www.sojaare.com/\">Sojaa Re</a> "
			      "[http://www.sojaare.com]"
			      "<br><br>The program is provided AS IS with NO WARRANTY OF ANY KIND, "
			      "INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR "
			      "A PARTICULAR PURPOSE. If it works for you, you are just too damn lucky.");
}
