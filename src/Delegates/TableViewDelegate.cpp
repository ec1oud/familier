#include "TableViewDelegate.h"
#include <QTableView>
#include <QHeaderView>
#include "EventModel.h"
#include "GeneralEventModel.h"
#include "EventMembersModelForTableView.h"
#include <QRect>

TableViewDelegate::TableViewDelegate(IconModel *model, QObject *parent)
	: QItemDelegate(parent), _model (model)
{
}

QWidget* TableViewDelegate::createEditor (QWidget* parent, const QStyleOptionViewItem &option,
					   const QModelIndex& index ) const
{
	QAbstractItemModel *m = const_cast<QAbstractItemModel*>(index.model ());
	GeneralEventModel *sm = dynamic_cast<GeneralEventModel *>(m);
	QModelIndex idx = sm->mapToSource (index);
	Event *event = static_cast<Event*>(idx.internalPointer ());
	if (!event)
		return QItemDelegate::createEditor(parent, option, index);

	EventMembersModelForTableView *model = new EventMembersModelForTableView(event);
	model->setSourceModel (_model);
	connect (model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), sm, SLOT(handleMemberListChange()));
	QTableView *tabView = new QTableView(parent);
	tabView->setModel(model);
	tabView->setMinimumSize (400, 300);
	tabView->horizontalHeader ()->resizeSection (0, 220);

	return tabView;
}

void TableViewDelegate::setEditorData(QWidget* editor,
				       const QModelIndex & index ) const
{
	QTableView* tabView = qobject_cast<QTableView*>(editor);

	const QAbstractItemModel *model = index.model();
	if (!tabView || !model)
		QItemDelegate::setEditorData(editor, index);
}

void TableViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
				      const QModelIndex &index) const
{
	if (!index.isValid())
		return;

	QTableView* tabView = qobject_cast<QTableView*>(editor);
	if (!tabView)
		return QItemDelegate::setModelData(editor, model, index);
}

void TableViewDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
					      const QModelIndex &/* index */) const
{
	int width = 400;
	int height = 300;
	QWidget *parentWidget = dynamic_cast<QWidget*>(editor->parent ());
	if (!parentWidget)
		editor->setGeometry(QRect(option.rect.x (), option.rect.y (), width, height));
	else
	{
		QRect parentWidgetRect = parentWidget->geometry ();
		QRect rect = QRect(option.rect.x (), option.rect.y (), width, height);
		if (parentWidgetRect.x () + parentWidgetRect.width () < rect.x () + rect.width ())
			rect.adjust (-width, 0, -width, 0);
		if (parentWidgetRect.y () + parentWidgetRect.height () < rect.y () + rect.width ())
			rect.adjust (0, -height, 0, -height);
		editor->setGeometry (rect);
	}
}
