#include "FamilyTreeView.h"
#include <QApplication>
#include <QScrollBar>
#include "FamilyTreeModel.h"
#include "Individual.h"
#include <QFontMetrics>
#include <QFont>
#include <qmath.h>

#ifndef M_PI
#define M_PI 3.1415927
#endif

FamilyTreeView::FamilyTreeView(QWidget *parent) :
		QAbstractItemView(parent),
		_idealHeight (0), _idealWidth (0), _hashNeedsUpdate (false),
		_showExtraDetailsForItem (true), _showBasicDetailsForItem (true),
		_xStep (2), _yStep (4),
		_plusLength (4), _plusPadding (2),
		_itemSeparation (10), _lineSeparation (2), _iconSize (64), _dummyDotSize (8)
{
	setFocusPolicy(Qt::WheelFocus);
	setFont(QApplication::font("QTreeView"));
	horizontalScrollBar()->setRange(0, 0);
	verticalScrollBar()->setRange(0, 0);
	setDragEnabled (true);
	setAcceptDrops (true);
	setDropIndicatorShown (true);
	setTabKeyNavigation (true);
	setSelectionMode (QAbstractItemView::SingleSelection);
}

void FamilyTreeView::setModel (QAbstractItemModel *model)
{
	QAbstractItemView::setModel (model);
	_hashNeedsUpdate = true;
}

QRect FamilyTreeView::visualRect (const QModelIndex &index) const
{
	QRect rect;
	if (index.isValid ())
		rect = viewportRectForIndex (index).toRect();
	return rect;
}

void FamilyTreeView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint scrollHint)
{
	if (!index.isValid ())
		return;

	QRect viewRect = viewport()->rect();
	TreeItem *item = getItem (index);
	QRect itemRect = viewportRectForItem (item).toRect ();
	if (itemRect.left() < viewRect.left())
		horizontalScrollBar()->setValue(horizontalScrollBar()->value()
						+ itemRect.left() - viewRect.left());
	else if (itemRect.right() > viewRect.right())
		horizontalScrollBar()->setValue(horizontalScrollBar()->value()
						+ qMin(itemRect.right() - viewRect.right(),
						       itemRect.left() - viewRect.left()));
	if (itemRect.top() < viewRect.top())
		verticalScrollBar()->setValue(verticalScrollBar()->value() +
					      itemRect.top() - viewRect.top());
	else if (itemRect.bottom() > viewRect.bottom())
		verticalScrollBar()->setValue(verticalScrollBar()->value() +
					      qMin(itemRect.bottom() - viewRect.bottom(),
						   itemRect.top() - viewRect.top()));

	itemRect = viewportRectForItem (item).toRect ();
	viewRect = viewport ()->rect ();
	switch (scrollHint)
	{
	case QAbstractItemView::PositionAtTop:
		if (itemRect.top () > 0)
			verticalScrollBar ()->setValue (verticalScrollBar ()->value () +
							viewRect.top () - itemRect.top ());
		break;
	case QAbstractItemView::PositionAtBottom:
		if (itemRect.top () < viewRect.bottom ())
			verticalScrollBar ()->setValue (verticalScrollBar ()->value () -
							viewRect.bottom () + itemRect.top ());
		break;
	case QAbstractItemView::PositionAtCenter:
		{
			int xViewMid = viewRect.width ()/ 2;
			int yViewMid = viewRect.height ()/ 2;
			if (itemRect.left () < xViewMid)
				horizontalScrollBar()->setValue(horizontalScrollBar()->value()-
								xViewMid + itemRect.width () / 2);
			else
				horizontalScrollBar()->setValue(horizontalScrollBar()->value() +
								xViewMid - itemRect.width () / 2);
			if (itemRect.top () < yViewMid)
				verticalScrollBar()->setValue(verticalScrollBar()->value() -
							      yViewMid + itemRect.height () / 2);
			else
				verticalScrollBar()->setValue(verticalScrollBar()->value() +
							      yViewMid + itemRect.height () / 2);
		}
		break;
	default:
		break;
	}

	viewport()->update();
}

QModelIndex FamilyTreeView::indexAt (const QPoint &point) const
{
	QPoint relPoint (point);
	relPoint.rx() += horizontalScrollBar()->value();
	relPoint.ry() += verticalScrollBar()->value();

	calculateRectsIfRequired ();
	QModelIndex rootIdx = rootIndex ();
	return indexAtRecursive (relPoint, rootIdx);
}

TreeItem *FamilyTreeView::itemAt (const QPoint &point) const
{
	QPoint relPoint (point);
	relPoint.rx() += horizontalScrollBar()->value();
	relPoint.ry() += verticalScrollBar()->value();

	calculateRectsIfRequired ();
	for (QHash<TreeItem*, QRectF>::const_iterator i = _rectForItem.constBegin ();
		i != _rectForItem.constEnd (); i++)
	{
		TreeItem *item = i.key ();
		QRectF rect = i.value ();
		if (rect.contains (relPoint))
			return item;
	}

	return NULL;
}

bool FamilyTreeView::getShowExtraDetailsForItem () const
{
	return _showExtraDetailsForItem;
}

bool FamilyTreeView::getShowBasicDetailsForItem () const
{
	return _showBasicDetailsForItem;
}

void FamilyTreeView::dataChanged (const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	_hashNeedsUpdate = true;
	QAbstractItemView::dataChanged (topLeft, bottomRight);
}

