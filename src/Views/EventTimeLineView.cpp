#include "EventTimeLineView.h"
#include "EventLineModel.h"
#include <QApplication>
#include <QScrollBar>
#include <QMouseEvent>
#include <QDate>
#include <QPainter>
#include <QRect>
#include <QFontMetrics>

EventTimeLineView::EventTimeLineView (QWidget *parent) :
		QAbstractItemView(parent),
		c_dotRadius (3), c_minYear (1900), c_maxYear (QDate::currentDate ().year ()),
		_minYear (c_minYear), _maxYear (c_maxYear),
		_yearUnit (10), _idealWidth (0), _idealHeight (0),
		_bottomBarWidth (30), _hashNeedsUpdate (true)
{
	setFocusPolicy(Qt::WheelFocus);
	setFont(QApplication::font("QListView"));
	horizontalScrollBar()->setRange(0, 0);
	verticalScrollBar()->setRange(0, 0);
}

void EventTimeLineView::setModel (QAbstractItemModel *model)
{
	QAbstractItemView::setModel (model);
	_hashNeedsUpdate = true;
}

QRect EventTimeLineView::visualRect (const QModelIndex &index) const
{
	QRect rect;
	if (index.isValid ())
		rect = viewportRectForIndex (index).toRect();
	return rect;
}

void EventTimeLineView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint)
{
	QRect viewRect = viewport()->rect();
	QRect itemRect = visualRect(index);

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
	viewport()->update();
}

QModelIndex EventTimeLineView::indexAt (const QPoint &point) const
{
	QPoint relPoint (point);
	relPoint.rx() += horizontalScrollBar()->value();
	relPoint.ry() += verticalScrollBar()->value();

	calculateRectsIfRequired ();

	QHashIterator<int, QRectF> i(_rectForRow);
	while (i.hasNext())
	{
		i.next();
		if (i.value().contains(relPoint))
			return model()->index(i.key(), 0, rootIndex ());
	}
	return QModelIndex();
}

void EventTimeLineView::dataChanged (const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	_hashNeedsUpdate = true;
	QAbstractItemView::dataChanged (topLeft, bottomRight);
	viewport ()->update ();
}

void EventTimeLineView::rowsInserted (const QModelIndex &parent, int start, int end)
{
	_hashNeedsUpdate = true;
	QAbstractItemView::rowsInserted (parent, start, end);
}

void EventTimeLineView::rowsAboutToBeRemoved (const QModelIndex &parent, int start, int end)
{
	_hashNeedsUpdate = true;
	QAbstractItemView::rowsAboutToBeRemoved (parent, start, end);
}

void EventTimeLineView::updateGeometries()
{
	QFontMetrics fm(font());
	horizontalScrollBar()->setSingleStep(fm.width("n"));
	horizontalScrollBar()->setPageStep(viewport()->width());
	horizontalScrollBar()->setRange(0, qMax(0.0, _idealWidth - viewport()->width()));
	const int RowHeight = fm.height();
	verticalScrollBar()->setSingleStep(RowHeight);
	verticalScrollBar()->setPageStep(viewport()->height());
	verticalScrollBar()->setRange(0, qMax(0.0, _idealHeight + _bottomBarWidth
					      - viewport()->height()));
}

bool EventTimeLineView::edit (const QModelIndex &, EditTrigger, QEvent *)
{
	return false;
}

QModelIndex EventTimeLineView::moveCursor (QAbstractItemView::CursorAction cursorAction,
					Qt::KeyboardModifiers modifiers)
{
	Q_UNUSED(modifiers)

	QModelIndex index = currentIndex ();
	if (index.isValid ())
	{
		switch (cursorAction)
		{
		case MoveLeft:
			index = model ()->index (index.row () - 1, index.column (), index.parent ());
			break;
		case MoveRight:
			index = model ()->index (index.row () - 1, index.column (), index.parent ());
			break;
		case MoveUp:
			break;
		case MoveDown:
			break;
		default:
			break;
		}
	}

	viewport()->update();
	return index;
}

int EventTimeLineView::horizontalOffset() const
{
	return horizontalScrollBar()->value();
}

int EventTimeLineView::verticalOffset() const
{
	return verticalScrollBar()->value();
}

void EventTimeLineView::scrollContentsBy(int dx, int dy)
{
	scrollDirtyRegion(dx, dy);
	viewport()->scroll(dx, dy);
}

bool EventTimeLineView::isIndexHidden (const QModelIndex &index) const
{
	Q_UNUSED(index)
	return false;
}

void EventTimeLineView::setSelection(const QRect &rect,
				  QFlags<QItemSelectionModel::SelectionFlag> flags)
{
	Q_UNUSED(rect)

	QModelIndex invalid;
	QItemSelection selection(invalid, invalid);
	selectionModel()->select(selection, flags);

	update();
}

void EventTimeLineView::mousePressEvent(QMouseEvent *event)
{
	QAbstractItemView::mousePressEvent(event);
	setCurrentIndex(indexAt(event->pos()));
}

