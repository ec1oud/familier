#include "Union.h"
#include "Individual.h"
#include "FemaleIndividual.h"

DEFINEAUTOITEMMANAGEMENT(Union)

Union::Union (const QDateTime &creationDateTime, const QDateTime &modificationDateTime,
	      const unsigned long modificationCount)
		      : IdableWithChildren (++s_MaxUnionId, creationDateTime, modificationDateTime, modificationCount)
{
	_name ="";
	init ();
}

Union::Union (const QDomElement &domElement) : IdableWithChildren (domElement)
{
	_name = "";
	initLinks();
}

Union::Union (const Union &aUnion) : IdableWithChildren (aUnion)
{
	_husband = aUnion._husband;
	_wife = aUnion._wife;
	_marriageEvent = aUnion._marriageEvent;
	_divorceEvent = aUnion._divorceEvent;
}

Union::~Union ()
{
	REMOVEITEMFROMITEMMANAGEMENT(Union)
}

void Union::setupItemLinks (const QDomElement &domElement)
{
	blockRecordingOfItemModifications ();

	IdableWithChildren::setupItemLinks (domElement);

	LOADITEMFROMXMLWITHOUTCREATION(Individual,husband)
	LOADITEMFROMXMLWITHOUTCREATION(Individual,wife)

	LOADITEMFROMXMLWITHOUTCREATION(Event,marriageEvent)
	LOADITEMFROMXMLWITHOUTCREATION(Event,divorceEvent)

	unblockRecordingOfItemModifications ();
}

/*IdableWithChildren* Union::getItemWithId (const IDTYPE id) const
{
	if (Individual::s_IdToIndividualMap.contains(id))
		return Individual::s_IdToIndividualMap.value(id);
	else
		return NULL;
}*/

DEFINITIONOFGETITEMWITHID(Union,Individual)

void Union::init ()
{
	initLinks();
}

void Union::initLinks()
{
	_husband = 0;
	_wife = 0;

	_marriageEvent = 0;
	_divorceEvent = 0;

	INSERTITEMINTOITEMMANAGEMENT(Union)
}

bool Union::appendChild (const IdableWithChildren *child)
{
	if (IdableWithChildren::appendChild(child))
	{
		propagateChildAlteration ();
		return true;
	}

	return false;
}

bool Union::removeChild (const IdableWithChildren *child)
{
	if (IdableWithChildren::removeChild(child))
	{
		propagateChildAlteration ();
		return true;
	}

	return false;
}

bool Union::removeChild (const IDTYPE chid)
{
	if (IdableWithChildren::removeChild(chid))
	{
		propagateChildAlteration ();
		return true;
	}

	return false;
}

bool Union::insertChild (const long chidx, const IdableWithChildren *child)
{
	if (IdableWithChildren::insertChild(chidx, child))
	{
		propagateChildAlteration ();
		return true;
	}

	return false;
}

bool Union::removeChildAt (const long chidx)
{
	if (IdableWithChildren::removeChildAt(chidx))
	{
		propagateChildAlteration ();
		return true;
	}

	return false;
}

IdableWithChildren* Union::takeChildAt (const long chidx)
{
	IdableWithChildren* child = IdableWithChildren::takeChildAt(chidx);
	if (child)
		propagateChildAlteration ();

	return child;
}

void Union::propagateChildAlteration () const
{
	if (_husband)
		_husband->unsetStatsUpToDate ();
}

QString Union::getName () const
{
	QString name = "Union of";
	if (_husband)
	{
		name += _husband->getName();
		if (_wife)
			name += " with " + _wife->getName();
	}
	else if (_wife)
		name += _wife->getName();
	else
		name += " unknowns";

	return name;
}

GETSETMAXIDCPP (Union)

SETFUNCTIONWITHCONSTCAST (Union, Husband, Individual*, husband)
SETFUNCTIONWITHCONSTCAST (Union, Wife, Individual*, wife)
SETFUNCTIONWITHCONSTCAST (Union, MarriageEvent, Event *, marriageEvent)
SETFUNCTIONWITHCONSTCAST (Union, DivorceEvent, Event *, divorceEvent)

/*bool Union::makeChildAdopted (const IDTYPE chid)
{
	if (-1 != _adoptedChildren.indexOf (chid))
		return false;

	_adoptedChildren.append (chid);
	return true;
}

bool Union::makeChildUnadopted (const IDTYPE chid)
{
	return _adoptedChildren.removeOne (chid);
}

bool Union::isChildAdopted (const IDTYPE chid)
{
	return -1 != _adoptedChildren.indexOf (chid);
}*/

QString Union::getChildTypeName () const
{
	return "Child";
}


QTextStream& operator << (QTextStream &stream, Union &aUnion)
{
	stream << "\t<Union ";
	stream << dynamic_cast <Idable &> (aUnion);
	if (aUnion._husband)
		stream << " husband=\"" << aUnion._husband->getId() << "\"";
	if (aUnion._wife)
		stream << " wife=\"" << aUnion._wife->getId() << "\"";
	if (aUnion._marriageEvent)
		stream << " marriageEvent=\"" << aUnion._marriageEvent->getId() << "\"";
	if (aUnion._divorceEvent)
		stream << " divorceEvent=\"" << aUnion._divorceEvent->getId() << "\"";
	stream << ">" << endl;
	stream << dynamic_cast <IdableWithChildren &> (aUnion);
	stream << "\t</Union>" << endl;
	return stream;
}

