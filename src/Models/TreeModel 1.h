#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "TreeItem.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QMimeData>

#include "Family.h"
#include "Individual.h"

class TreeModel : public QAbstractItemModel
{
	Q_OBJECT

private:
	TreeItem *			_rootItem;

private:
	static const QString		s_mimetype;

public:
	TreeModel					(QObject *parent = 0);
	TreeModel					(TreeItem *rootItem, QObject *parent = 0);
	~TreeModel					();

public:
	TreeItem *		getItem			(const QModelIndex &index) const;
	TreeItem *		getChild		(const QModelIndex &index, const int row) const;

	virtual QModelIndex	index			(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex		index			(TreeItem *item, int column = 0) const;

	virtual QModelIndex	parent			(const QModelIndex &index) const;
	QModelIndex		parent			(TreeItem *childItem) const;

	virtual bool		hasChildren		(const QModelIndex &parent = QModelIndex()) const;
	virtual int		rowCount		(const QModelIndex &parent = QModelIndex()) const;
	virtual int		columnCount		(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant	headerData		(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual Qt::ItemFlags	flags			(const QModelIndex &index) const;
	virtual QVariant	data			(const QModelIndex &index, int role) const;
	virtual bool		setData			(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	/*virtual QModelIndex	commonParent		(const QModelIndex &index1,
							 const QModelIndex &index2);*/

	bool			insertRow		(Familier *fam, bool createItemOnly, int row,
							 const QModelIndex &parent = QModelIndex(),
							 IDTYPE uid = 0);
	virtual bool		removeRow		(int row, const QModelIndex &parent = QModelIndex());

	Qt::DropActions		supportedDragActions	() const;
	virtual Qt::DropActions	supportedDropActions	() const;

	virtual QStringList	mimeTypes		() const;
	virtual QMimeData*	mimeData		(const QModelIndexList &indexes) const;
	virtual bool		dropMimeData		(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

	void			clear			();

private slots:
	void			adjustForModelReset	();
	void			adjustForFamilyNameChange ();

signals:
	void			rowInserted		();
	void			itemDumminessChanged	(TreeItem *);
	void			expandAssociatedTree	();
};

#endif /* TREEMODEL_H */
