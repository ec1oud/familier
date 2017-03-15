#ifndef PIEVIEW_H
#define PIEVIEW_H

#include <QAbstractItemView>
#include <QFont>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QRect>
#include <QSize>
#include <QPoint>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QRubberBand;
QT_END_NAMESPACE

//! [0]
class PieView : public QAbstractItemView
{
	Q_OBJECT

private:
	int		_margin;
	int		_totalSize;
	int		_pieSize;
	int		_validItems;
	int		_totalValue;
	QPoint		_origin;
	QRubberBand *	_rubberBand;

public:
	PieView(QWidget *parent = 0);

	virtual QRect		visualRect		(const QModelIndex &index) const;
	virtual void		scrollTo		(const QModelIndex &index,
							 ScrollHint hint = EnsureVisible);
	virtual QModelIndex	indexAt			(const QPoint &point) const;

public slots:
	virtual void		dataChanged		(const QModelIndex &topLeft,
							 const QModelIndex &bottomRight);

protected slots:
	virtual void		rowsInserted		(const QModelIndex &parent,
							 int start, int end);
	virtual void		rowsAboutToBeRemoved	(const QModelIndex &parent,
							 int start, int end);

protected:
	virtual bool		edit			(const QModelIndex &, EditTrigger, QEvent *);
	virtual QModelIndex	moveCursor		(QAbstractItemView::CursorAction cursorAction,
							 Qt::KeyboardModifiers modifiers);

	virtual int		horizontalOffset	() const;
	virtual int		verticalOffset		() const;

	virtual bool		isIndexHidden		(const QModelIndex &index) const;

	virtual void		setSelection		(const QRect&,
							 QItemSelectionModel::SelectionFlags command);

	virtual void		mousePressEvent		(QMouseEvent *event);
	virtual void		mouseMoveEvent		(QMouseEvent *event);
	virtual void		mouseReleaseEvent	(QMouseEvent *event);

	virtual void		paintEvent		(QPaintEvent *event);
	virtual void		resizeEvent		(QResizeEvent *event);
	virtual void		scrollContentsBy	(int dx, int dy);

	virtual QRegion		visualRegionForSelection(const QItemSelection &selection) const;

private:
	QRect			itemRect		(const QModelIndex &item) const;
	QRegion			itemRegion		(const QModelIndex &index) const;
	int			rows			(const QModelIndex &index = QModelIndex()) const;
	virtual void		updateGeometries	();
};
//! [0]

#endif
