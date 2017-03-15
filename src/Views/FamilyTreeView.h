#ifndef FAMILYTREEVIEW_H
#define FAMILYTREEVIEW_H

#include <QAbstractItemView>
#include <QHash>
#include <QRectF>
#include "TreeModel.h"
#include "TreeItem.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QHash>

class FamilyTreeView : public QAbstractItemView
{
	Q_OBJECT

	mutable qreal					_idealHeight;
	mutable qreal					_idealWidth;
	mutable QHash <QModelIndex, QRectF>		_rectForIndex;
	mutable QHash <TreeItem*, QRectF>		_rectForItem;
	mutable QList <TreeItem*>			_listOfCollapsedItems;
	mutable bool					_hashNeedsUpdate;
	bool						_showExtraDetailsForItem;
	bool						_showBasicDetailsForItem;

	const int					_xStep;
	const int					_yStep;
	const int					_plusLength;
	const int					_plusPadding;
	const qreal					_itemSeparation;
	const qreal					_lineSeparation;
	const qreal					_iconSize;
	const qreal					_dummyDotSize;

public:
	explicit FamilyTreeView (QWidget *parent = 0);

	virtual void		setModel		(QAbstractItemModel *model);
	virtual QRect		visualRect		(const QModelIndex &index) const;
	virtual void		scrollTo		(const QModelIndex &index,
							 ScrollHint hint = EnsureVisible);
	virtual QModelIndex	indexAt			(const QPoint &point) const;
	TreeItem *		itemAt			(const QPoint &point) const;
	bool			getShowExtraDetailsForItem () const;
	bool			getShowBasicDetailsForItem () const;

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
	virtual void		mouseDoubleClickEvent	(QMouseEvent *event);
	virtual void		keyPressEvent		(QKeyEvent *event);

	virtual void		paintEvent		(QPaintEvent *event);
	virtual void		resizeEvent		(QResizeEvent *event);
	virtual void		scrollContentsBy	(int dx, int dy);

	virtual QRegion		visualRegionForSelection(const QItemSelection &selection) const;

private:
	TreeItem *		getItem			(const QModelIndex &index) const;
	QRectF			viewportRectForIndex	(const QModelIndex &index) const;
	QRectF			viewportRectForItem	(TreeItem *item) const;
	QRectF			viewportRectForItemTerminator (TreeItem *item) const;
	qreal			setRectForField		(TreeItem *item, Familier::t_columns column,
							 qreal &y) const;
	void			calculateRectsIfRequired() const;
	void			calculateRectsIfRequired(TreeItem *item) const;
	QModelIndex		indexAtForFields	(const QPoint &point, QModelIndex &index) const;
	QModelIndex		indexAtRecursive	(const QPoint &point, QModelIndex &index) const;
	bool			setSelectionRecursively	(const QModelIndex &index, const QRect &rect,
							 QItemSelectionModel::SelectionFlags command);
	void			paintIndex		(QModelIndex index, QPainter &painter);
	void			paintIndexesRecursively	(QModelIndex index, QPainter &painter);
	void			paintOutline		(QPainter &painter, const QRect &rectangle);
	void			paintTreeBranchTerminationForDaughters
							(QPainter &painter, const QRect &rectangle);
	void			paintTreeBranchTerminationForAll
							(QPainter &painter, const QRect &rectangle);
	void			paintTreeBranchTerminationForExpandable
							(QPainter &painter, const QRect &rectangle);

	void			collapse		(TreeItem *item);
	void			expand			(TreeItem *item);
	void			collapseAll		(const QModelIndex &index);
	bool			isExpanded		(TreeItem *item) const;
	void			setExpanded		(TreeItem *item, bool expanded);

public:
	bool			isExpanded		(const QModelIndex &index) const;
	void			setExpanded		(const QModelIndex &index, bool expanded);

public slots:
	void			edit			(const QModelIndex &index);
	virtual void		reset			();
	void			collapse		(const QModelIndex &index);
	void			collapseAll		();
	void			expand			(const QModelIndex &index);
	void			expandAll		();
	void			setShowExtraDetailsForItem (const bool showExtraDetailsForItem);
	void			setShowBasicDetailsForItem (const bool showBasicDetailsForItem);
};

#endif // FAMILYTREEVIEW_H
