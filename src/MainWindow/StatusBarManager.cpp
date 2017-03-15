#include "StatusBarManager.h"
#include "Family.h"
#include "MaleIndividual.h"
#include "FemaleIndividual.h"
#include "Utilities.h"
#include "SettingsManager.h"
#include <QTimer>

StatusBarManager::StatusBarManager(Ui::MainWindow *_ui, QObject *parent)
	: QObject(parent), ui(_ui), _hasSystemTray (false),
	  _systemTrayIcon (new QSystemTrayIcon (parent))
{
	_todaysEventLabel = new MovingTextLabel (dynamic_cast<QWidget*>(parent));
	_todaysEventLabel->setFrameStyle (QFrame::Panel | QFrame::Sunken);
	ui->statusBar->addPermanentWidget (_todaysEventLabel);
	_todaysEventLabel->setStaticText ("<b>Today's Events:</b> ");

	_countsLabel = new QLabel (dynamic_cast<QWidget*>(parent));
	_countsLabel->setFrameStyle (QFrame::Panel | QFrame::Sunken);
	ui->statusBar->addPermanentWidget (_countsLabel);

	ui->statusBar->setSizeGripEnabled (true);

	updateTodayEventInStatusBar ();
	updateItemCountInStatusBar ();

	QTimer *timer = new QTimer (this);
	connect (timer, SIGNAL(timeout()), this, SLOT(updateTodayEventInStatusBar()));
	timer->start (600000);

	if (SettingsManager::s_EnableSystemTray && QSystemTrayIcon::isSystemTrayAvailable ())
		enableSystemTray ();
}

MovingTextLabel* StatusBarManager::getTodaysEventLabel () const
{
	return _todaysEventLabel;
}

void StatusBarManager::setDocked (bool docked)
{
	SettingsManager::s_MainWindowIsVisible = !docked;
	setShowMainWindowActionText ();
}

void StatusBarManager::setShowMainWindowActionText ()
{
	if (SettingsManager::s_MainWindowIsVisible)
		_showMainWindowAction->setText ("&Hide Main Window");
	else
		_showMainWindowAction->setText ("&Show Main Window");
}

void StatusBarManager::enableSystemTray ()
{
	if (!_hasSystemTray)
	{
		_hasSystemTray = true;

		QMenu *systemTrayMenu = new QMenu (dynamic_cast <QWidget*>(parent ()));

		_showMainWindowAction = new QAction (dynamic_cast <QWidget*>(parent ()));
		setShowMainWindowActionText ();
		connect (_showMainWindowAction, SIGNAL(triggered()),
			 this, SLOT(emitShowMainWindow()));
		systemTrayMenu->addAction (_showMainWindowAction);
		systemTrayMenu->addAction (ui->action_LockOrUnlock);

		systemTrayMenu->addSeparator ();
		systemTrayMenu->addAction (ui->action_Preferences);
		systemTrayMenu->addSeparator ();
		systemTrayMenu->addAction (ui->action_About);
		systemTrayMenu->addAction (ui->action_About_Qt);
		systemTrayMenu->addSeparator ();
		systemTrayMenu->addAction (ui->action_Exit);

		_systemTrayIcon->setIcon (QIcon (":images/familier.png"));
		_systemTrayIcon->setContextMenu (systemTrayMenu);
		_systemTrayIcon->show ();

		connect (_systemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			 this, SLOT(handleSystemTrayActivation(QSystemTrayIcon::ActivationReason)));
	}

	updateTodayEventInStatusBar ();
}

void StatusBarManager::disableSystemTray ()
{
	_hasSystemTray = false;
	_systemTrayIcon->hide ();
}

void StatusBarManager::setSystemTrayHidden (bool show)
{
	if (show)
		enableSystemTray ();
	else
		disableSystemTray ();
}

