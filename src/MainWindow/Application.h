#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include "MainWindow.h"

class Application : public QApplication
{
	Q_OBJECT

	MainWindow *		_mainWindow;

public:
	explicit Application (int &argc, char *argv[]);

	virtual void		commitData		(QSessionManager &sm);
	virtual void		saveState		(QSessionManager &sm);
	void			setMainWindow		(MainWindow *mainWindow);

signals:

public slots:

};

#endif // APPLICATION_H
