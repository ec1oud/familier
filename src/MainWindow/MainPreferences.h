#ifndef MAINPREFERENCES_H
#define MAINPREFERENCES_H

#include <QDialog>
#include "ui_MainPreferences.h"

namespace Ui
{
	class MainPreferences;
}

class MainPreferences : public QDialog
{
	Q_OBJECT

public:
	explicit MainPreferences(QWidget *parent = 0);
	~MainPreferences();

protected:
	void changeEvent(QEvent *e);

public:
	Ui::MainPreferences *ui;
};

#endif // MAINPREFERENCES_H
