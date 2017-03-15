#include "EventModel.h"
#include <QPixmap>
#include <QTimer>

EventModel::EventModel(Individual *individual, QObject *parent)
	: QAbstractItemModel(parent), _individual(individual)
{
	populateModel ();
}

QModelIndex EventModel::index(int row, int column, const QModelIndex &parent) const
{
	if (row < 0 || parent.isValid())
		return QModelIndex();

	if (row < _indexToEventMap.count())
		return createIndex(row, column, _indexToEventMap.value(row));
	else
		return QModelIndex();
}

QModelIndex EventModel::parent(const QModelIndex &index) const
{
	Q_UNUSED(index)
	return QModelIndex();
}

bool EventModel::hasChildren(const QModelIndex &parent) const
{
	return rowCount(parent) > 0;
}

int EventModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return _indexToEventMap.count();
}

int EventModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED (parent)
	return colMaximum;
}

QVariant EventModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal)
	{
		Columns column = static_cast<Columns>(section);
		if (role == Qt::DisplayRole)
			switch (column)
			{
			case colName: return "Name";
			case colDate: return "Date";
			case colPlace: return "Place";
			case colIsMember: return "Is Member";
			case colDescription: return "Description";
			case colMembers: return "Members";
			default: return QVariant();
			}
	}
	else if (orientation == Qt::Vertical)
		if (role == Qt::DisplayRole)
			return section + 1;

	return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags EventModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	Columns column = static_cast<Columns>(index.column());
	Qt::ItemFlags itemFlags = Qt::ItemIsEnabled;
	switch (column)
	{
	case colName:
		itemFlags |= Qt::ItemIsSelectable | Qt::ItemIsEditable;
		break;
	case colDate:
		itemFlags |= Qt::ItemIsEditable;
		break;
	case colPlace:
		itemFlags |= Qt::ItemIsEditable;
		break;
	case colDescription:
		itemFlags |= Qt::ItemIsEditable;
		break;
	case colIsMember:
		if (_individual)
			itemFlags |= Qt::ItemIsEditable;
		break;
	case colMembers:
		if (!_individual)
			itemFlags |= Qt::ItemIsEditable;
		break;
	default:
		break;
	}

	return itemFlags;
}

QVariant EventModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	int row = index.row();
	if (row < 0 || row >= _indexToEventMap.count())
		return QVariant();

	Columns column = static_cast<Columns>(index.column());

	if (role == Qt::DisplayRole)
	{
		switch (column)
		{
		case colName:
			{
				Event *event = _indexToEventMap.value(row);
				QString name = event->getSpecialEventName ();
				QString date;
				if (_individual)
				{
					date = event->getDated().getDateString();
					if (date.length() > 10)
						date = date.mid(0, 7).append("...");
					date = " - " + date;
				}

				return QString ("%1%2 (%3)").arg(name).arg(date).arg(_indexToEventMap.value(row)->getId());
			}
		case colDate:
			{
				QString date = _indexToEventMap.value(row)->toDisplayDate();

				if (_individual && date.length () >= 15)
					return date.mid(0, 12).append("...");
				return date;
			}
		case colPlace:
			{
				QString place = _indexToEventMap.value(row)->getPlace();

				if (_individual && place.length () >= 12)
					return place.mid(0, 12).append("...");
				return place;
			}
		case colIsMember:
			if (_indexToEventMap.value(row)->isChild(_individual->getId()))
				return "True";
			else
				return "False";
		case colMembers:
			{
				Event *event = _indexToEventMap.value(row);
				QString memberList = event->getListOfAnchors(Event::membersListingOnly, false, false);

				if (_individual && memberList.length () >= 20)
					return memberList.mid(0, 17).append("...");
				return memberList;
			}
		case colDescription:
			if (_indexToEventMap.value(row)->getDescription().length()) return "...";
		default:
			break;
		}
	}
	else if (role == Qt::EditRole)
	{
		switch (column)
		{
		case colName: return _indexToEventMap.value(row)->getName();
		case colDate: return _indexToEventMap.value(row)->getDated().getDateString();
			/*{
				Dated dated = _indexToEventMap.value(row)->getDated();
				if (dated.wasDateParsed())
					return dated.getDate();
				else
					return dated.getDateString();
			}*/
		case colPlace: return _indexToEventMap.value(row)->getPlace();
		case colDescription:
			return _indexToEventMap.value(row)->getDescriptionInDisplayFormat ();
		case colIsMember: return _indexToEventMap.value(row)->isChild(_individual->getId());
		default:
			break;
		}
	}
	else if (role == Qt::DecorationRole)
	{
		switch (column)
		{
		case colName:
			{
				if (_individual)
				{
					Event *event = _indexToEventMap.value(row);
					if (_individual->getBirthEvent() == event)
						return QPixmap(":images/alive.png");
					if (_individual->getDeathEvent() == event)
						return QPixmap(":images/dead.png");
					for (int i = 0; i < _individual->spouseCount(); i++)
					{
						IDTYPE spid = _individual->getSpouse(i)->getId();
						if (_individual->getMarriageEvent(spid) == event)
							return QPixmap(":images/marriage.png").scaledToHeight(20);
						if (_individual->getDivorceEvent(spid) == event)
							return QPixmap(":images/divorce.png").scaledToHeight(20);
					}
					if (event->hasEventMember(_individual->getId()))
						return QPixmap(":images/event.png").scaledToHeight(20);
				}
				else
					return QPixmap(":images/event.png").scaledToHeight (20);
			}
		/*case colIsMember:
			if (_indexToEventMap.value(row)->isChild(_individual->getId()))
				return QPixmap(":images/alive.png");
			else
				return QPixmap(":images/dead.png");*/
		default:
			break;
		}
	}
	else if (role == Qt::ToolTipRole)
	{
		switch (column)
		{
		case colName:
			if (_individual)
			{
				Event *event = _indexToEventMap.value(row);
				return event->getEventInformation ();
			}
		case colDate:
			return _indexToEventMap.value(row)->toDisplayDate();
		case colPlace:
			return _indexToEventMap.value(row)->getPlace();
		case colIsMember:
			{
				QString memInfo = _individual->getName() + " is ";
				if (!_indexToEventMap.value(row)->isChild(_individual->getId()))
					memInfo += "not ";
				memInfo += "a member. Set here true to make " +
					   _individual->getName() +
					   " a member. Set here false to revoke " +
					   _individual->genderBias(Familier::hisher, false) +
					   " membership.";
				return memInfo;
			}
		case colMembers:
			{
				Event *event = _indexToEventMap.value(row);
				return event->getListOfAnchors(Event::members, false, false);
			}
		case colDescription:
			return _indexToEventMap.value(row)->getDescriptionInDisplayFormat ();
		default:
			break;
		}
	}

	return QVariant();
}

