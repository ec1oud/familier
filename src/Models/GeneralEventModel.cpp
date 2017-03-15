#include "GeneralEventModel.h"
#include "EventModel.h"

GeneralEventModel::GeneralEventModel(QObject *parent) :
		QSortFilterProxyModel(parent)
{
}

QModelIndex GeneralEventModel::mapFromSource (const QModelIndex &sourceIndex) const
{
	if (!sourceIndex.isValid ())
		return QModelIndex();

	int col = sourceIndex.column ();

	if (col == EventModel::colIsMember)
		return QModelIndex();

	if (col > EventModel::colIsMember)
		col --;

	return index (sourceIndex.row (), col, sourceIndex.parent ());
}

QModelIndex GeneralEventModel::mapToSource (const QModelIndex &proxyIndex) const
{
	if (!proxyIndex.isValid ())
		return QModelIndex();

	int col = proxyIndex.column ();
	if (col >= EventModel::colIsMember)
		col ++;

	return sourceModel ()->index (proxyIndex.row (), col, proxyIndex.parent ());
}

QVariant GeneralEventModel::headerData (int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Vertical && role == Qt::DisplayRole)
		return section + 1;

	int sec = section;
	if (section >= EventModel::colIsMember)
		sec ++;

	return sourceModel ()->headerData (sec, orientation, role);
}

int GeneralEventModel::columnCount (const QModelIndex &parent) const
{
	return sourceModel ()->columnCount (parent) - 1;
}

void GeneralEventModel::handleMemberListChange () const
{
	emit memberListChanged();
}
