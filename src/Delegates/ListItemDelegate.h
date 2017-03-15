#ifndef LISTITEMDELEGATE_H
#define LISTITEMDELEGATE_H

#include <QStyledItemDelegate>

class ListItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit ListItemDelegate(QObject *parent = 0);

	virtual void		setEditorData		(QWidget *editor,
							 const QModelIndex &index) const;

signals:

public slots:

};

#endif // LISTITEMDELEGATE_H
