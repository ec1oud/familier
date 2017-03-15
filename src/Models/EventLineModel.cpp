#include "EventLineModel.h"

EventLineModel::EventLineModel(QObject *parent) :
		QSortFilterProxyModel(parent)
{
}

QModelIndex EventLineModel::mapFromSource (const QModelIndex &sourceIndex) const
{
	if (!sourceIndex.isValid ())
		return QModelIndex();

	EventModel *em = dynamic_cast<EventModel*>(sourceModel ());
	if (!em)
		return QModelIndex();
	return createIndex (sourceIndex.row (), sourceIndex.column (),
			    em->_indexToEventMap.value (sourceIndex.row ()));
}

QModelIndex EventLineModel::mapToSource (const QModelIndex &proxyIndex) const
{
	if (!proxyIndex.isValid ())
		return QModelIndex();

	if (proxyIndex.column () >= EventModel::colMaximum)
		return QModelIndex();

	return sourceModel ()->index (proxyIndex.row (), proxyIndex.column (),
				      mapToSource (proxyIndex.parent ()));
}

QModelIndex EventLineModel::index (int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid ())
		return QModelIndex();

	EventModel *em = dynamic_cast<EventModel*>(sourceModel ());
	if (!em)
		return QModelIndex();

	return createIndex (row, column, em->_indexToEventMap.value (row));
}

QModelIndex EventLineModel::parent (const QModelIndex &child) const
{
	Q_UNUSED(child)

	return QModelIndex();
}

QVariant EventLineModel::headerData (int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch (section)
		{
		case e_col_Year: return "Year";
		case e_col_Month: return "Month";
		case e_col_Day: return "Day";
		default: break;
		}
	}

	return sourceModel ()->headerData (section, orientation, role);
}

int EventLineModel::columnCount (const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return e_col_Maximum;
}

QVariant EventLineModel::data (const QModelIndex &index, int role) const
{
	if (index.column () >= e_col_Year)
	{
		if (role == Qt::DisplayRole)
		{
			EventModel *em = dynamic_cast<EventModel*>(sourceModel ());
			if (!em)
				return QVariant();

			switch (index.column ())
			{
			case e_col_Year:
				return em->_indexToEventMap.value (index.row ())->getDated ().year ();
			case e_col_Month:
				return em->_indexToEventMap.value (index.row ())->getDated ().month ();
			case e_col_Day:
				return em->_indexToEventMap.value (index.row ())->getDated ().day ();
			default: break;
			}
		}
		return QVariant();
	}

	if (index.column () == EventModel::colName && role == Qt::ToolTipRole)
	{
		Event *event = static_cast<Event*>(index.internalPointer ());
		return event->getEventInformation ();
	}

	return sourceModel ()->data (mapToSource (index), role);
}
