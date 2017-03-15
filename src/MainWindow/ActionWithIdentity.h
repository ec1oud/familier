#ifndef ACTIONWITHIDENTITY_H
#define ACTIONWITHIDENTITY_H

#include <QAction>

class ActionWithIdentity : public QAction
{
	Q_OBJECT

private:
	unsigned long m_ID;
public:
	ActionWithIdentity(unsigned long ID, QObject *parent);
	//~ActionWithIndentity();
	unsigned long getId() { return m_ID; }

public slots:
	void editEntry();

signals:
	void triggered(unsigned long entry);
};

#endif // ACTIONWITHIDENTITY_H
