#include "EventModelForComboBox.h"
#include "TreeItem.h"
#include "Event.h"

EventModelForComboBox::EventModelForComboBox(QObject *parent) :
		QSortFilterProxyModel(parent)
{
}

QModelIndex EventModelForComboBox::mapFromSource(const QModelIndex &sourceIndex) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();

	return index(sourceIndex.row() + 1, sourceIndex.column(), sourceIndex.parent());
}

QModelIndex EventModelForComboBox::mapToSource(const QModelIndex &proxyIndex) const
{
	if (!proxyIndex.isValid() || proxyIndex.row() == 0)
		return QModelIndex();

	return sourceModel()->index(proxyIndex.row() - 1, proxyIndex.column(), proxyIndex.parent());
}

QModelIndex EventModelForComboBox::index (int row, int column, const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	if (row < 0 || row -1 >= Event::s_EventList.count())
		return QModelIndex();

	if (row == 0)
		return createIndex(row, column, (void *)0);

	return createIndex(row, column, Event::s_EventList.at(row - 1));
}

QModelIndex EventModelForComboBox::parent (const QModelIndex& index) const
{
	Q_UNUSED(index);
	return QModelIndex();
}

int EventModelForComboBox::rowCount (const QModelIndex& parent) const
{
	if (!parent.isValid())
		return Event::s_EventList.count() + 1;

	return 0;
}

int EventModelForComboBox::columnCount (const QModelIndex& parent) const
{
	Q_UNUSED (parent)
	return sourceModel()->columnCount();
}

QVariant EventModelForComboBox::data (const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() == 0)
		return QVariant();

	return sourceModel()->data(mapToSource(index), role);
}

bool EventModelForComboBox::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid() && index.row() == 0)
		return false;

	return sourceModel()->setData(mapToSource(index), value, role);
}

QVariant EventModelForComboBox::headerData(int section, Qt::Orientation orientation, int role) const
{
	return sourceModel()->headerData(section, orientation, role);
}