void FamilyTreeView::rowsInserted (const QModelIndex &parent, int start, int end)
{
	_hashNeedsUpdate = true;
	QAbstractItemView::rowsInserted (parent, start, end);
}

void FamilyTreeView::rowsAboutToBeRemoved (const QModelIndex &parent, int start, int end)
{
	_hashNeedsUpdate = true;
	QAbstractItemView::rowsAboutToBeRemoved (parent, start, end);
}

void FamilyTreeView::updateGeometries()
{
	QFontMetrics fm(font());
	horizontalScrollBar()->setSingleStep(fm.width("n"));
	horizontalScrollBar()->setPageStep(viewport()->width());
	horizontalScrollBar()->setRange(0, qMax(0.0, _idealWidth - viewport()->width()));
	const int RowHeight = fm.height();
	verticalScrollBar()->setSingleStep(RowHeight);
	verticalScrollBar()->setPageStep(viewport()->height());
	verticalScrollBar()->setRange(0, qMax(0.0, _idealHeight - viewport()->height()));
}

bool FamilyTreeView::edit (const QModelIndex &index, EditTrigger trigger, QEvent *event)
{
	if (model ()->flags (index) && Qt::ItemIsEditable)
		return QAbstractItemView::edit (index, trigger, event);

	return false;
}

QModelIndex FamilyTreeView::moveCursor (QAbstractItemView::CursorAction cursorAction,
					Qt::KeyboardModifiers modifiers)
{
	QModelIndex index = currentIndex ();
	if (index.isValid ())
	{
		TreeItem *item = getItem (index);
		if (item)
		{
			TreeItem *newItem = NULL;
			switch (cursorAction)
			{
			case MoveLeft:
				if (modifiers == Qt::NoModifier && item->parentItem ())
					newItem = item->parentItem ()->leftChildFrom (item);
				if (!newItem)
					horizontalScrollBar ()->setValue (horizontalScrollBar ()->value () - 10);
				break;
			case MoveRight:
				if (modifiers == Qt::NoModifier && item->parentItem ())
					newItem = item->parentItem ()->rightChildFrom (item);
				if (!newItem)
					horizontalScrollBar ()->setValue (horizontalScrollBar ()->value () + 10);
				break;
			case MoveUp:
				if (modifiers == Qt::NoModifier)
				{
					newItem = item->parentItem ();
					while (newItem && newItem->isDummy () && newItem->parentItem ())
						newItem = newItem->parentItem ();
				}
				if (!newItem)
					verticalScrollBar ()->setValue (verticalScrollBar ()->value () - 10);
				break;
			case MoveDown:
				if (modifiers == Qt::NoModifier && item->hasChild ())
				{
					TreeItem *tempItem = item;
					do
					{
						if (tempItem->hasChild ())
							newItem = tempItem->childAt (0);
						tempItem = newItem;
					} while (newItem->isDummy () && newItem->hasChild ());
				}
				else
					verticalScrollBar ()->setValue (verticalScrollBar ()->value () + 10);
				break;
			default:
				newItem = NULL;
				break;
			}

			if (newItem)
				index = dynamic_cast<FamilyTreeModel*>(model ())->index (newItem);
		}
	}

	viewport()->update();
	return index;
}

int FamilyTreeView::horizontalOffset() const
{
	return horizontalScrollBar()->value();
}

int FamilyTreeView::verticalOffset() const
{
	return verticalScrollBar()->value();
}

void FamilyTreeView::scrollContentsBy(int dx, int dy)
{
	scrollDirtyRegion(dx, dy);
	viewport()->scroll(dx, dy);
}

bool FamilyTreeView::isIndexHidden (const QModelIndex &index) const
{
	Q_UNUSED(index)
	return false;
}

void FamilyTreeView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags)
{
	QRect rectangle = rect.translated(horizontalScrollBar()->value(),
					  verticalScrollBar()->value()).normalized();

	calculateRectsIfRequired ();
	QModelIndex rootIdx = rootIndex ();

	if (!setSelectionRecursively (rootIdx, rectangle, flags))
	{
		QModelIndex invalid;
		QItemSelection selection(invalid, invalid);
		selectionModel()->select(selection, flags);
	}

	update();
}

void FamilyTreeView::mousePressEvent(QMouseEvent *event)
{
	QAbstractItemView::mousePressEvent(event);
	QModelIndex idx = indexAt (event->pos());
	if (idx.isValid ())
		setCurrentIndex (idx);
	else
	{
		QPoint p = event->pos ();
		p.ry () -= (_yStep + 2 * _plusLength + _plusPadding);
		TreeItem *item = itemAt (p);
		if (item && !isExpanded (item))
		{
			QRect rect = viewportRectForItemTerminator (item).toRect ();
			if (rect.contains (event->pos ()))
				expand (item);
		}
	}
}

void FamilyTreeView::mouseDoubleClickEvent (QMouseEvent *event)
{
	TreeItem *item = itemAt (event->pos ());
	if (item)
		setExpanded (item, !isExpanded (item));

	QAbstractItemView::mouseDoubleClickEvent (event);
}

