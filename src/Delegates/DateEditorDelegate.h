#ifndef DATEEDITORDELEGATE_H
#define DATEEDITORDELEGATE_H

#include <QStyledItemDelegate>

class DateEditorDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit DateEditorDelegate(QObject *parent = 0);

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

#endif // DATEEDITORDELEGATE_H
