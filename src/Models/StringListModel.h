#ifndef STRINGLISTMODEL_H
#define STRINGLISTMODEL_H

#include <QAbstractItemModel>
#include <QStringList>

class StringListModel : public QAbstractItemModel
{
	Q_OBJECT

	QStringList			_stringList;

public:
	explicit StringListModel(QStringList stringList, QObject *parent = 0);

	QModelIndex		index			(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex		parent			(const QModelIndex &index) const;

	bool			hasChildren		(const QModelIndex &parent = QModelIndex()) const;
	int			rowCount		(const QModelIndex &parent = QModelIndex()) const;
	int			columnCount		(const QModelIndex &parent = QModelIndex()) const;

	//QVariant		headerData		(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	Qt::ItemFlags		flags			(const QModelIndex &index) const;
	QVariant		data			(const QModelIndex &index, int role) const;
	bool			setData			(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	bool			insertRow		(int row, const QModelIndex &parent = QModelIndex());
	bool			removeRow		(int row, const QModelIndex &parent = QModelIndex());

	QStringList		getData			() const { return _stringList; }
signals:

public slots:

};

#endif // STRINGLISTMODEL_H