void FamilyTreeView::keyPressEvent (QKeyEvent *event)
{
	int key = event->key ();
	switch (key)
	{
	case Qt::Key_Asterisk:
		expandAll ();
		break;
	case Qt::Key_Period:
		collapseAll ();
		break;
	case Qt::Key_Plus:
		{
			QModelIndex currentIdx = currentIndex ();
			if (currentIdx.isValid ())
				expand (currentIdx);
		}
		break;
	case Qt::Key_Minus:
		{
			QModelIndex currentIdx = currentIndex ();
			if (currentIdx.isValid ())
				collapse (currentIdx);
		}
		break;
	case Qt::Key_Up:
		if (!currentIndex ().isValid ())
			verticalScrollBar ()->setValue (verticalScrollBar ()->value () - 10);
		break;
	case Qt::Key_Down:
		if (!currentIndex ().isValid ())
			verticalScrollBar ()->setValue (verticalScrollBar ()->value () + 10);
		break;
	case Qt::Key_PageUp:
		verticalScrollBar ()->setValue (verticalScrollBar ()->value () -
						viewport ()->rect ().height ());
		break;
	case Qt::Key_PageDown:
		verticalScrollBar ()->setValue (verticalScrollBar ()->value () +
						viewport ()->rect ().height ());
		break;
	case Qt::Key_Home:
		verticalScrollBar ()->setValue (0);
		break;
	case Qt::Key_End:
		verticalScrollBar ()->setValue (verticalScrollBar ()->maximum ());
		break;
	case Qt::Key_Left:
		if (!currentIndex ().isValid ())
			horizontalScrollBar ()->setValue (horizontalScrollBar ()->value () - 10);
		break;
	case Qt::Key_Right:
		if (!currentIndex ().isValid ())
			horizontalScrollBar ()->setValue (horizontalScrollBar ()->value () + 10);
		break;
	case Qt::Key_BracketLeft:
		horizontalScrollBar ()->setValue (horizontalScrollBar ()->value () -
						  viewport ()->rect ().width ());
		break;
	case Qt::Key_BracketRight:
		horizontalScrollBar ()->setValue (horizontalScrollBar ()->value () +
						  viewport ()->rect ().width ());
		break;
	case Qt::Key_BraceLeft:
		horizontalScrollBar ()->setValue (0);
		break;
	case Qt::Key_BraceRight:
		horizontalScrollBar ()->setValue (horizontalScrollBar ()->maximum ());
		break;
	default:
		break;
	}

	QAbstractItemView::keyPressEvent (event);
}

void FamilyTreeView::paintEvent (QPaintEvent *event)
{
	Q_UNUSED(event)

	calculateRectsIfRequired ();
	updateGeometries ();

	QPainter painter (viewport());
	painter.setRenderHints (QPainter::Antialiasing | QPainter::TextAntialiasing);

	paintIndexesRecursively (rootIndex (), painter);
}

void FamilyTreeView::resizeEvent(QResizeEvent*)
{
	_hashNeedsUpdate = true;
	calculateRectsIfRequired ();
	updateGeometries();
}

QRegion FamilyTreeView::visualRegionForSelection (const QItemSelection &selection) const
{
	QRegion region;
	foreach (const QItemSelectionRange &range, selection)
	{
		for (int row = range.top(); row <= range.bottom(); ++row)
		{
			for (int column = range.left(); column < range.right(); ++column)
			{
				QModelIndex index = model()->index(row, column, rootIndex());
				region += visualRect(index);
			}
		}
	}

	return region;
}

TreeItem* FamilyTreeView::getItem (const QModelIndex &index) const
{
	FamilyTreeModel *familyTreeModel = dynamic_cast <FamilyTreeModel*> (model ());
	if (!familyTreeModel)
		return NULL;

	return familyTreeModel->getItem (index);
}

QRectF FamilyTreeView::viewportRectForIndex (const QModelIndex &index) const
{
	if (!index.isValid ())
		return QRectF ();

	calculateRectsIfRequired ();

	QRectF rect = _rectForIndex.value (index);
	if (!rect.isValid ())
		return rect;
	return QRectF (rect.x() - horizontalScrollBar()->value(),
		       rect.y() - verticalScrollBar()->value(),
		       rect.width(), rect.height());
}

QRectF FamilyTreeView::viewportRectForItem (TreeItem *item) const
{
	calculateRectsIfRequired ();

	QRectF rect = _rectForItem.value (item);
	if (!rect.isValid ())
		return rect;

	return QRectF (rect.x() - horizontalScrollBar()->value(),
		       rect.y() - verticalScrollBar()->value(),
		       rect.width(), rect.height());
}

QRectF FamilyTreeView::viewportRectForItemTerminator (TreeItem *item) const
{
	QRectF rect = viewportRectForItem (item);
	if (!rect.isValid ())
		return rect;

	qreal xmid = (rect.left () + rect.right ()) / 2;
	qreal rectSize = 2 * _itemSeparation;
	return QRectF (xmid - _itemSeparation, rect.bottom (), rectSize, rectSize);
}

qreal FamilyTreeView::setRectForField (TreeItem *item, Familier::t_columns column, qreal &y) const
{
	const int horizontalPadding = 50;
	FamilyTreeModel *tm = dynamic_cast<FamilyTreeModel*>(model());
	QModelIndex index = tm->index (item, column);

	qreal pixmapWidth = 0;
	qreal pixmapHeight = 0;
	QVariant value = index.data (Qt::DecorationRole);
	if (value.canConvert<QPixmap> ())
	{
		QPixmap pixmap = value.value<QPixmap>();
		pixmapWidth = pixmap.width ();
		pixmapHeight = pixmap.height ();
	}

	value = index.data (Qt::FontRole);
	QFont fx;
	if (value.canConvert<QFont>())
		fx = value.value<QFont>();
	else
		fx = font ();
	QFontMetrics fmx (fx);
	y += _lineSeparation;
	qreal hx = qMax (qreal (fmx.height ()), pixmapHeight);
	qreal wx = fmx.width (index.data (Qt::DisplayRole).toString ())
		   + pixmapWidth + horizontalPadding;
	_rectForIndex [index] = QRectF (0, y, wx, hx);
	y += hx;

	return wx;
}

