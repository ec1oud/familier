#ifndef EVENTMODEL_H
#define EVENTMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QStringList>

#include "Individual.h"
#include "Event.h"

class EventModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	enum Columns { colName,
		       colDate,
		       colPlace,
		       colIsMember,
		       colMembers,
		       colDescription,
		       colMaximum };

	Individual		*_individual;
	QMap<int, Event*>	_indexToEventMap;

public:
	explicit EventModel(Individual *individual = 0, QObject *parent = 0);

	QModelIndex		index			(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex		parent			(const QModelIndex &index) const;

	bool			hasChildren		(const QModelIndex &parent = QModelIndex()) const;
	int			rowCount		(const QModelIndex &parent = QModelIndex()) const;
	int			columnCount		(const QModelIndex &parent = QModelIndex()) const;

	QVariant		headerData		(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	Qt::ItemFlags		flags			(const QModelIndex &index) const;
	QVariant		data			(const QModelIndex &index, int role) const;
	bool			setData			(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	bool			insertRow		(int row, const QModelIndex &parent = QModelIndex());
	bool			removeRow		(int row, const QModelIndex &parent = QModelIndex());

	void			setIndividual		(Individual *individual) { _individual = individual; }
	void			populateModel		();
	int			getIndexFromId		(const IDTYPE eid) const;
	Event*			getEventFromIndex	(const int idx) const;

signals:
	void			eventModelRegenerated ();

public slots:
	void			regenerateModel		();
	void			clear			();
};

#endif // EVENTMODEL_H
