#ifndef EVENTTIMELINEVIEW_H
#define EVENTTIMELINEVIEW_H

#include <QAbstractItemView>

class EventTimeLineView : public QAbstractItemView
{
	Q_OBJECT

	const int					c_dotRadius;
	const int					c_minYear;
	const int					c_maxYear;

	mutable int					_minYear;
	mutable int					_maxYear;
	mutable int					_yearUnit;
	mutable qreal					_idealWidth;
	mutable qreal					_idealHeight;
	mutable qreal					_bottomBarWidth;
	mutable QHash <int, QRectF>			_rectForRow;
	mutable bool					_hashNeedsUpdate;

public:
	explicit EventTimeLineView(QWidget *parent = 0);

	virtual void		setModel		(QAbstractItemModel *model);
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
	/* This is just a private in PieView */
	virtual void		updateGeometries	();

protected:
	virtual bool		edit			(const QModelIndex &, EditTrigger, QEvent *);
	virtual QModelIndex	moveCursor		(QAbstractItemView::CursorAction cursorAction,
							 Qt::KeyboardModifiers modifiers);

	virtual	int		horizontalOffset	() const;
	virtual int		verticalOffset		() const;

	virtual bool		isIndexHidden		(const QModelIndex &index) const;

	virtual void		setSelection		(const QRect&,
							 QItemSelectionModel::SelectionFlags command);

	virtual void		mousePressEvent		(QMouseEvent *event);

	virtual void		paintEvent		(QPaintEvent *event);
	virtual void		resizeEvent		(QResizeEvent *event);
	virtual void		scrollContentsBy	(int dx, int dy);

	virtual QRegion		visualRegionForSelection(const QItemSelection &selection) const;

private:
	QRectF			viewportRectForIndex	(const QModelIndex &index) const;
	void			calculateRectsIfRequired() const;

signals:

public slots:

};

#endif // EVENTTIMELINEVIEW_H