void FamilyTreeView::calculateRectsIfRequired () const
{
	if (!_hashNeedsUpdate)
		return;

	_idealHeight = 0;
	_idealWidth = 0;

	_rectForIndex.clear ();
	_rectForItem.clear ();

	TreeItem *rootItem = TreeItem::s_RootItemForFamilyTreeView;
	if (rootItem->isRoot ())
	{
		_rectForItem [rootItem] = QRectF (0, 0, 0, 0);
		for (int i = 0; i < rootItem->childCount (); i++)
		{
			calculateRectsIfRequired (rootItem->childAt (i));
			_rectForItem [rootItem] = QRectF (0, _idealHeight, 0, 0);
		}
		_rectForItem.remove (rootItem);
	}
	else
		calculateRectsIfRequired (rootItem);

	_hashNeedsUpdate = false;
	viewport ()->update ();
}

void FamilyTreeView::calculateRectsIfRequired (TreeItem *item) const
{
	qreal cellWidthExcludingIcon = 0;
	qreal itemWidth = 0;
	qreal itemHeight = 0;
	qreal itemVerticalSeparation = 2 * _itemSeparation;

	QRectF parentRect = _rectForItem.value (item->parentItem ());
	qreal y_ = parentRect.y () + parentRect.height ();

	qreal y = y_ + itemVerticalSeparation;

	FamilyTreeModel *tm = dynamic_cast<FamilyTreeModel*>(model());

	if (_showBasicDetailsForItem && _showExtraDetailsForItem &&
	    item->isIndividual () && !item->isDummy ())
	{
		_rectForIndex[tm->index (item, Familier::e_col_maximum)]
				= QRectF (0, y + _lineSeparation, _iconSize, _iconSize);
		itemWidth += _lineSeparation + _iconSize;
	}

	if (!_showBasicDetailsForItem || (item->isIndividual () && item->isDummy ()))
	{
		FamilyTreeModel *tm = dynamic_cast<FamilyTreeModel*>(model());
		QModelIndex index = tm->index (item, Familier::e_col_name);
		_rectForIndex [index] = QRectF (0, y, _dummyDotSize, _dummyDotSize);
		y += _dummyDotSize;
		cellWidthExcludingIcon = _dummyDotSize;
	}
	else
		cellWidthExcludingIcon = qMax (cellWidthExcludingIcon,
					       setRectForField (item, Familier::e_col_name, y));

	if (_showBasicDetailsForItem && _showExtraDetailsForItem &&
	    item->isIndividual () && !item->isDummy ())
	{
		Individual *individual = dynamic_cast<Individual*>(item->fam ());
		if (individual->getBirthEvent ())
			cellWidthExcludingIcon = qMax (cellWidthExcludingIcon,
						       setRectForField (item, Familier::e_col_birthDate, y));
		if (individual->getDeathEvent ())
			cellWidthExcludingIcon = qMax (cellWidthExcludingIcon,
						       setRectForField (item, Familier::e_col_deathDate, y));
		if (item->isSpouseItem () &&
		    individual->getMarriageEvent (item->parentItem ()->id ()))
			cellWidthExcludingIcon = qMax (cellWidthExcludingIcon,
						       setRectForField (item, Familier::e_col_marriageDate, y));
	}

	y += _lineSeparation;

	itemHeight = y - y_ - _itemSeparation;
	qreal itemHeightWithIconOnly = _iconSize + 2 * _lineSeparation;

	if (_showBasicDetailsForItem && _showExtraDetailsForItem &&
	    item->isIndividual () && !item->isDummy () && itemHeightWithIconOnly > itemHeight)
		itemHeight = itemHeightWithIconOnly;
	itemWidth += _lineSeparation + cellWidthExcludingIcon + _lineSeparation;

	if (_showBasicDetailsForItem && (!item->isIndividual () || !item->isDummy ()))
		_rectForItem [item] = QRectF (0, y_ + itemVerticalSeparation, itemWidth, itemHeight);
	else
		_rectForItem [item] = QRectF (0, y_ + itemVerticalSeparation, _dummyDotSize, _dummyDotSize);
	_idealHeight = qMax (_idealHeight, y_ + itemHeight + 2 * itemVerticalSeparation);

	int childCountX = item->childCount ();
	if (isExpanded (item))
	{
		for (int i = 0; i < childCountX; i++)
			calculateRectsIfRequired (item->childAt (i));
	}

	qreal x_ = 0;
	qreal x = 0;
	if (childCountX && isExpanded (item))
	{
		TreeItem *leftMostDescendent = item->childAt (0);
		while (leftMostDescendent->hasChild () && isExpanded (leftMostDescendent))
			leftMostDescendent = leftMostDescendent->childAt (0);

		TreeItem *rightMostDescendent = item->childAt (item->childCount () - 1);
		while (rightMostDescendent->hasChild () && isExpanded (rightMostDescendent))
			rightMostDescendent = rightMostDescendent->childAt (rightMostDescendent->childCount () - 1);

		qreal xMid = (_rectForItem.value (leftMostDescendent).left () +
			      _rectForItem.value (rightMostDescendent).right ()) / 2;
		x_ = xMid - (itemWidth / 2);
		x = x_;
	}
	else
	{
		if (item->isIndividual ())
		{
			qreal parentMaxWidth = 0;
			TreeItem *tempChildItem = item;
			TreeItem *tempItem = item->parentItem ();
			while (tempItem && tempItem->leftMostChild () == tempChildItem)
			{
				qreal parentWidth = _rectForItem.value (tempItem).width ();
				if (parentMaxWidth < parentWidth)
					parentMaxWidth = parentWidth;
				if (!tempItem->isIndividual ())
					break;
				tempChildItem = tempItem;
				tempItem = tempItem->parentItem ();
			}
			TreeItem *childItem = tempItem->leftChildFrom (tempChildItem);
			if (!childItem)
				x_ = 0;
			else
			{
				while (childItem)
				{
					qreal xForChild = _rectForItem.value (childItem).right ();
					if (x_ < xForChild)
						x_ = xForChild;
					if (!isExpanded (childItem))
						break;
					childItem = childItem->rightMostChild ();
				}
			}
			if (parentMaxWidth > itemWidth)
				x_ += + (parentMaxWidth - itemWidth) / 2 + _itemSeparation;
		}
		else
			x_ = 0;
		x = x_ + _itemSeparation;
	}

	QModelIndex index;
	qreal width;

	if (_showBasicDetailsForItem && _showExtraDetailsForItem &&
	    item->isIndividual () && !item->isDummy ())
	{
		x += _lineSeparation;
		index = tm->index (item, Familier::e_col_maximum);
		width = _rectForIndex[index].width ();
		_rectForIndex[index].setLeft (x);
		_rectForIndex[index].setWidth (width);
		x += width;
	}

	x += _lineSeparation;

	index = tm->index (item, Familier::e_col_name);
	width = _rectForIndex[index].width ();
	_rectForIndex[index].setLeft (x);
	_rectForIndex[index].setWidth (width);

	if (_showBasicDetailsForItem && _showExtraDetailsForItem &&
	    item->isIndividual () && !item->isDummy ())
	{
		Individual *individual = dynamic_cast<Individual*>(item->fam ());
		if (individual->getBirthEvent ())
		{
			index = tm->index (item, Familier::e_col_birthDate);
			width = _rectForIndex[index].width ();
			_rectForIndex[index].setLeft (x);
			_rectForIndex[index].setWidth (width);
		}

		if (individual->getDeathEvent ())
		{
			index = tm->index (item, Familier::e_col_deathDate);
			width = _rectForIndex[index].width ();
			_rectForIndex[index].setLeft (x);
			_rectForIndex[index].setWidth (width);
		}

		if (item->isSpouseItem () &&
		    individual->getMarriageEvent (item->parentItem ()->id ()))
		{
			index = tm->index (item, Familier::e_col_marriageDate);
			width = _rectForIndex[index].width ();
			_rectForIndex[index].setLeft (x);
			_rectForIndex[index].setWidth (width);
		}
	}

	x += cellWidthExcludingIcon + _lineSeparation;

	if (childCountX && isExpanded (item))
		_rectForItem [item].setX (x_);
	else
		_rectForItem [item].setX (x_ + _itemSeparation);
	_rectForItem [item].setWidth (itemWidth);

	if (_idealWidth - _itemSeparation < x)
		_idealWidth = x + _itemSeparation;
}

