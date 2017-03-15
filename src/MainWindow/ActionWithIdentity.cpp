#include "ActionWithIdentity.h"

ActionWithIdentity::ActionWithIdentity(unsigned long ID, QObject *parent)
		: QAction(parent)
{
	m_ID = ID;
	connect(this, SIGNAL(triggered()), this, SLOT(editEntry()));
}

//ActionWithIdentity::~ActionWithIndentity()
//{
//}

void ActionWithIdentity::editEntry()
{
	emit triggered(m_ID);
}
