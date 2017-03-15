#ifndef STATUSBARMANAGER_H
#define STATUSBARMANAGER_H

#include <QObject>
#include "ui_MainWindow.h"
#include <QLabel>
#include "MovingTextLabel.h"
#include <QSystemTrayIcon>
#include <QAction>

class StatusBarManager : public QObject
{
	Q_OBJECT

	QLabel *		_countsLabel;
	MovingTextLabel *	_todaysEventLabel;
	Ui::MainWindow *	ui;
	bool			_hasSystemTray;
	QSystemTrayIcon *	_systemTrayIcon;
	QAction *		_showMainWindowAction;

public:
	explicit StatusBarManager(Ui::MainWindow *_ui, QObject *parent = 0);

	MovingTextLabel*	getTodaysEventLabel		() const;
	void			setDocked			(bool docked);

private:
	void			setShowMainWindowActionText	();
	void			enableSystemTray		();
	void			disableSystemTray		();

signals:
	void			showMainWindow			();
	void			hideMainWindow			();

public slots:
	void			setSystemTrayHidden		(bool show);
	void			updateItemCountInStatusBar	();
	void			updateTodayEventInStatusBar	();
	void			handleSystemTrayActivation (QSystemTrayIcon::ActivationReason reason);
	void			emitShowMainWindow		();
};

#endif // STATUSBARMANAGER_H