QModelIndex FamilyTreeView::indexAtForFields (const QPoint &point, QModelIndex &index) const
{
	QModelIndex idx = index;
	QRectF rect = _rectForIndex.value (idx);
	if (rect.contains (point))
		return idx;

	TreeItem *item = getItem (index);

	if (_showExtraDetailsForItem && item->isIndividual () && !item->isDummy ())
	{
		idx = model ()->index (index.row (), Familier::e_col_birthDate, index.parent ());
		rect = _rectForIndex.value (idx);
		if (rect.contains (point))
			return idx;

		idx = model ()->index (index.row (), Familier::e_col_deathDate, index.parent ());
		rect = _rectForIndex.value (idx);
		if (rect.contains (point))
			return idx;

		idx = model ()->index (index.row (), Familier::e_col_marriageDate, index.parent ());
		rect = _rectForIndex.value (idx);
		if (rect.contains (point))
			return idx;

		idx = model ()->index (index.row (), Familier::e_col_maximum, index.parent ());
		rect = _rectForIndex.value (idx);
		if (rect.contains (point))
			return idx;
	}

	return QModelIndex ();
}

QModelIndex FamilyTreeView::indexAtRecursive (const QPoint &point, QModelIndex &index) const
{
	if (index.isValid ())
	{
		QModelIndex foundIdx = indexAtForFields (point, index);
		if (foundIdx.isValid ())
			return foundIdx;
	}

	for (int i = 0; i < model ()->rowCount (index); i++)
	{
		QModelIndex chIdx = model ()->index (i, Familier::e_col_name, index);
		QModelIndex foundIdx = indexAtRecursive (point, chIdx);
		if (foundIdx.isValid ())
			return foundIdx;
	}

	return QModelIndex();
}

