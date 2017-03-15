#include "StringListModel.h"

StringListModel::StringListModel(QStringList stringList, QObject *parent) :
		QAbstractItemModel(parent), _stringList (stringList)
{
}

QModelIndex StringListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (row < 0 || parent.isValid())
		return QModelIndex();

	if (row < _stringList.count())
		return createIndex(row, column, (void *)row);

	return QModelIndex();
}

QModelIndex StringListModel::parent (const QModelIndex &index) const
{
	Q_UNUSED(index)
	return QModelIndex();
}

bool StringListModel::hasChildren(const QModelIndex &parent) const
{
	return rowCount(parent) > 0;
}

int StringListModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return _stringList.count();
}

int StringListModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED (parent)
	return 1;
}

Qt::ItemFlags StringListModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant StringListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.column() != 0)
		return QVariant();

	int row = index.row();
	if (row < 0 || row >= _stringList.count())
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		if (index.column() == 0)
			return _stringList.at(row);
	}

	return QVariant();
}

bool StringListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid() || index.column() != 0)
		return false;

	if (role != Qt::EditRole)
		return false;

	int row = index.row();

	if (row < 0 || row > _stringList.count())
		return false;

	QString string = value.toString();
	if (!string.length())
		return false;

	_stringList.replace(row, string);
	emit dataChanged(index, index);
	return true;
}

bool StringListModel::insertRow(int row, const QModelIndex & parent)
{
	if (row < 0 || row > rowCount())
		return false;

	beginInsertRows(parent, row, row);
	_stringList.insert(row, "*** Fix Me ***");
	endInsertRows();
	return true;
}

bool StringListModel::removeRow(int row, const QModelIndex & parent)
{
	if (row < 0 || row > rowCount())
		return false;

	beginRemoveRows(parent, row, row);
	_stringList.removeAt(row);
	endRemoveRows();

	return true;
}
