#include "TableModel.h"
#include "TreeModel.h"
#include "TreeItem.h"

TableModel::TableModel (QObject *parent)
			: QSortFilterProxyModel	(parent),
			  _sortColumn (0),
			  _sortOrder (Qt::AscendingOrder)
{
	populateModel ();
}

QModelIndex TableModel::mapFromSource (const QModelIndex& sourceIndex) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();

	int col = sourceIndex.column();
	TreeItem *item = static_cast<TreeItem *> (sourceIndex.internalPointer());
	if (!item->isIndividual())
		return QModelIndex();

	int nrow = _individualItemsSortedList.indexOf (item);
	if (nrow < 0)
		return QModelIndex ();
	return index (nrow, col, QModelIndex());
}

QModelIndex TableModel::mapToSource (const QModelIndex& proxyIndex) const
{
	if (!proxyIndex.isValid())
		return QModelIndex();

	int col = proxyIndex.column();
	int row = proxyIndex.row();
	if (row < 0 || row >= _individualItemsSortedList.size ())
		return QModelIndex ();

	TreeItem *item = _individualItemsSortedList.at (row);

	return dynamic_cast<TreeModel *> (sourceModel())->index (item, col);
}

QModelIndex TableModel::index (int row, int column, const QModelIndex& parent) const
{
	if (parent.isValid ())
		return QModelIndex();

	if (row < 0 || row >= _individualItemsSortedList.size ())
		return QModelIndex ();

	return createIndex(row, column, _individualItemsSortedList.at (row));
}

QModelIndex TableModel::parent (const QModelIndex& index) const
{
	Q_UNUSED(index);
	return QModelIndex();
}

int TableModel::rowCount (const QModelIndex& parent) const
{
	if (!parent.isValid())
		return _individualItemsSortedList.count ();

	return 0;
}

int TableModel::columnCount (const QModelIndex& parent) const
{
	Q_UNUSED (parent)
	return sourceModel()->columnCount();
}

QVariant TableModel::data (const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	return sourceModel()->data(mapToSource(index), role);
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return sourceModel()->headerData(section, orientation, role);
}

void TableModel::sort (int column, Qt::SortOrder order)
{
	if (column < 0 || column >= rowCount (QModelIndex ()) ||
			(_sortColumn == column && _sortOrder == order))
		return;

	_sortOrder = order;

	if (_sortColumn == column)
	{
		beginResetModel ();
		for (int i = 0; i < _individualItemsSortedList.size () / 2; i++)
			_individualItemsSortedList.swap (i, _individualItemsSortedList.size () - 1 -i);
		endResetModel ();
		return;
	}

	_sortColumn = column;

	int role = Qt::EditRole;
	if (column == 0) /// Special handling for name column
		role = Qt::DisplayRole;

	QMap <QString, TreeItem*> mapOfColumnToTreeItem;
	for (int i = 0; i < _individualItemsSortedList.size (); i++)
	{
		QVariant columnData = index (i, column, QModelIndex ()).data (role);
		QString columnDataAsString;
		if (columnData.isValid ())
		{
			columnDataAsString = columnData.toString ();
			if (columnDataAsString.toInt ())
				columnDataAsString = QString ("%1").arg (columnDataAsString, 6, '0');
		}
		mapOfColumnToTreeItem.insertMulti (columnDataAsString, _individualItemsSortedList.at (i));
	}

	beginResetModel ();
	if (order == Qt::AscendingOrder)
	{
		_individualItemsSortedList.clear ();
		for (QMap <QString, TreeItem*>::const_iterator i = mapOfColumnToTreeItem.constBegin ();
		     i != mapOfColumnToTreeItem.constEnd (); i++)
			_individualItemsSortedList << i.value ();
	}

	if (order == Qt::DescendingOrder)
	{
		_individualItemsSortedList.clear ();
		for (QMap <QString, TreeItem*>::const_iterator i = mapOfColumnToTreeItem.constEnd ();
		     i != mapOfColumnToTreeItem.constBegin (); --i)
			_individualItemsSortedList << i.value ();
	}
	endResetModel ();
}

void TableModel::clear ()
{
	beginResetModel();
	_individualItemsSortedList.clear ();
	endResetModel();
}

TreeItem* TableModel::getItem (const QModelIndex &index) const
{
	int row = index.row ();

	if (index.parent ().isValid () || row < 0 || row >= _individualItemsSortedList.size ())
		return NULL;

	return _individualItemsSortedList.at (row);
}

void TableModel::populateModel ()
{
	QList<TreeItem> individualItems;
	foreach (TreeItem *item, TreeItem::individualTreeItemList)
		individualItems << *item;

	qSort (individualItems);

	_individualItemsSortedList.clear ();
	foreach (TreeItem item, individualItems)
	{
		if (TreeItem::idToIndividualTreeItemHash.contains (item.id ()))
			_individualItemsSortedList << TreeItem::idToIndividualTreeItemHash.value (item.id ());
	}

	int sortColumn = _sortColumn;
	_sortColumn = -1;

	sort (sortColumn, _sortOrder);
}

void TableModel::regenerateModel ()
{
	beginResetModel ();
	populateModel ();
	endResetModel ();
}

void TableModel::handleItemDumminessChange(TreeItem *item)
{
	beginResetModel();
	/* Note here we are supposed to take care of dumminess change,
	   so if it is dummy now, it was not dummy before and vice-versa.
	 */
	if (!item->isDummy())
	{
		if (!TreeItem::individualTreeItemList.contains(item))
			TreeItem::individualTreeItemList.append(item);
	}
	else
	{
		if (TreeItem::individualTreeItemList.contains(item))
			TreeItem::individualTreeItemList.removeOne(item);
	}
	populateModel ();
	endResetModel();
}

void TableModel::setupSignals ()
{
	connect (sourceModel (), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
		 this, SLOT(regenerateModel()));
	connect (sourceModel (), SIGNAL(rowsInserted (const QModelIndex &, int, int)),
		 this, SLOT(regenerateModel()));
	connect (sourceModel (), SIGNAL(rowsAboutToBeRemoved (const QModelIndex &, int, int)),
		 this, SLOT(regenerateModel()));
}

void TableModel::breakSignals ()
{
	disconnect (sourceModel (), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
		 this, SLOT(regenerateModel()));
	disconnect (sourceModel (), SIGNAL(rowsInserted (const QModelIndex &, int, int)),
		 this, SLOT(regenerateModel()));
	disconnect (sourceModel (), SIGNAL(rowsAboutToBeRemoved (const QModelIndex &, int, int)),
		 this, SLOT(regenerateModel()));
}