bool FamilyTreeView::setSelectionRecursively (const QModelIndex &index, const QRect &rectangle,
					      QItemSelectionModel::SelectionFlags flags)
{
	bool ret = false;

	int firstRow = model()->rowCount(index);
	int lastRow = -1;
	for (int row = 0; row < model ()->rowCount (index); row++)
	{
		QModelIndex idx = model ()->index (row, Familier::e_col_name, index);
		if (setSelectionRecursively (idx, rectangle, flags))
			ret = true;

		QRectF rect = _rectForIndex.value (idx);
		if (!rect.intersects (rectangle))
			continue;

		idx = model ()->index (row, Familier::e_col_birthDate, index);
		rect = _rectForIndex.value (idx);
		if (rect.intersects (rectangle))
			continue;

		idx = model ()->index (row, Familier::e_col_deathDate, index);
		rect = _rectForIndex.value (idx);
		if (rect.contains (rectangle))
			continue;

		idx = model ()->index (row, Familier::e_col_marriageDate, index);
		rect = _rectForIndex.value (idx);
		if (rect.contains (rectangle))
			continue;

		idx = model ()->index (row, Familier::e_col_maximum, index);
		rect = _rectForIndex.value (idx);
		if (rect.contains (rectangle))
			continue;

		if (firstRow > row)
			firstRow = row;
		if (lastRow < row)
			lastRow = row;
	}

	if (firstRow != model()->rowCount() && lastRow != -1)
	{
		QItemSelection selection(model()->index(firstRow, 0, index),
					 model()->index(lastRow, 0, index));
		selectionModel()->select(selection, flags);
		ret = true;
	}

	return ret;
}

void FamilyTreeView::paintIndex (QModelIndex index, QPainter &painter)
{
	QRectF rect = viewportRectForIndex (index);
	if (rect.isValid () && viewport ()->rect ().intersects (rect.toRect ()))
	{
		QStyleOptionViewItem option = viewOptions();
		option.rect = rect.toRect ();
		if (selectionModel()->isSelected(index))
			option.state |= QStyle::State_Selected;
		if (currentIndex() == index)
			option.state |= QStyle::State_HasFocus;
		itemDelegate()->paint(&painter, option, index);
	}
}

