#ifndef HELPMENUHANDLER_H
#define HELPMENUHANDLER_H

#include <QObject>
#include "ui_MainWindow.h"

class HelpMenuHandler : public QObject
{
	Q_OBJECT

	Ui::MainWindow *			ui;

public:
	explicit HelpMenuHandler(Ui::MainWindow *_ui, QObject *parent = 0);

signals:

public slots:
	void		aboutQt		();
	void		about		();
};

#endif // HELPMENUHANDLER_H
