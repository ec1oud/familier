#include "EventMembersModelForTableView.h"
#include "TreeItem.h"
#include "Individual.h"

EventMembersModelForTableView::EventMembersModelForTableView(Event *event, QObject *parent)
	: QSortFilterProxyModel(parent), _event (event)
{
}

QModelIndex EventMembersModelForTableView::mapFromSource (const QModelIndex &sourceIndex) const
{
	if (!sourceIndex.isValid ())
		return QModelIndex();

	if (sourceIndex.column () > 0)
		return QModelIndex();

	return index (sourceIndex.row (), sourceIndex.column (), sourceIndex.parent ());
}

QModelIndex EventMembersModelForTableView::mapToSource (const QModelIndex &proxyIndex) const
{
	if (!proxyIndex.isValid ())
		return QModelIndex();

	return sourceModel ()->index (proxyIndex.row (), proxyIndex.column (), proxyIndex.parent ());
}

Qt::ItemFlags EventMembersModelForTableView::flags (const QModelIndex &index) const
{
	switch (index.column ())
	{
	case 0: return Qt::ItemIsEnabled;
	case 1: return Qt::ItemIsEnabled | Qt::ItemIsEditable;
	default: break;
	}

	return 0;
}

QVariant EventMembersModelForTableView::headerData (int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && section == 1)
		return "Is Member";

	return sourceModel ()->headerData (section, orientation, role);
}

int EventMembersModelForTableView::columnCount (const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return 2;
}

QVariant EventMembersModelForTableView::data (const QModelIndex &index, int role) const
{
	if (index.column () == 1)
	{
		QModelIndex srcIndex = mapToSource (index);
		TreeItem *item = static_cast<TreeItem*>(srcIndex.internalPointer ());
		IDTYPE id = item->id ();

		if (role == Qt::DisplayRole)
		{
			if (_event->hasEventMember (id))
				return "True";
			else
				return "False";
		}
		else if (role == Qt::EditRole)
		{
			if (_event->hasEventMember (id))
				return true;
			else
				return false;
		}

		return QVariant ();
	}

	return sourceModel ()->data (mapToSource (index), role);
}

bool EventMembersModelForTableView::setData (const QModelIndex &index, const QVariant &value, int role)
{
	if (index.column () == 1)
	{
		if (role == Qt::EditRole)
		{
			QModelIndex srcIndex = mapToSource (index);
			TreeItem *item = static_cast<TreeItem*>(srcIndex.internalPointer ());
			IDTYPE id = item->id ();

			bool success = false;
			if (true == value.toBool ())
				success = _event->appendChild (Individual::s_IdToIndividualHash.value (id));
			else
				success = _event->removeChild (id);

			if (success)
				emit dataChanged (index, index);
		}
		else
			return false;
	}

	return sourceModel ()->setData (mapToSource (index), value, role);
}
