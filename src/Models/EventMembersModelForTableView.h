#ifndef EVENTMEMBERSMODELFORTABLEVIEW_H
#define EVENTMEMBERSMODELFORTABLEVIEW_H

#include <QSortFilterProxyModel>
#include "Event.h"

class EventMembersModelForTableView : public QSortFilterProxyModel
{
	Q_OBJECT

	Event *			_event;

public:
	explicit EventMembersModelForTableView(Event *event, QObject *parent = 0);

	virtual QModelIndex 	mapFromSource	(const QModelIndex& sourceIndex) const;
	virtual QModelIndex	mapToSource	(const QModelIndex& proxyIndex) const;
	virtual Qt::ItemFlags	flags		(const QModelIndex &index) const;
	virtual QVariant	headerData	(int section, Qt::Orientation orientation, int role) const;
	virtual int		columnCount	(const QModelIndex& parent) const;
	virtual QVariant	data		(const QModelIndex &index, int role) const;
	virtual bool		setData		(const QModelIndex &index, const QVariant &value, int role);
signals:

public slots:

};

#endif // EVENTMEMBERSMODELFORTABLEVIEW_H
