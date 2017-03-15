#ifndef TABLEVIEWDELEGATE_H
#define TABLEVIEWDELEGATE_H

#include <QItemDelegate>
#include "IconModel.h"

class TableViewDelegate : public QItemDelegate
{
	Q_OBJECT

	IconModel *		_model;

public:
	explicit TableViewDelegate(IconModel *model, QObject *parent = 0);

	QWidget*	createEditor		(QWidget* parent, const QStyleOptionViewItem &option,
						 const QModelIndex& index ) const;
	void		setEditorData		(QWidget* editor, const QModelIndex & index ) const;
	void		setModelData		(QWidget *editor, QAbstractItemModel *model,
						 const QModelIndex &index) const;
	void		updateEditorGeometry	(QWidget *editor, const QStyleOptionViewItem &option,
						 const QModelIndex &/* index */) const;

signals:

public slots:

};

#endif // TABLEVIEWDELEGATE_H