void StatusBarManager::updateItemCountInStatusBar()
{
	int familyCount = Family::s_FamilyList.count();
	if (!familyCount)
	{
		_countsLabel->setText ("Family Tree is empty!");
		return;
	}
	int baseFamilyCount = Family::s_NameToBaseFamilyMap.count();
	int individualCount = Individual::s_IndividualList.count() - Individual::s_DummyIndividualCount;
	int eventCount = Event::s_EventList.count();
	int unionCount = Union::s_UnionList.count();
	_countsLabel->setText(QString("<b>Tree statistics:-</b> Families: %1 (%2), "
				     "Individuals: %3 (%4), Events: %5, and Unions: %6")
			     .arg(familyCount).arg(baseFamilyCount).arg(individualCount)
			     .arg(Individual::s_IndividualList.count ()).arg(eventCount)
			     .arg(unionCount));
	_countsLabel->setToolTip (QString(
			"<table>"
			"<tr><th colspan='4'><h3>Tree Statistics</h3></th></tr>"
			"<tr><th colspan='4'>Family</th></tr>"
			"<tr><td><b>Total:</b></td><td>%1</td>"
			"<td><b>Base:</b></td><td>%2</td></tr>"
			"<tr><th colspan='4'>Individuals</th></tr>"
			"<tr><td><b>Total:</b></td><td>%3</td>"
			"<td><b>Real:</b></td><td>%4</td></tr>"
			"<tr><td><b>Male:</b></td><td>%5</td>"
			"<td><b>Female:</b></td><td>%6</td></tr>"
			"<tr><th colspan='4'>Other</th></tr>"
			"<tr><td><b>Events:</td><td>%7</td>"
			"<td><b>Unions:</td><td>%8</td></tr>"
			"</table>")
				 .arg(familyCount)
				 .arg(baseFamilyCount)
				 .arg(Individual::s_IndividualList.count ())
				 .arg(individualCount)
				 .arg(MaleIndividual::s_MaleIndividualCount)
				 .arg(FemaleIndividual::s_FemaleIndividualCount)
				 .arg(eventCount)
				 .arg(unionCount));
}

void StatusBarManager::updateTodayEventInStatusBar ()
{
	QString todayEventText;
	QString systemTrayTodayEventText;
	QString systemTrayMessage;
	QList <Event *> todaysEventList;
	foreach (Event *event, Event::s_EventList)
		if (event->didEventHappenToday ())
			todaysEventList << event;

	if (todaysEventList.size ())
	{
		todayEventText += todaysEventList[0]->getSpecialEventName (Event::e_Ids);
		QString todayEventSpecialName = todaysEventList[0]->getSpecialEventName ();

		systemTrayMessage = todayEventSpecialName;
		if (todaysEventList.size () > 1)
			systemTrayTodayEventText += "<ol><li>" + todayEventSpecialName + "</li>";
		else
			systemTrayTodayEventText += todayEventSpecialName;

		for (int i = 1; i < todaysEventList.size (); i++)
		{
			todayEventText += ", " + todaysEventList[i]->getSpecialEventName (Event::e_Ids);
			QString todayEventSpecialName = todaysEventList[i]->getSpecialEventName ();
			systemTrayTodayEventText += "<li>" + todayEventSpecialName + "</li>";
			systemTrayMessage += ", " + todayEventSpecialName;
		}

		if (todaysEventList.size () > 1)
			systemTrayTodayEventText += "</ol>";
	}

	if (todayEventText.isEmpty ())
	{
		QDate today = QDate::currentDate ();
		todayEventText = QString ("No events for %1<sup>%2</sup> %3.")
				.arg (today.toString ("d"))
				.arg (Dated::getDayth (today.day ()))
				.arg (today.toString ("MMMM"));
	}

	_todaysEventLabel->setMovingText ("=> " + todayEventText);

	if (_hasSystemTray)
	{
		QString convertedTodayEventText;
		QString systemTrayToolTip = "<h1>" + SettingsManager::s_DocumentTitle + "</h1>";
		systemTrayToolTip += _countsLabel->text ();
		systemTrayToolTip += "<h2>Today's events</h2>";
		if (systemTrayTodayEventText.size () > 1)
		{
			convertedTodayEventText = systemTrayMessage;
			systemTrayToolTip += systemTrayTodayEventText;
		}
		else
		{
			convertedTodayEventText = todayEventText;
			systemTrayToolTip += convertedTodayEventText;
		}
		QRegExp htmlTagMatcher ("<.*>");
		htmlTagMatcher.setMinimal (true);

		convertedTodayEventText.replace (htmlTagMatcher, "");

		_systemTrayIcon->showMessage ("Today's events", convertedTodayEventText);
		_systemTrayIcon->setToolTip (systemTrayToolTip);
	}
}

void StatusBarManager::handleSystemTrayActivation (QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::DoubleClick)
		emitShowMainWindow ();
}

void StatusBarManager::emitShowMainWindow ()
{
	if (SettingsManager::s_MainWindowIsVisible)
		emit hideMainWindow ();
	else
		emit showMainWindow ();
}