QTextStream& Union::toGEDCOM(QTextStream &stream) const
{
	stream << "0 @F" << _id << "@ FAM" << endl;
	if (_husband)
		stream << "1 HUSB @I" << _husband->getId () << "@" << endl;
	if (_wife && _wife->isNonDummy ())
		stream << "1 WIFE @I" << _wife->getId () << "@" << endl;
	if (_marriageEvent)
	{
		stream << "1 MARR" << endl;
		_marriageEvent->toGEDCOM (stream);
	}
	if (_divorceEvent)
	{
		stream << "1 DIV" << endl;
		_divorceEvent->toGEDCOM (stream);
	}
	foreach (IdableWithChildren *child, _children)
		stream << "1 CHIL @I" << child->getId() << "@" << endl;
	return stream;
}

void Union::fromGEDCOM (const QStringList &gedcomEntryStringList,
			const QHash<QString, Individual*> &xRefToIndividualHash,
			const QHash <QString, QStringList> &xRefToNoteGEDCOMInfoHash)
{
	blockRecordingOfItemModifications ();

	QStringList eventGedComEntryStringList;
	enum { none, marriage, divorce } eventMode = none;

	foreach (QString line, gedcomEntryStringList)
	{
		QList<QString> list = line.split (QRegExp ("\\s+"));
		if (list.count () <= 1)
			continue;

		int level = list.at (0).toInt ();

		switch (level)
		{
		case 1:
			if (eventMode != none)
			{
				switch (eventMode)
				{
				case marriage:
					{
						Event *event = new Event;
						event->setName ("Marriage");
						if (getMarriageEvent ())
							delete getMarriageEvent ();
						setMarriageEvent (event);
						event->fromGEDCOM (eventGedComEntryStringList,
								   xRefToNoteGEDCOMInfoHash);
					}
					break;
				case divorce:
					{
						Event *event = new Event;
						event->setName ("Divorce");
						if (getDivorceEvent ())
							delete getDivorceEvent ();
						setDivorceEvent (event);
						event->fromGEDCOM (eventGedComEntryStringList,
								   xRefToNoteGEDCOMInfoHash);
					}
					break;
				default:
					break;
				}
			}
			{
				QString tag = list.at (1);
				if (tag == "HUSB")
				{
					if (list.size () != 3)
						continue;

					QString fxRef = list.at (2);
					Individual *individual = xRefToIndividualHash.value (fxRef, NULL);
					if (individual)
						setHusband (individual);
				}
				else if (tag == "WIFE")
				{
					if (list.size () != 3)
						continue;

					QString fxRef = list.at (2);
					Individual *individual = xRefToIndividualHash.value (fxRef, NULL);
					if (individual)
						setWife (individual);
				}
				else if (tag == "CHIL")
				{
					if (list.size () != 3)
						continue;

					QString fxRef = list.at (2);
					Individual *individual = xRefToIndividualHash.value (fxRef, NULL);
					if (individual)
						appendChild (individual);
				}
				else if (tag == "MARR")
					eventMode = marriage;
				else if (tag == "DIV")
					eventMode = divorce;
			}
			break;
		case 2:
		case 3:
			{
				switch (eventMode)
				{
				case marriage:
				case divorce:
					eventGedComEntryStringList << line;
					break;
				default:
					break;
				}
			}
		default:
			break;
		}
	}

	if (_husband)
	{
		if (getMarriageEvent ())
			getMarriageEvent ()->appendChild (_husband);
		if (getDivorceEvent ())
			getDivorceEvent ()->appendChild (_husband);
	}

	if (_wife)
	{
		if (getMarriageEvent ())
			getMarriageEvent ()->appendChild (_wife);
		if (getDivorceEvent ())
			getDivorceEvent ()->appendChild (_wife);
	}
	else
	{
		_wife = new FemaleIndividual;
		_wife->setDummy (true);
		_wife->appendChild (this);
	}

	unblockRecordingOfItemModifications ();
}

QDebug operator << (QDebug debug, Union &aUnion)
{
	debug << "Union Start" << endl;
	debug << dynamic_cast <Idable &> (aUnion);
	debug << "Husband:" << (aUnion._husband ? aUnion._husband->getId () : 0) << endl;
	debug << "Wife:" << (aUnion._wife ? aUnion._wife->getId () : 0) << endl;
	debug << "Marriage Event:" << (aUnion._marriageEvent ? aUnion._marriageEvent->getId() : 0) << endl;
	debug << "Divorce Event:" << (aUnion._divorceEvent ? aUnion._divorceEvent->getId() : 0) << endl;
	debug << dynamic_cast <IdableWithChildren &> (aUnion);
	debug << "Union End" << endl;
	debug << endl;
	return debug;
}

Individual* Union::getSpouse (bool spouseGenderIsFemale) const
{
	if (spouseGenderIsFemale)
		return _wife;
	else
		return _husband;
}
