#include "Event.h"
#include "Individual.h"
#include "Family.h"
#include "Utilities.h"

#define OPERATOR(X) \
bool Event::operator X (const Event &event) const \
{ \
	if (_dated.wasDateParsed() && event._dated.wasDateParsed()) \
		return _dated X event._dated; \
\
	if (_dated.wasDateParsed()) \
		return true; \
	if (event._dated.wasDateParsed()) \
		return false; \
\
	if (_name == event._name) \
		return _id X event._id; \
\
	return _name X event._name; \
}

DEFINEAUTOITEMMANAGEMENT(Event)

Event::Event (const QDateTime &creationDateTime, const QDateTime &modificationDateTime,
	      const unsigned long modificationCount)
		      : IdableWithChildren (++s_MaxEventId, creationDateTime, modificationDateTime, modificationCount)
{
	INSERTITEMINTOITEMMANAGEMENT(Event)
}

Event::Event (const QDomElement &domElement) : IdableWithChildren (domElement)
{
	QString date = domElement.attribute("date", "").trimmed();
	Idable::unescapeStringFromXML (date);
	_dated = Dated (date);

	_place = domElement.attribute("place", "").trimmed();
	unescapeStringFromXML(_place);

	if (domElement.elementsByTagName("Description").count())
	{
		_description = domElement.elementsByTagName("Description").at(0).toElement().text().trimmed();
		unescapeStringFromXML(_description);
	}
	else
		_description = "";

	INSERTITEMINTOITEMMANAGEMENT(Event)
}

Event::Event (const Event &event) : IdableWithChildren (event)
{
	_dated = event._dated;
	_place = event._place;
	_description = event._description;
}

Event::~Event ()
{
	REMOVEITEMFROMITEMMANAGEMENT(Event)
}

void Event::setupItemLinks (const QDomElement &domElement)
{
	blockRecordingOfItemModifications ();
	IdableWithChildren::setupItemLinks (domElement);
	unblockRecordingOfItemModifications ();
}

DEFINITIONOFGETITEMWITHID(Event,Individual)

GETSETMAXIDCPP (Event)

QString Event::getSpecialEventName (NameRepresentation nameRepresentation) const
{
	EventType eventType = e_None;
	QList<IdableWithChildren*> idableWithChildrenList;
	foreach (IdableWithChildren *idableWithChildren, _children)
	{
		Individual *individual = dynamic_cast<Individual*>(idableWithChildren);
		if (individual->getBirthEvent () == this)
		{
			if (eventType == e_None || eventType == e_BirthEvent)
			{
				eventType = e_BirthEvent;
				idableWithChildrenList << idableWithChildren;
			}
			else
			{
				eventType = e_None;
				break;
			}
		}
		if (individual->getDeathEvent ()== this)
		{
			if (eventType == e_None || eventType == e_DeathEvent)
			{
				eventType = e_DeathEvent;
				idableWithChildrenList << idableWithChildren;
			}
			else
			{
				eventType = e_None;
				break;
			}
		}
		for (int i = 0; i < individual->spouseCount (); i++)
		{
			IDTYPE spid = individual->getSpouse (i)->getId ();
			if (individual->getMarriageEvent (spid)== this)
			{
				if (eventType == e_None || eventType == e_MarriageEvent)
				{
					eventType = e_MarriageEvent;
					idableWithChildrenList << idableWithChildren;
				}
				else
				{
					eventType = e_None;
					break;
				}
			}
			if (individual->getDivorceEvent (spid)== this)
			{
				if (eventType == e_None || eventType == e_DivorceEvent)
				{
					eventType = e_DivorceEvent;
					idableWithChildrenList << idableWithChildren;
				}
				else
				{
					eventType = e_None;
					break;
				}
			}
		}
	}

	if (eventType == e_None)
		return Idable::getName ();


	QString ret;
	switch (nameRepresentation)
	{
	case e_Anchor:
		ret = IdableWithChildren::getListOfAnchors (idableWithChildrenList,false, true);
		break;
	case e_PlainText:
		ret = IdableWithChildren::getListOfAnchors (idableWithChildrenList, false, false)
				.replace("<u>", "").replace("</u>", "");
		break;
	case e_Ids:
		for (int i = 0; i < idableWithChildrenList.size () - 1; i++)
			ret += QString ("[[iid=%1]], ").arg (idableWithChildrenList.at (i)->getId ());
		if (idableWithChildrenList.size () > 1)
			ret += "and ";
		ret += QString ("[[iid=%1]]").arg (idableWithChildrenList.last ()->getId ());
		break;
	}

	if (eventType == e_BirthEvent)
		return "Birth of " + ret;
	else if (eventType == e_DeathEvent)
		return "Death of " + ret;
	else if (eventType == e_MarriageEvent)
		return "Marriage of " + ret;
	else if (eventType == e_DivorceEvent)
		return "Divorce of " + ret;

	return Idable::getName ();
}

SETFUNCTION (Event, Dated, Dated &, dated)

bool Event::setDated (const QString &dated)
{
	return setDated (Dated (dated));
}

SETFUNCTION (Event, Place, QString &, place)
SETFUNCTION (Event, Description, QString &, description)

QString Event:: getChildTypeName () const
{
	return "Member";
}

QTextStream& operator << (QTextStream &stream, Event &event)
{
	stream << "\t<Event ";
	stream << dynamic_cast <Idable &> (event);
	stream << event._dated;
	if (event._place.length())
	{
		QString place = event._place;
		Idable::escapeStringForXML (place);
		stream << " place=\"" << place << "\"";
	}
	stream << ">" << endl;
	stream << dynamic_cast <IdableWithChildren &> (event);
	if (event._description.length())
	{
		QString description = event._description;
		Idable::escapeStringForXML(description);
		stream << "\t\t<Description>" << endl
				<< description << endl
				<< "\t\t</Description>" << endl;
	}
	stream << "\t</Event>" << endl;
	return stream;
}