void FamilyTreeView::paintIndexesRecursively (QModelIndex index, QPainter &painter)
{
	if (index.isValid ())
	{
		int row = index.row ();
		QModelIndex parentIndex = index.parent ();
		TreeItem *item = getItem (index);

		if (_showBasicDetailsForItem && (!item->isIndividual () || !item->isDummy ()))
			paintIndex (model ()->index (row, Familier::e_col_name, parentIndex), painter);
		else
		{
			QVariant foregroundColor = index.data (Qt::ForegroundRole);
			QColor color = foregroundColor.value<QColor>();
			QRectF rect = viewportRectForIndex (index);
			painter.save();
			QBrush brush;
			if (color.isValid ())
				brush = QBrush (color);
			else
				brush = QBrush (Qt::red);
			qreal brushSize;
			if (_showBasicDetailsForItem)
				brushSize = 8.0;
			else if (item->isDummy ())
				brushSize = 1.0;
			else
				brushSize = 4.0;
			painter.setPen (QPen (brush, brushSize));
			painter.drawEllipse (rect.toRect ());
			painter.restore ();
		}

		if (_showBasicDetailsForItem && _showExtraDetailsForItem &&
		    item->isIndividual () && !item->isDummy ())
		{
			QModelIndex imageIndex = model ()->index (row, Familier::e_col_maximum, parentIndex);
			QRectF target = viewportRectForIndex (imageIndex);
			if (target.isValid () && viewport ()->rect ().intersects (target.toRect ()))
			{
				QRectF source (0, 0, _iconSize, _iconSize);
				QVariant value = model ()->data (imageIndex, Qt::DecorationRole);
				if (value.canConvert <QIcon>())
				{
					QIcon icon = value.value <QIcon>();
					painter.drawPixmap (target, icon.pixmap (_iconSize, _iconSize), source);
				}
			}

			Individual *individual = dynamic_cast<Individual*>(item->fam ());
			if (individual->getBirthEvent ())
				paintIndex (model ()->index (row, Familier::e_col_birthDate, parentIndex),
					    painter);
			if (individual->getDeathEvent ())
				paintIndex (model ()->index (row, Familier::e_col_deathDate, parentIndex),
					    painter);
			if (item->isSpouseItem () && individual->getMarriageEvent (item->parentItem ()->id ()))
				paintIndex (model ()->index (row, Familier::e_col_marriageDate, parentIndex),
					    painter);
		}

		if (item)
		{
			QRect itemRect = viewportRectForItem (item).toRect ();
			qreal xmid = (itemRect.left () + itemRect.right ()) / 2;
			qreal ytop = itemRect.top ();
			qreal ybot = itemRect.bottom ();
			qreal xvLeft = viewport ()->rect ().left ();
			qreal xvRight = viewport ()->rect ().right ();

			painter.save();
			painter.setPen(QPen(Qt::white, 0.5));

			if ((_showBasicDetailsForItem && (!item->isIndividual () || !item->isDummy ())) &&
			    itemRect.isValid () && viewport ()->rect ().intersects (itemRect))
				paintOutline (painter, itemRect);
			else if (_showBasicDetailsForItem && item->isIndividual ())
			{
				TreeItem *leftSibling = item->parentItem ()->leftChildFrom (item);
				TreeItem *rightSibling = item->parentItem ()->rightChildFrom (item);

				if (leftSibling)
				{
					QRect rectForSibling = viewportRectForItem (leftSibling).toRect ();
					if (viewport ()->rect ().contains (rectForSibling) ||
					    (rectForSibling.left () < xvLeft && itemRect.right () > xvRight))
						painter.drawText (xvRight - 200 - _itemSeparation / 2,
								  ytop - 2 * _itemSeparation,
								  200, 20, Qt::AlignRight | Qt::AlignTop,
								  item->fam ()->getName () + " ->");
				}

				if (rightSibling)
				{
					QRect rectForSibling = viewportRectForItem (rightSibling).toRect ();
					if (viewport ()->rect ().contains (rectForSibling) ||
					    (itemRect.left () < xvLeft && rectForSibling.right () > xvRight))
						painter.drawText (xvLeft + _itemSeparation / 2,
								  ytop - _itemSeparation / 2 - 2,
								  "<- " + item->fam ()->getName ());
				}
			}
			if (itemRect.isValid () && !viewport ()->rect ().contains (itemRect))
			{
				if (_showBasicDetailsForItem && !item->isIndividual ())
				{
					if (xvLeft < itemRect.left ())
						painter.drawText (xvLeft + _itemSeparation / 2,
								  ytop - _itemSeparation / 2 -2,
								  item->fam ()->getName () + " ->");
					else
						painter.drawText (xvRight - 200 - _itemSeparation / 2,
								  ytop - 2 * _itemSeparation,
								  200, 20, Qt::AlignRight | Qt::AlignTop,
								  "<- " + item->fam ()->getName ());
				}
			}

			painter.drawLine (xmid, ytop, xmid, ytop - _itemSeparation);

			if (item->hasChild ())
			{
				if (isExpanded (item))
				{
					painter.drawLine (xmid, ybot, xmid, ybot + _itemSeparation + 1);
					if (ybot + _itemSeparation < viewport ()->rect ().bottom () &&
					    ybot + _itemSeparation > viewport ()->rect ().top () &&
					    item->childCount () > 1)
					{
						QRectF leftMostChildRect = viewportRectForItem (item->leftMostChild ());
						QRectF rightMostChildRect = viewportRectForItem (item->rightMostChild ());
						qreal xLeft = (leftMostChildRect.left () + leftMostChildRect.right ()) / 2;
						qreal xRight = (rightMostChildRect.left () + rightMostChildRect.right ()) / 2;
						qreal x1 = 0;
						qreal x2 = 0;
						if (xLeft < xvLeft)
							x1 = xvLeft;
						else
							x1 = xLeft;
						if (xRight > xvRight)
							x2 = xvRight;
						else
							x2 = xRight;
						painter.drawLine (x1, ybot + _itemSeparation, x2, ybot + _itemSeparation);
					}
				}
				else
				{
					QRect rectangle = viewportRectForItemTerminator (item).toRect ();
					paintTreeBranchTerminationForExpandable (painter, rectangle);
				}
			}
			else if (_showBasicDetailsForItem)
			{
				QRect rectangle = viewportRectForItemTerminator (item).toRect ();
				bool paintForDaughter = false;
				if (item->isIndividual () && !item->isSpouseItem ())
				{
					Individual *individual = dynamic_cast<Individual*>(item->fam ());
					if (individual->isFemale ())
						paintForDaughter = true;
				}

				if (paintForDaughter)
					paintTreeBranchTerminationForDaughters (painter, rectangle);
				else
					paintTreeBranchTerminationForAll (painter, rectangle);
			}

			if (!item->isIndividual ())
			{
				QRectF parentRect = viewportRectForItem (item->parentItem ());
				qreal xparentMid = (parentRect.left () + parentRect.right ()) / 2;
				qreal x1 = 0;
				qreal x2 = 0;
				if (xmid < xvLeft)
					x1 = xvLeft;
				else
					x1 = xmid;
				if (xparentMid > xvRight)
					x2 = xvRight;
				else
					x2 = xparentMid;
				painter.drawLine (x1, ytop - _itemSeparation - 1, x2, ytop - _itemSeparation - 1);
			}

			painter.restore();
		}
	}

	if (isExpanded (index))
	{
		for (int i = 0; i < model ()->rowCount (index); i++)
			paintIndexesRecursively (model()->index (i, Familier::e_col_name, index), painter);
	}
}

void FamilyTreeView::paintOutline(QPainter &painter, const QRect &rectangle)
{
	const QRect rect = rectangle.adjusted(0, 0, -1, -1);
	painter.save();
	painter.setPen(QPen(palette().dark().color(), 0.5));
	painter.drawRect(rect);
	painter.setPen(QPen(Qt::black, 0.5));
	painter.drawLine(rect.bottomLeft(), rect.bottomRight());
	painter.drawLine(rect.bottomRight(), rect.topRight());
	painter.restore();
}

void FamilyTreeView::paintTreeBranchTerminationForDaughters (QPainter &painter, const QRect &rectangle)
{
	qreal top = rectangle.top ();
	qreal xmid = (rectangle.left () + rectangle.right ()) / 2;
	int radius = _itemSeparation - _yStep;
	QPoint center (xmid, top + _yStep + radius);
	QPoint crossLineP1 (center.x () + radius * qCos (M_PI / 4),
			    center.y () - radius * qSin (M_PI / 4));
	QPoint crossLineP2 (center.x () - radius * qCos (M_PI / 4),
			    center.y () + radius * qSin (M_PI / 4));

	painter.save ();
	painter.setPen (QPen(Qt::red, 1.0));
	painter.drawEllipse (center, radius, radius);
	painter.drawLine (crossLineP1, crossLineP2);
	painter.restore ();
	painter.drawLine (xmid, top, xmid, top + _itemSeparation - _yStep - 1);
}

