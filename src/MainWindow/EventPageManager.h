#ifndef EVENTPAGEMANAGER_H
#define EVENTPAGEMANAGER_H

#include <QObject>
#include "ui_MainWindow.h"
#include "EventModel.h"

class EventPageManager : public QObject
{
	Q_OBJECT

	Ui::MainWindow *		ui;
	EventModel*			_eventModel;

public:
	explicit EventPageManager(EventModel *eventModel, Ui::MainWindow *_ui, QObject *parent = 0);

signals:
	void		documentModified();

public slots:
	void		addEvent	();
	void		delEvent	();
};

#endif // EVENTPAGEMANAGER_H