QTextStream& Event::toGEDCOM(QTextStream &stream) const
{
	QDate eventDate = _dated.getDate ();
	QString eventDateString;
	int quay = _dated.quay ();
	if (quay == 3)
		eventDateString = eventDate.toString("dd MMM yyyy");
	else
		eventDateString = toDisplayDate ();

	stream << "2 DATE " << eventDateString << endl;
	stream << "2 SOUR" << endl;
	stream << "3 QUAY " << quay << endl;
	if (_place.size())
		stream << "2 PLAC " << _place << endl;
	if (_description.length())
		stream << "2 NOTE @N" << ++ IdableWithChildren::s_commonNotesID << "@" << endl;

	return stream;
}

void Event::fromGEDCOM (const QStringList &gedcomEntryStringList,
			const QHash <QString, QStringList> &xRefToNoteGEDCOMInfoHash)
{
	blockRecordingOfItemModifications ();

	foreach (QString line, gedcomEntryStringList)
	{
		QList<QString> list = line.split (QRegExp ("\\s+"));
		if (list.count () <= 1)
			continue;

		QString tag = list.at (1);
		if (tag == "DATE")
		{
			if (list.size () < 3)
				continue;

			QString dateString = list.at (2);
			for (int i = 3; i < list.size (); i++)
				dateString += " " + list.value (i);

			QDate date = QDate::fromString (dateString, "dd MMM yyyy");
			if (date.isValid ())
				setDated (date);
		}
		else if (tag == "PLAC")
		{
			if (list.size () < 3)
				continue;

			QString placeString = list.at (2);
			for (int i = 3; i < list.size (); i++)
				placeString += " " + list.value (i);

			setPlace (placeString);
		}
		else if (tag == "NOTE")
		{
			if (list.size () < 3)
				continue;
			QString fxRef = list.at (2);
			const QStringList &noteGedComList = xRefToNoteGEDCOMInfoHash.value (fxRef);
			_description = IdableWithChildren::notesFromGEDCOM (noteGedComList);
		}
	}

	unblockRecordingOfItemModifications ();
}

QDebug operator << (QDebug debug, Event &event)
{
	debug << "Event Start" << endl;
	debug << dynamic_cast <Idable &> (event);
	debug << "Place: " << event._place << endl;
	debug << event._dated << endl;
	debug << dynamic_cast <IdableWithChildren &> (event);
	debug << "Event End" << endl;
	debug << endl;
	return debug;
}

OPERATOR (<)
OPERATOR (>)

QString Event::getListOfAnchors (const int type, const bool useFullName, const bool createAnchor)
{
	switch (AnchorType(type))
	{
	case members:
		return IdableWithChildren::getListOfAnchors(_children, useFullName, createAnchor);
	case membersListingOnly:
		return (IdableWithChildren::getListOfAnchors(_children, useFullName, false))
				.replace("<u>", "").replace("</u>", "");
	}

	return "";
}

QString Event::toDisplayDate (Dated &refDate) const
{
	return _dated.toDisplayString(refDate);
}

QString Event::toDisplayDate (Event *refEvent) const
{
	Dated refDate = refEvent->getDated ();
	return toDisplayDate (refDate);
}

QString Event::toDisplayDate () const
{
	return _dated.toDisplayString();
}

bool Event::hasDescription () const
{
	return _description.size() > 0;
}

bool Event::hasEventMember (const IDTYPE id) const
{
	return isChild(id);
}

QString Event::getDescriptionInDisplayFormat ()
{
	QString output;
	Utilities::convertFTMLToDisplayFormat (_description, output, false, true);
	return output;
}

QString Event::getEventInformation ()
{
	QString ret = "<i>Name:</i> " + getUnderlinedName();
	ret += "<br><i>Date:</i> " + toDisplayDate();
	if (_place.length ())
		ret += "<br><i>Place:</i> " + _place;
	ret += "<br><i>Members:</i> " + getListOfAnchors(members, false, true);
	if (hasDescription())
	{
		ret += "<br><i>Description:</i><br>";
		ret += getDescriptionInDisplayFormat ();
	}
	ret += "<br><br>";

	return ret;
}

bool Event::didEventHappenToday () const
{
	QDate today = QDate::currentDate ();
	return _dated.month () == today.month () && _dated.day () == today.day ();
}

bool Event::fullTextSearch (const QRegExp &searchRegExp, QStringList &resultsList) const
{
	QStringList tempList;
	bool found = IdableWithChildren::fullTextSearch (searchRegExp, tempList);

	if (_dated.fullTextSearch (searchRegExp, resultsList))
	{
		found = true;
		tempList << QString ("Dated: %1").arg (_dated.toDisplayString ());
	}

	if (_place.contains (searchRegExp))
	{
		found = true;
		tempList << QString ("Place: %1").arg (_place);
	}

	QString expandedHTMLDescription;
	QString expandedTextDescription = Utilities::convertFTMLToDisplayFormat (_description, expandedHTMLDescription, true, false);
	if (expandedTextDescription.contains (searchRegExp))
	{
		found = true;
		tempList << QString ("Description:<br>%1<br>--- END ---").arg (expandedTextDescription);
	}

	if (found)
	{
		resultsList << QString ("Event: %1").arg (getName ());
		resultsList << tempList;
	}

	return found;
}