void FamilyTreeView::paintTreeBranchTerminationForAll (QPainter &painter, const QRect &rectangle)
{
	qreal top = rectangle.top ();
	qreal xmid = (rectangle.left () + rectangle.right ()) / 2;
	int xDiff = _itemSeparation - _xStep;
	int yDiff = _yStep;
	painter.save ();
	QPen pen;
	pen.setWidth (1.0);
	pen.setColor (QColor ("orange"));
	painter.setPen (pen);
	while (xDiff > 0)
	{
		painter.drawLine (xmid - xDiff, top + yDiff, xmid + xDiff, top + yDiff);
		xDiff -= _xStep;
		yDiff += _yStep;
	}
	painter.restore ();
	painter.drawLine (xmid, top, xmid, top + _itemSeparation - _yStep - 1);
}

void FamilyTreeView::paintTreeBranchTerminationForExpandable (QPainter &painter, const QRect &rectangle)
{
	qreal top = rectangle.top ();
	qreal xmid = (rectangle.left () + rectangle.right ()) / 2;
	qreal ymid = (rectangle.top () + rectangle.bottom ()) / 2 + 1;
	qreal plusLeft = xmid - _plusLength;
	qreal plusRight = xmid + _plusLength;
	qreal plusTop = top + _yStep + _plusPadding;
	qreal plusBot = plusTop + 2 * _plusLength;
	qreal rectLeft = plusLeft - _plusPadding;
	qreal rectTop = top + _yStep;
	qreal rectWidth = 2 * (_plusLength + _plusPadding);
	qreal rectHeight = rectWidth;

	painter.save ();
	painter.setPen (QPen(Qt::green, 1.0));
	painter.drawLine (plusLeft, ymid, plusRight, ymid);
	painter.drawLine (xmid, plusTop, xmid, plusBot);
	painter.restore ();

	painter.drawLine (xmid, top, xmid, rectTop - 1);
	painter.drawRect (rectLeft, rectTop, rectWidth, rectHeight);
}

void FamilyTreeView::collapse (TreeItem *item)
{
	if (!_listOfCollapsedItems.contains (item))
	{
		_listOfCollapsedItems << item;
		_hashNeedsUpdate = true;

		viewport ()->update ();
	}
}

void FamilyTreeView::expand (TreeItem *item)
{
	if (_listOfCollapsedItems.contains (item))
	{
		_listOfCollapsedItems.removeOne (item);
		_hashNeedsUpdate = true;

		viewport ()->update ();
	}
}

void FamilyTreeView::collapseAll (const QModelIndex &index)
{
	if (index.isValid ())
	{
		TreeItem *item = getItem (index);
		_listOfCollapsedItems << item;
	}

	for (int row = 0; row < model ()->rowCount (index); row++)
	{
		QModelIndex childIdx = model ()->index (row, 0, index);
		collapseAll (childIdx);
	}
}

bool FamilyTreeView::isExpanded (TreeItem *item) const
{
	return !_listOfCollapsedItems.contains (item);
}

bool FamilyTreeView::isExpanded (const QModelIndex &index) const
{
	if (!index.isValid ())
		return true;

	TreeItem *item = getItem (index);
	return isExpanded (item);
}

void FamilyTreeView::setExpanded (TreeItem *item, bool expanded)
{
	if (expanded)
		expand (item);
	else
		collapse (item);
}

void FamilyTreeView::setExpanded (const QModelIndex &index, bool expanded)
{
	if (expanded)
		expand (index);
	else
		collapse (index);
}

void FamilyTreeView::edit (const QModelIndex &index)
{
	QAbstractItemView::edit (index);
}

void FamilyTreeView::reset ()
{
	_hashNeedsUpdate = true;
	_listOfCollapsedItems.clear ();
	QAbstractItemView::reset ();
	viewport ()->update ();
}

void FamilyTreeView::collapse (const QModelIndex &index)
{
	if (!index.isValid () || !model ()->rowCount (index))
		return;

	TreeItem *item = getItem (index);
	collapse (item);
}

void FamilyTreeView::collapseAll ()
{
	_listOfCollapsedItems.clear ();
	_hashNeedsUpdate = true;

	QModelIndex rootIdx = rootIndex ();
	collapseAll (rootIdx);

	viewport ()->update ();
}

void FamilyTreeView::expand (const QModelIndex &index)
{
	if (!index.isValid ())
		return;

	TreeItem *item = getItem (index);
	expand (item);
}

void FamilyTreeView::expandAll ()
{
	_listOfCollapsedItems.clear ();
	_hashNeedsUpdate = true;

	viewport ()->update ();
}

void FamilyTreeView::setShowExtraDetailsForItem (const bool showExtraDetailsForItem)
{
	_showExtraDetailsForItem = showExtraDetailsForItem;
	_hashNeedsUpdate = true;
	viewport ()->update ();
}

void FamilyTreeView::setShowBasicDetailsForItem (const bool showBasicDetailsForItem)
{
	_showBasicDetailsForItem = showBasicDetailsForItem;
	_hashNeedsUpdate = true;
	viewport ()->update ();
}