bool EventModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return false;

	if (role != Qt::EditRole)
		return false;

	int row = index.row();
	Columns column = static_cast<Columns>(index.column());

	try
	{
		switch (column)
		{
		case colName: throw _indexToEventMap.value(row)->setName(value.toString());
		case colDate:
/*			if (value.toDate().isValid())
				throw _indexToEventMap.value(row)->setDated(Dated(value.toDate()));
			else*/
			if (_indexToEventMap.value(row)->setDated(value.toString()))
			{
				QTimer::singleShot (500, this, SLOT(regenerateModel ()));
				throw true;
			}
			else
				throw false;
		case colPlace: throw _indexToEventMap.value(row)->setPlace(value.toString());
		case colDescription: throw _indexToEventMap.value(row)->setDescription(value.toString());
		case colIsMember:
			if (qVariantCanConvert<bool>(value))
			{
				if (value.toBool())
					throw _indexToEventMap.value(row)->appendChild(_individual);
				else
					throw _indexToEventMap.value(row)->removeChild(_individual);
			}
		default: throw false;
		}
	}
	catch (bool what)
	{
		if (what)
			emit dataChanged(index, index);
		return what;
	}

	return false;
}

bool EventModel::insertRow(int row, const QModelIndex & parent)
{
	if (row < 0 || row > rowCount())
		return false;

	Event *event = new Event();

	beginInsertRows(parent, row, row);
	_indexToEventMap[_indexToEventMap.count()] = event;
	endInsertRows();

	return true;
}

bool EventModel::removeRow(int row, const QModelIndex & parent)
{
	if (row < 0 || row > rowCount())
		return false;

	Event *event = _indexToEventMap.value(row);

	beginRemoveRows(parent, row, row);
	for (int i = row; i < _indexToEventMap.count () - 1; i++)
		_indexToEventMap[i] = _indexToEventMap.value (i+1);
	_indexToEventMap.remove(_indexToEventMap.count () - 1);
	endRemoveRows();

	foreach (Union *aUnion, Union::s_UnionList)
	{
		/* Removing all the references to the event from all the Unions,
		   so that program doesn't crash when an event is removed. Note,
		   same event can be made to be marriageEvent and divorceEvent
		   at the same time, so checking for both at the same time.
		 */
		if (aUnion->getMarriageEvent () == event)
			aUnion->setMarriageEvent (NULL);
		if (aUnion->getDivorceEvent () == event)
			aUnion->setDivorceEvent (NULL);
	}

	foreach (Individual *individual, Individual::s_IndividualList)
	{
		if (individual->getBirthEvent () == event)
			individual->setBirthEvent (NULL);
		if (individual->getDeathEvent () == event)
			individual->setDeathEvent (NULL);
	}

	delete event;

	return true;
}

void EventModel::populateModel ()
{
	QList<Event> events;
	foreach (Event *event, Event::s_EventList)
		events << *event;

	qSort (events);

	_indexToEventMap.clear ();
	int row = 0;
	foreach (Event event, events)
	{
		if (Event::s_IdToEventHash.contains(event.getId()))
		{
			_indexToEventMap[row] = Event::s_IdToEventHash.value(event.getId());
			row ++;
		}
		else
			qDebug () << "Reject" << row << event.getId() << event.getName();
	}
}

int EventModel::getIndexFromId(const unsigned long eid) const
{
	int ret = 0;
	foreach (Event *event, _indexToEventMap)
	{
		if (event->getId() == eid)
			return ret;

		ret ++;
	}

	return -1;
}

Event *EventModel::getEventFromIndex(const int idx) const
{
	if (idx < 0 || idx >= _indexToEventMap.count())
		return NULL;

	return _indexToEventMap.value(idx);
}

void EventModel::regenerateModel ()
{
	beginResetModel ();
	_indexToEventMap.clear ();
	populateModel ();
	endResetModel ();

	emit eventModelRegenerated ();
}

void EventModel::clear ()
{
	beginResetModel ();
	_indexToEventMap.clear ();
	endResetModel ();
}