void EventTimeLineView::paintEvent (QPaintEvent *event)
{
	Q_UNUSED(event)

	const int relativeBarHeight = 5;
	const int verticalBarsLength = 5;
	QFontMetrics fm (font ());
	_bottomBarWidth = relativeBarHeight + fm.height ();
	if (_bottomBarWidth < 30)
		_bottomBarWidth = 30;

	updateGeometries ();

	QPainter painter (viewport());
	QRect viewportRect = viewport ()->rect ();
	painter.setRenderHints (QPainter::Antialiasing | QPainter::TextAntialiasing);
	painter.save();
	painter.setPen(QPen(Qt::white, 1.5));

	int yVerticalReferencePosition = _idealHeight - verticalScrollBar ()->value ();
	painter.drawLine (viewportRect.left (), yVerticalReferencePosition + relativeBarHeight,
			  viewportRect.right (), yVerticalReferencePosition + relativeBarHeight);

	for (int year = _minYear; year <= _maxYear; year += 5)
	{
		int xYear = 5 * _yearUnit + (year - _minYear) * _yearUnit
			    - horizontalScrollBar ()->value ();
		if (xYear < viewportRect.left () || xYear > viewportRect.right ())
			continue;

		painter.drawLine (xYear, yVerticalReferencePosition + relativeBarHeight,
				  xYear, yVerticalReferencePosition + relativeBarHeight + verticalBarsLength);
		QString strYear = QString("%1").arg (year);
		int fw = fm.width (strYear);
		painter.drawText (xYear -fw / 2,
				  yVerticalReferencePosition + _bottomBarWidth - relativeBarHeight,
				  strYear);
	}

	for (int i = 0; i < model()->rowCount(rootIndex()); i++)
	{
		QModelIndex index = model()->index(i, 0, rootIndex());
		QRectF rect = viewportRectForIndex (index);
		if (!rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height())
			continue;

		int x = (rect.left () + rect.right ()) / 2;
		int y = (rect.top () + rect.bottom ()) / 2;
		int r = (rect.right () - rect.left ()) / 2;
		painter.drawEllipse (x, y, r, r);
	}

	painter.restore ();
}

void EventTimeLineView::resizeEvent(QResizeEvent*)
{
	_hashNeedsUpdate = true;
	calculateRectsIfRequired ();
	updateGeometries();
}

QRegion EventTimeLineView::visualRegionForSelection (const QItemSelection &selection) const
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

QRectF EventTimeLineView::viewportRectForIndex (const QModelIndex &index) const
{
	calculateRectsIfRequired ();

	QRectF rect = _rectForRow.value (index.row ());
	if (!rect.isValid ())
		return rect;
	return QRectF (rect.x() - horizontalScrollBar()->value(),
		       rect.y() - verticalScrollBar()->value(),
		       rect.width(), rect.height());
}

void EventTimeLineView::calculateRectsIfRequired () const
{
	if (!_hashNeedsUpdate)
		return;

	_minYear = c_minYear;
	_maxYear = c_maxYear;
	_idealHeight = 366 + 10;
	if (_idealHeight + _bottomBarWidth < viewport ()->height ())
		_idealHeight = viewport ()->height () - _bottomBarWidth;

	if (!model ()->hasChildren (rootIndex ()))
	{
		_idealWidth = 5 * _yearUnit + (_maxYear - _minYear) * 5 + 5 * _yearUnit;
		return;
	}

	int minModelYear = model ()->data (model()->index (0, EventLineModel::e_col_Year,
							   rootIndex ())).toInt ();
	int maxModelYear = model ()->data (model ()->index (model ()->rowCount (rootIndex ()) - 1,
							    EventLineModel::e_col_Year,
							    rootIndex ())).toInt ();
	if (_minYear > minModelYear)
		_minYear = minModelYear;
	if (_maxYear < maxModelYear)
		_maxYear = maxModelYear;
	_idealWidth = 5 * _yearUnit + (_maxYear - _minYear + 1) * _yearUnit + 5 * _yearUnit;

	for (int i = 0; i < model ()->rowCount (rootIndex ()); i++)
	{
		QModelIndex itemIndex = model ()->index (i, 0, rootIndex ());
		QModelIndex index = model ()->index (i, EventLineModel::e_col_Year, rootIndex ());
		int year = model ()->data (index, Qt::DisplayRole).toInt ();
		index = model ()->index (i, EventLineModel::e_col_Month, rootIndex ());
		int month = model ()->data (index, Qt::DisplayRole).toInt ();
		index = model ()->index (i, EventLineModel::e_col_Day, rootIndex ());
		int day = model ()->data (index, Qt::DisplayRole).toInt ();
		if (year < 0)
			continue;
		qreal x = 5 * _yearUnit + (year - _minYear) * _yearUnit;
		qreal y = 0;
		if (month < 0)
			y = _idealHeight;
		else
		{
			QDate date (year, month, day < 0 ? 1 : day);
			y = _idealHeight - date.dayOfYear ();
		}

		_rectForRow [itemIndex.row ()] = QRectF (x - c_dotRadius, y - c_dotRadius,
							 c_dotRadius, c_dotRadius);
	}
}
