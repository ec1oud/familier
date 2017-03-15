#include "Individual.h"
#include "Family.h"
#include "Union.h"
#include "SettingsManager.h"
#include <QColor>
#include <QPixmap>
#include <QFont>
#include "Utilities.h"

#define DEFINE(NAME,TITLE)	"<defn title=\"Alternately: " # TITLE "\">" # NAME "</defn>"
#define GETRELATIVESHELPERFUNCTION(R1A,R1B,R2A,R2B,FUNCTION) \
	allok = true; \
	individuals = FUNCTION; \
	if (individuals.size ()) \
	{ \
		ret += DEFINE (R1A and R2A, R1B and R2B) ": "; \
		qSort (individuals.begin (), individuals.end (), \
			(bool (*) (const Individual *, const Individual *)) isYoungerThan); \
		ret += getListOfAnchors (individuals, true, true); \
		ret += ".<br>"; \
	} \
	if (!allok) listMayBeIncomplete = true;


#define UPDATEINHERITABLEVARIABLE(variable,capitalized) \
	_ ## variable = domElement.attribute(# variable, "").trimmed(); \
	unescapeStringFromXML(_ ## variable); \
	if (_ ## variable.length()) \
		_is ## capitalized ## Inherited = false; \
	else \
		_is ## capitalized ## Inherited = true;


DEFINEAUTOITEMMANAGEMENT(Individual)

uint Individual::s_DummyIndividualCount = 0;

Individual::Individual (const IDTYPE id,
			const QDateTime &creationDateTime, const QDateTime &modificationDateTime,
			const unsigned long modificationCount)
				: Familier (id, creationDateTime, modificationDateTime, modificationCount)
{
	init();
}

Individual::Individual (const QDateTime &creationDateTime, const QDateTime &modificationDateTime,
			const unsigned long modificationCount)
				: Familier (++s_MaxIndividualId, creationDateTime, modificationDateTime, modificationCount)
{
	init ();
}

Individual::Individual (const QDomElement &domElement) : Familier (domElement)
{
	_dummy = domElement.attribute("dummy", "0").trimmed().toULong();
	if (_dummy)
		s_DummyIndividualCount ++;
	_dead = domElement.attribute("dead", "0").trimmed().toULong();

	UPDATEINHERITABLEVARIABLE(village,Village)
	UPDATEINHERITABLEVARIABLE(address,Address)

	_imagePath = domElement.attribute("imagePath", "").trimmed();
	unescapeStringFromXML(_imagePath);

	_facebookID = domElement.attribute ("facebookID", "").trimmed ();
	unescapeStringFromXML (_facebookID);

	if (domElement.elementsByTagName("Notes").count())
	{
		_notes = domElement.elementsByTagName("Notes").at(0).toElement().text().trimmed();
		unescapeStringFromXML(_notes);
	}
	else
		_notes = "";

	initLinks();

	_thumbnail = NULL;
	_pixmap = NULL;
}

Individual::Individual (const Individual &individual) : Familier (individual)
{
	_dummy = individual._dummy;
	_dead = individual._dead;
	_adoptedTo = individual._adoptedTo;
	_adoptedFrom = individual._adoptedFrom;
	_parentUnion = individual._parentUnion;
	_birthEvent = individual._birthEvent;
	_deathEvent = individual._deathEvent;
	_family = individual._family;
	_isFamilyInherited = individual._isFamilyInherited;
	_village = individual._village;
	_isVillageInherited = individual._isVillageInherited;
	_address = individual._address;
	_isAddressInherited = individual._isAddressInherited;
	_imagePath = individual._imagePath;
	_facebookID = individual._facebookID;
	_notes = individual._notes;

	_thumbnail = _thumbnail;
	_pixmap = _pixmap;
}

Individual::~Individual ()
{
	REMOVEITEMFROMITEMMANAGEMENT(Individual)

	if (!_copyConstructed)
	{
		if (_dummy)
			s_DummyIndividualCount --;

		if (_pixmap)
			delete _pixmap;
		if (_thumbnail)
			delete _thumbnail;
	}
}

void Individual::setupItemLinks(const QDomElement &domElement)
{
	Familier::setupItemLinks(domElement);

	LOADITEMFROMXMLWITHOUTCREATION(Individual,adoptedTo)
	LOADITEMFROMXMLWITHOUTCREATION(Individual,adoptedFrom)
	LOADITEMFROMXMLWITHOUTCREATION(Event,birthEvent)
	LOADITEMFROMXMLWITHOUTCREATION(Event,deathEvent)
}

/*bool Individual::isValid () const
{
	return Familier::isValid();
}*/

void Individual::updateAdoptedIndividual (Individual *adoptedIndividual)
{
	IDTYPE id = adoptedIndividual->getId ();
	*adoptedIndividual = *this;
	_adoptedTo = adoptedIndividual;
	_pixmap = NULL;
	_thumbnail = NULL;
	adoptedIndividual->_adoptedFrom = this;
	adoptedIndividual->_parentUnion = NULL;
	adoptedIndividual->_family = NULL;
	adoptedIndividual->setId (id);
}

DEFINITIONOFGETITEMWITHID(Individual,Union)

void Individual::init ()
{
	_dummy = false;
	_dead = false;

	_isFamilyInherited = true;
	_isAddressInherited = true;
	_isVillageInherited = true;

	_thumbnail = NULL;
	_pixmap = NULL;

	initLinks();
}

void Individual::initLinks ()
{
	_adoptedTo = 0;
	_adoptedFrom = 0;

	_parentUnion = 0;
	_birthEvent = 0;
	_deathEvent = 0;
	_family = 0;

	INSERTITEMINTOITEMMANAGEMENT(Individual)
}

bool Individual::isDead() const
{
	if (_adoptedTo)
		return _adoptedTo-> isDead();

	return _dead;
}

bool Individual::isFamilyInherited() const
{
	if (_adoptedTo)
		return _adoptedTo->isFamilyInherited();

	return _isFamilyInherited;
}

bool Individual::isVillageInherited() const
{
	if (_adoptedTo)
		return _adoptedTo->isVillageInherited();

	return _isVillageInherited;
}

bool Individual::isAddressInherited() const
{
	if (_adoptedTo)
		return _adoptedTo->isAddressInherited();

	return _isAddressInherited;
}

Individual* Individual::getFather () const
{
	if (!_parentUnion)
		return NULL;

	return getParentUnion ()->getHusband ();
}

Individual* Individual::getMother () const
{
	if (!_parentUnion)
		return NULL;

	return getParentUnion ()->getWife ();
}

QString Individual::getImagePath () const
{
	if (_adoptedTo)
		return _adoptedTo->getImagePath ();

	if (_imagePath.size())
		return SettingsManager::s_DocumentDirectoryPath + "/" + _imagePath;

	if (isFemale())
		return ":images/female_icon.png";
	else
		return ":images/male_icon.png";
}

SETFUNCTIONWITHCONSTCAST (Individual,	AdoptedTo,	Individual *,	adoptedTo)
SETFUNCTIONWITHCONSTCAST (Individual,	AdoptedFrom,	Individual *,	adoptedFrom)

bool Individual::setDummy (const bool dummy)
{
	/* No setting dummy for males and adopted individuals.*/
	if (!isFemale () || _adoptedTo || _adoptedFrom)
		return false;

	if (_dummy == dummy)
		return false;

	_dummy = dummy;
	if (_dummy)
	{
		_name = QString ("Dummy-%1").arg(_id);
		s_DummyIndividualCount ++;
	}
	else
	{
		_name = FIXME;
		s_DummyIndividualCount --;
	}
	updateModificationTime ();
	return true;
}

GETSETMAXIDCPP (Individual)

SETFUNCTIONI (Individual,	Dead,		bool,		dead)

SETFUNCTIONWITHCONSTCAST (Individual,	ParentUnion,	Union *,	parentUnion)
SETFUNCTIONWITHCONSTCASTI (Individual,	BirthEvent,	Event *,	birthEvent)
SETFUNCTIONWITHCONSTCASTI (Individual,	DeathEvent,	Event *,	deathEvent)
SETFUNCTIONWITHCONSTCAST (Individual,	Family,		Family *,	family)

bool Individual::setVillageInherited(const bool villageInherited)
{
	if (_adoptedTo)
		return _adoptedTo->setVillageInherited (villageInherited);

	if (_isVillageInherited == villageInherited)
		return false;

	_isVillageInherited = villageInherited;
	if (villageInherited)
	{
		Individual *ancestor = getFather ();
		while (ancestor)
		{
			_village = ancestor->getVillage();
			if (ancestor->isVillageInherited())
				ancestor = ancestor->getFather();
			else
				break;
		}
	}
	updateModificationTime();
	return true;
}

SETFUNCTIONIQSTRING (Individual,	Village,	village)

bool Individual::setAddressInherited(const bool addressInherited)
{
	if (_adoptedTo)
		return _adoptedTo->setAddressInherited (addressInherited);

	if (_isAddressInherited == addressInherited)
		return false;

	_isAddressInherited = addressInherited;
	if (addressInherited)
	{
		Individual *ancestor = getFather ();
		while (ancestor)
		{
			_village = ancestor->getAddress();
			if (ancestor->isAddressInherited())
				ancestor = ancestor->getFather();
			else
				break;
		}
	}
	updateModificationTime();
	return true;
}

SETFUNCTIONIQSTRING (Individual,	Address,	address)

bool Individual::setImagePath (const QString &imagePath)
{
	if (_adoptedTo)
		return _adoptedTo->setImagePath (imagePath);

	QString timagePath = imagePath.trimmed ();
	if (_imagePath == timagePath)
		return false;

	_imagePath = timagePath;
	if (_thumbnail)
		delete _thumbnail;
	if (_pixmap)
		delete _pixmap;
	_thumbnail = NULL;
	_pixmap = NULL;

	updateModificationTime ();
	return true;
}

SETFUNCTIONIQSTRING (Individual,	FacebookID,	facebookID)
SETFUNCTIONIQSTRING (Individual,	Notes,		notes)

Qt::ItemFlags Individual::flags(const int col, const bool isSpouseItem) const
{
	Qt::ItemFlags itemFlags = Qt::ItemIsEnabled;

	switch (col)
	{
	case e_col_link:
		break;
	case e_col_sex:
		break;
	case e_col_dead:
		itemFlags |= Qt::ItemIsEditable;
		break;
	case e_col_birthDate:
		itemFlags |= Qt::ItemIsEditable;
		break;
	case e_col_deathDate:
		if (isDead())
			itemFlags |= Qt::ItemIsEditable;
		break;
	case e_col_marriageDate:
		if (isFemale() && isSpouseItem)
			itemFlags |= Qt::ItemIsEditable;
		break;
	case e_col_facebookID:
		itemFlags |= Qt::ItemIsEditable;
		break;
	case e_col_dummyOrBase:
		itemFlags |= Qt::ItemIsEditable;
		break;
	case e_col_myGeneration:
		break;
	default:
#ifdef DEBUGMODE
		qDebug() << "Code reaches here:" << __FILE__ << __LINE__;
#endif
		itemFlags = Familier::flags(col, isSpouseItem);
		break;
	}
	return itemFlags;
}

QVariant Individual::data(const int col, const int role, const IDTYPE spouseID)
{
	if (role == Qt::ForegroundRole)
	{
		switch (col)
		{
		case e_col_name:
			if (getAdoptedTo ())
				return QColor ("Gray");

			if (isFemale())
			{
				if (spouseID)
					return QColor ("blue");
				else
					return QColor ("red");
			}
			else
				return QVariant (QColor ("green"));
		case e_col_birthDate: return QVariant (QColor ("#c0ea00"));
		case e_col_deathDate: return QVariant (QColor ("yellow"));
		case e_col_marriageDate: return QVariant (QColor ("orange"));
		case e_col_link: return QVariant (QColor ("white"));
		default:
#ifdef DEBUGMODE
			qDebug() << "Code reaches here:" << __FILE__ << __LINE__;
#endif
			break;
		}
	}
	else if (role == Qt::FontRole)
	{
		QFont font;
		switch (col)
		{
		case e_col_name:
			if (isFemale())
			{
				if (spouseID)
					font.setItalic(true);
			}
			else
			{
				font.setBold(true);
			}
			font.setCapitalization(QFont::SmallCaps);
			break;
		case e_col_birthDate:
			{
				Event *event = getBirthEvent();
				if (event && !event->wasDateParsed())
					font.setItalic(true);
				font.setPixelSize(11);
			}
			break;
		case e_col_deathDate:
			{
				Event *event = getDeathEvent();
				if (event && !event->wasDateParsed())
					font.setItalic(true);
				font.setPixelSize(11);
			}
			break;
		case e_col_marriageDate:
			//font.setBold(true);
			if (isFemale() && spouseID)
			{
				Event *event = getMarriageEvent (spouseID);
				if (event && !event->wasDateParsed())
					font.setItalic(true);
			}
			font.setPixelSize(11);
			break;
		case e_col_link:
			font.setItalic(true);
			font.setPixelSize(11);
			break;
		default:
#ifdef DEBUGMODE
			qDebug() << "Code reaches here:" << __FILE__ << __LINE__;
#endif
			if (isNonDummy ())
				return Familier::data(col, role, spouseID);
			break;
		}
		if (isDummy())
			font.setPixelSize(3);
		return font;
	}
	else if (role == Qt::TextAlignmentRole)
	{
		switch (col)
		{
		case e_col_sex: return QVariant(Qt::AlignHCenter);
		case e_col_dead: return QVariant(Qt::AlignHCenter);
		default:
#ifdef DEBUGMODE
			qDebug() << "Code reaches here:" << __FILE__ << __LINE__;
#endif
			break;
		}
	} /// Qt::TextAlignmentRole

	else if (role == Qt::DecorationRole)
	{
		if (isDummy())
			return QVariant();

		switch (col)
		{
		case e_col_name:
		case e_col_sex:
		case e_col_dead:
			return QPixmap (getDecoration(col, spouseID));
		default:
			break;
		}
	} /// Qt::DecorationRole

	else if (role == Qt::ToolTipRole)
	{
		switch (col)
		{
		case e_col_name:
			if (isDummy())
				return "This is a dummy individual. In most cases "
						"it is female spouse item whose information "
						"is unknown. Please update information for this "
						"individual to see more data here.";
			return getToolTip(false, spouseID ? spouseID : 0);
		case e_col_sex:
			if (isFemale())
				return QVariant(QString("%1 is Female").arg(getName()));
			else
				return QString("%1 is Male").arg(getName());
		case e_col_dead:
			if (isDead())
				return QVariant(QString("%1 is no longer with us").arg(getName()));
			else
				return QVariant(QString("%1 is very much with us").arg(getName()));
		case e_col_birthDate:
			if (getBirthEvent())
			{
				if (isDead())
				{
					if(getDeathEvent())
						return QVariant(QString("Value within brackets is the age of %1 at the time of %2 death.").arg(getName()).arg(genderBias (hisher, false)));
					else
						return QVariant(QString("Value within brackets is the years since birth of %1.").arg(getName()));
				}
				else
					return QVariant(QString("Value within brackets is the current age of %1.").arg(getName()));
			}
			return QVariant();
		case e_col_marriageDate:
			if (isFemale() && spouseID)
			{
				if (getMarriageEvent(spouseID))
				{
					Individual *spouse = Individual::s_IdToIndividualHash.value(spouseID);
					if (isDead())
					{
						if(getDeathEvent())
							return QVariant(QString("Value within brackets is the number of years %1 was married with.%2").arg(getName()).arg(spouse->getName()));
						else
							return QVariant(QString("Value within brackets is the years since marriage of %1 with %2.").arg(getName()).arg(spouse->getName()));
					}
					else
						return QVariant(QString("Value within brackets is the number of years since marriage of %1 with %2.").arg(getName()).arg(spouse->getName()));
				}
			}
			return QVariant();
		case e_col_deathDate:
			if (isDead() && getDeathEvent())
				return QVariant(QString("Value within brackets is the years since death of %1.").arg(getName()));
			return QVariant();
		default:
#ifdef DEBUGMODE
			qDebug() << "Code reaches here:" << __FILE__ << __LINE__;
#endif
			break;
		}
	} /// Qt::ToolTipRole

	else if (role == Qt::WhatsThisRole)
	{
		QString dateWhatsThis = "<br><br>Enter the date as <b>YYYYMMDD</b> (e.g. <i>20100117</i>)."
					"<br>In case of circa dates enter as <b>c. YYYY</b> or <b>c. YYYYMM</b> (e.g. <i>c. 2010</i> or <i>c. 201001</i>)."
					"<br>Any other string will be treated as plain string and will be stored as is (e.g. <i>Somedate</i>).";

		switch (col)
		{
		case e_col_name:
			return QVariant(QString("<b>Full Name</b> of the individual including the family name."
						"<br>Images are used to depict certain things. A description follows:"
						"<br><img src=':images/son_unmarried.png'> depicts a male not having a spouse"
						"<br><img src=':images/son_married.png'> depicts a male having spouses, but not having a child"
						"<br><img src=':images/son_with_children.png'> depicts a male having children"
						"<br><img src=':images/daughter_unmarried.png'> depicts a female not having a spouse, in the role of a daughter"
						"<br><img src=':images/daughter_married.png'> depicts a female having spouses, in the role of a daughter"
						"<br><img src=':images/wife.png'> depicts a female, currently in the role of a wife"
						"<br><br>This field can be edited inline. However, family name can not be edited inline."));
		case e_col_dead:
			return QVariant(QString("Shows if the individual is dead or alive."
						"<br><img src=':images/dead.png'> means the individual is dead."
						"<br><img src=':images/alive.png'> means the individual is alive."
						"<br><br>This field can be edited inline."));
		case e_col_sex:
			return QVariant(QString("<b>Sex</b> of the individual."
						"<br><img src=':images/male.png'> depicts a male."
						"<br><img src=':images/female.png'> depicts a female."
						"<br><br>This field can not be edited inline."
						"<br>Note changing sex of an individual could potentially lead to data loss."));
		case e_col_birthDate:
			return QVariant(QString("<b>Birthday</b> of the individual."
						"<br>For more information about value in brackets see tooltip."
						"<br><br>This field can be edited inline."
						"%1"
						).arg(dateWhatsThis));
		case e_col_marriageDate:
			return QVariant(QString("The date on which the individual was married."
						"<br>For more information about value in brackets see tooltip."
						"<br><br>This field can be edited inline only in the case of a married female currently in the role of a wife."
						"%1"
						).arg(dateWhatsThis));
		case e_col_deathDate:
			return QVariant(QString("The date on which the individual died."
						"<br>For more information about value in brackets see tooltip."
						"<br><br>This field can be edited inline only if the individual is dead."
						"%1"
						).arg(dateWhatsThis));
		case e_col_link:
			return QVariant(QString("Link to another location on this tree where the individual is present. This field only applies to married females. Click on the text to follow the individual's other locations on the tree."
						"<br><br>This field can not be edited inline."));
		default:
#ifdef DEBUGMODE
			qDebug() << "Code reaches here:" << __FILE__ << __LINE__;
#endif
			break;
		}
	} /// Qt::WhatsThisRole

	else if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		switch (col)
		{
		case e_col_name:
			if (role == Qt::DisplayRole)
			{
				if (isDummy())
					return "***";
				else
					return getFullName();
			}
			break;
		case e_col_sex:
			if (role == Qt::EditRole)
				return QVariant (isFemale ());
			return QVariant ();
		case e_col_dead:
			if (role == Qt::EditRole)
				return QVariant (isDead());
			return QVariant ();
		case e_col_birthDate:
			{
				Event *birth = getBirthEvent();
				if (birth)
				{
					if (role == Qt::DisplayRole)
					{
						Event *death = getDeathEvent();
						if (death)
							return birth->toDisplayDate (death);
						Dated tDate(QDate::currentDate());
						return birth->toDisplayDate(tDate);
					}
					else
						return birth->getDated().getDateString();
				}
			}
			return QVariant();
		case e_col_deathDate:
			{
				Event *death = getDeathEvent();
				if (death)
				{
					if (role == Qt::DisplayRole)
					{
						Dated tDate(QDate::currentDate());
						return death->toDisplayDate(tDate);
					}
					else
						return death->getDated().getDateString();
				}
			}
			return QVariant();
		case e_col_marriageDate:
			if (isFemale() && spouseID)
			{
				Event *marriage = getMarriageEvent(spouseID);
				if (marriage)
				{
					if (role == Qt::DisplayRole)
					{
						Event *death = getDeathEvent();
						Event *spouseDeath = getSpouseWithID (spouseID)->getDeathEvent ();
						if (death && !spouseDeath)
							return marriage->toDisplayDate(death);
						else if (!death && spouseDeath)
							return marriage->toDisplayDate (spouseDeath);
						else if (death && spouseDeath)
						{
							if (*death < *spouseDeath)
								return marriage->toDisplayDate (death);
							else
								return marriage->toDisplayDate (spouseDeath);
						}
						Dated tDate(QDate::currentDate());
						return marriage->toDisplayDate(tDate);
					}
					else
						return marriage->getDated().getDateString();
				}
			}
			return QVariant();
		case e_col_link:
			if (isFemale())
			{
				if (spouseID)
				{
					if (Individual *father = getFather())
						return QString("From: %1").arg(father->getFullName());
				}
				else if (hasSpouse())
				{
					QString spouses = getSpouse(0)->getFullName();
					for (int i = 1; i < spouseCount(); i++)
						spouses += ", " + getSpouse(i)->getFullName();
					return QString("To: %1").arg(spouses);
				}
			}
			return QVariant();
		case e_col_facebookID:
			return _facebookID;
		case e_col_dummyOrBase:
			if (role == Qt::DisplayRole)
			{
				if (isDummy ())
					return "Is Dummy";
				else
					return "Not Dummy";
			}
			return isDummy();
		case e_col_myGeneration:
			if ((role == Qt::DisplayRole || role == Qt::EditRole) && getFamily ())
				return getFamily ()->getGenerations() - _generations;
		default:
#ifdef DEBUGMODE
			qDebug() << "Code reaches here:" << __FILE__ << __LINE__;
#endif
			break;
		}
	} /// Qt::DisplayRole || Qt::EditRole

	else if (role == Qt::StatusTipRole)
	{
		switch (col)
		{
		case e_col_sex: return QVariant(QString("Shows the sex"));
		default:
#ifdef DEBUGMODE
			qDebug() << "Code reaches here:" << __FILE__ << __LINE__;
#endif
			break;
		}
	}		/// Qt::StatusTipRole

	return Familier::data(col, role, spouseID);
}

bool Individual::setData(const int col, const QVariant &value, const IDTYPE spouseID)
{
	switch (col)
	{
	case e_col_dead: return setDead(value.toBool());
	case e_col_birthDate:
	{
		QDate date = value.toDate();

		Event *event = getBirthEvent ();
		if (!event)
		{
			event = new Event;
			event->setName ("Birth of " + getName ());
			setBirthEvent (event);
		}

		if (date.isValid())
			return event->setDated(Dated(date));
		else
			return event->setDated(value.toString());
	}
	case e_col_marriageDate:
		if (isFemale() && spouseID)
		{
			QDate date = value.toDate();

			Event *event = getMarriageEvent(spouseID);
			if (!event)
			{
				event = new Event;
				Individual *spouse = s_IdToIndividualHash.value (spouseID);
				event->setName ("Marriage of " + getFullName () + ", and "
						+ spouse->getName ());
				setMarriageEvent (spouseID, event);
			}

			if (date.isValid())
				return event->setDated(Dated(date));
			else
				return event->setDated(value.toString());
		}
	case e_col_deathDate:
	{
		QDate date = value.toDate();

		Event *event = getDeathEvent ();
		if (!event)
		{
			event = new Event;
			event->setName ("Death of " + getName ());
			setDeathEvent (event);
		}

		if (date.isValid())
			return event->setDated(Dated(date));
		else
			return event->setDated(value.toString());
	}
	case e_col_facebookID: return setFacebookID (value.toString ());
	case e_col_dummyOrBase:
		return setDummy(value.toBool());
	default:
#ifdef DEBUGMODE
		qDebug() << "Code reaches here:" << __FILE__ << __LINE__;
#endif
		break;
	}

	return Familier::setData(col, value, spouseID);
}

QString Individual::getChildTypeName () const
{
	return "Marriage";
}

QTextStream& operator << (QTextStream &stream, Individual &individual)
{
	stream << "\t<Individual ";
	stream << dynamic_cast <Idable &> (individual);
	if (individual.isFemale())
		stream << " type=\"Female\"";
	if (individual._dummy)
		stream << " dummy=\"1\"";
	if (individual._dead)
		stream << " dead=\"1\"";
	if (!individual._isVillageInherited)
	{
		QString village = individual._village;
		Idable::escapeStringForXML (village);
		stream << " village=\"" << village << "\"";
	}
	if (!individual._isAddressInherited)
	{
		QString address = individual._address;
		Idable::escapeStringForXML (address);
		stream << " address=\"" << address << "\"";
	}
	if (individual._imagePath.length())
	{
		QString imagePath = individual._imagePath;
		Idable::escapeStringForXML (imagePath);
		stream << " imagePath=\"" << imagePath << "\"";
	}
	if (individual._facebookID.length ())
	{
		QString facebookID = individual._facebookID;
		Idable::escapeStringForXML (facebookID);
		stream << " facebookID=\"" << facebookID << "\"";
	}
	if (individual._adoptedFrom)
		stream << " adoptedFrom=\"" << individual._adoptedFrom->getId() << "\"";
	if (individual._adoptedTo)
		stream << " adoptedTo=\"" << individual._adoptedTo->getId() << "\"";
	if (individual._birthEvent)
		stream << " birthEvent=\"" << individual._birthEvent->getId() << "\"";
	if (individual._deathEvent)
		stream << " deathEvent=\"" << individual._deathEvent->getId() << "\"";
	stream	<< ">" << endl;

	stream << dynamic_cast <Familier &> (individual);
	if (individual._notes.length())
	{
		QString notes = individual._notes;
		Idable::escapeStringForXML(notes);
		stream << "\t\t<Notes>" << endl
				<< notes << endl
				<< "\t\t</Notes>" << endl;
	}
	stream << "\t</Individual>" << endl;
	return stream;
}

QTextStream& Individual::toGEDCOM(QTextStream &stream) const
{
	if (isDummy ())
		return stream;

	stream << "0 @I" << _id << "@ INDI" << endl;

	stream << "1 NAME " << _name;
	if (SettingsManager::s_ExportForceLastName
			|| (getFamily () && getFamily ()->isChild (_id)))
		stream << " /" << getSurName () << "/";
	stream << endl;
	stream << "1 SEX " << (isFemale() ? "F" : "M") << endl;

	QString noteString;
	QTextStream noteStream (&noteString);

	if (_birthEvent)
	{
		stream << "1 BIRT" << endl;
		_birthEvent->toGEDCOM (stream);

		if (_birthEvent->hasDescription ())
			notesToGEDCOM (_birthEvent->getDescription (), noteStream);
	}
	if (_deathEvent)
	{
		stream << "1 DEAT" << endl;
		_deathEvent->toGEDCOM (stream);

		if (_deathEvent->hasDescription ())
			notesToGEDCOM (_deathEvent->getDescription (), noteStream);
	}
	if (_parentUnion)
		stream << "1 FAMC @F" << _parentUnion->getId () << "@" << endl;
	foreach (IdableWithChildren *aUnion, _children)
		stream << "1 FAMS @F" << aUnion->getId() << "@" << endl;
	if (_aliases.length())
		stream << "1 ALIA " << _aliases.at(0) << endl;
	if (_address.length())
		stream << "1 ADDR " << _address << endl;
	stream << "1 CHAN" << endl;
	stream << "2 DATE " << getModificationTime().toString("dd MMM yyyy") << endl;
	stream << "3 TIME " << getModificationTime ().toString("hh:mm:ss") << endl;
	if (_notes.length())
	{
		stream << "1 NOTE @N" << ++ IdableWithChildren::s_commonNotesID << "@" << endl;
		notesToGEDCOM (_notes, noteStream);
	}

	stream << noteString;

	return stream;
}

void Individual::fromGEDCOM (const QStringList &gedcomEntryStringList,
			     const QHash<QString, Union*> &xRefToUnionHash,
			     const QHash <QString, QStringList> &xRefToNoteGEDCOMInfoHash)
{
	blockRecordingOfItemModifications ();

	QStringList eventGedComEntryStringList;
	enum { none, birth, death } eventMode = none;

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
				case birth:
					{
						Event *event = new Event;
						event->setName ("Birth");
						event->appendChild (this);
						if (getBirthEvent ())
							delete getBirthEvent ();
						setBirthEvent (event);
						event->fromGEDCOM (eventGedComEntryStringList,
								   xRefToNoteGEDCOMInfoHash);
					}
					break;
				case death:
					{
						Event *event = new Event;
						event->setName ("Death");
						event->appendChild (this);
						if (getDeathEvent ())
							delete getDeathEvent ();
						setDeathEvent (event);
						event->fromGEDCOM (eventGedComEntryStringList,
								   xRefToNoteGEDCOMInfoHash);
					}
					break;
				default:
					break;
				}
				eventMode = none;
				eventGedComEntryStringList.clear ();
			}
			{
				QString tag = list.at (1);
				if (tag == "SEX")
					continue;
				else if (tag == "ALIA")
				{
					if (list.size () < 3)
						continue;
					_aliases << list.at (2);
				}
				else if (tag == "NAME")
				{
					if (list.size () < 3)
						continue;

					QString name = list.at (2);
					for (int i = 3; i < list.size (); i++)
						name += " " + list.at (i);

					QStringList names = name.split ('/');
					if (names.count () == 1)
						setName (name);
					else
					{
						// firstName includes all except last name
						QString firstName = names.at (0).trimmed ();
						QString lastName = names.at (1).trimmed ();
						setName (firstName);

						if (!lastName.isEmpty ())
						{
							Family *family;
							if (!Family::familyExists (lastName))
							{
								family = new Family;
								family->setName (lastName);
							}
							else
								family = Family::getFamilyWithName (lastName);
							setFamily (family);
							family->appendChild (this);
						}
					}
				}
				else if (tag == "FAMC")
				{
					if (list.size () < 3)
						continue;

					QString fxRef = list.at (2);
					Union *aUnion = xRefToUnionHash.value (fxRef, NULL);
					if (aUnion)
					{
						if (getParentUnion ())
							delete getParentUnion ();
						setParentUnion (aUnion);
					}
				}
				else if (tag == "FAMS")
				{
					if (list.size () < 3)
						continue;

					QString fxRef = list.at (2);
					Union *aUnion = xRefToUnionHash.value (fxRef, NULL);
					if (aUnion)
						appendChild (aUnion);
				}
				else if (tag == "NOTE")
				{
					if (list.size () < 3)
						continue;
					QString fxRef = list.at (2);
					const QStringList &noteGedComList = xRefToNoteGEDCOMInfoHash.value (fxRef);
					_notes = IdableWithChildren::notesFromGEDCOM (noteGedComList);
				}
				else if (tag == "BIRT")
					eventMode = birth;
				else if (tag == "DEAT")
					eventMode = death;
			}
			break;
		case 2:
		case 3:
			{
				switch (eventMode)
				{
				case birth:
				case death:
					eventGedComEntryStringList << line;
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	}

	unblockRecordingOfItemModifications ();
}

QDebug operator << (QDebug debug, Individual &individual)
{
	debug << "Individual Start" << endl;
	debug << dynamic_cast <Idable &> (individual);
	debug << "Type:" << (individual.isFemale() ? "Female" : "Male") << endl;
	debug << "Dummy:" << individual._dummy << endl;
	debug << "Dead:" << individual._dead << endl;
	debug << "Father:" << (individual.getFather () ? individual.getFather ()->getId () : 0) << endl;
	debug << "Mother:" << (individual.getMother () ? individual.getMother ()->getId () : 0) << endl;
	debug << "Adopted From:" << (individual._adoptedFrom ? individual._adoptedFrom->getId() : 0) << endl;
	debug << "Adopted To:" << (individual._adoptedTo ? individual._adoptedTo->getId() : 0) << endl;
	debug << "Birth Event:" << (individual._birthEvent ? individual._birthEvent->getId() : 0) << endl;
	debug << "Death Event:" << (individual._deathEvent ? individual._deathEvent->getId() : 0) << endl;
	debug << "Family:" << (individual._family ? individual._family->getId() : 0) << endl;
	debug << "Village:" << individual._village << endl;
	debug << "Address:" << individual._address << endl;
	debug << "Image Path:" << individual._imagePath << endl;
	debug << dynamic_cast <Familier &> (individual);
	debug << "Notes Start" << endl;
	debug << individual._notes << endl;
	debug << "Noted End" << endl;
	debug << "Individual End" << endl;
	debug << endl;
	return debug;
}

QString Individual::getPetName () const
{
	if (_aliases.count())
		return _aliases.at(0);

	return "";
}

QString Individual::getSurName () const
{
	if (!_family)
		return "";

	return _family->getName ();
}

QString Individual::getFullName () const
{
	return _name + " " + getSurName ();
}

bool Individual::hasImage () const
{
	if (_adoptedTo)
		return _adoptedTo->hasImage ();

	return _imagePath.size() > 0;
}

QPixmap* Individual::getImage ()
{
	if (!_pixmap)
		_pixmap = new QPixmap (getImagePath ());
	return _pixmap;
}

QIcon* Individual::getIcon ()
{
	if (!_thumbnail)
		_thumbnail = new QIcon (getImagePath ());
	return _thumbnail;
}

QString Individual::getListOfAnchors (const QList<Individual*> &individuals,
				      const bool useFullName,
				      const bool createAnchor)
{
	QList <IdableWithChildren*> iids;
	foreach (Individual *individual, individuals)
		iids << dynamic_cast <IdableWithChildren*> (individual);

	return IdableWithChildren::getListOfAnchors (iids, useFullName, createAnchor);
}

QString Individual::getDecoration(const int col, const unsigned long spouseID) const
{
	switch (col)
	{
	case e_col_name:
		if (isFemale())
		{
			if (!spouseID)
			{
				if (hasSpouse())
					return ":images/daughter_married.png";
				else
					return ":images/daughter_unmarried.png";
			}
			else
				return ":images/wife.png";
		}
		else
		{
			if (hasSpouse())
			{
				if (hasOffspring())
					return ":images/son_with_children.png";
				else
					return ":images/son_married.png";
			}
			else
				return ":images/son_unmarried.png";
		}
	case e_col_sex:
		if (isFemale())
			return ":images/female.png";
		else
			return ":images/male.png";
	case e_col_dead:
		if (isDead())
			return ":images/dead.png";
		else
			return ":images/alive.png";
	default:
#ifdef DEBUGMODE
		qDebug() << "Code reaches here:" << __FILE__ << __LINE__;
#endif
		break;
	}

	return "";
}

bool Individual::setPetName(const QString &petName)
{
	QString tpetName = petName.trimmed();
	if (!tpetName.length())
		return false;

	if (_aliases.count() == 0)
		_aliases.append(tpetName);
	else
	{
		if (!_aliases.contains(tpetName))
			_aliases.insert(0, tpetName);
		else
			_aliases.swap(0, _aliases.indexOf(tpetName));
	}

	updateModificationTime();
	return true;
}

bool Individual::hasSpouse () const
{
	return childCount() > 0;
}

bool Individual::isSpouse (const IDTYPE spid) const
{
	return getSpouseWithID (spid) != NULL;
}

long Individual::spouseCount () const
{
	return childCount ();
}

QString Individual::spouseCountAsString () const
{
	return toStringRepresentationOfNumber (spouseCount (), "spouse", "spouses");
}

long Individual::spouseNumber (const IDTYPE spid) const
{
	for (int i = 0; i < spouseCount(); i++)
	{
		Individual *spouse = getSpouse(i);
		if (spouse->getId() == spid)
			return i;
	}

	return -1;
}

Individual* Individual::getSpouse (const long spidx) const
{
	if (spidx < 0 || spidx >= childCount())
		return NULL;

	Union *aUnion = dynamic_cast<Union *> (childAt(spidx));
	return aUnion->getSpouse(!isFemale());
}

Individual* Individual::getSpouseWithID (const IDTYPE spid) const
{
	return getSpouse(spouseNumber (spid));
}

Union* Individual::getUnionWithSpouseID (const IDTYPE spid) const
{
	for (int i = 0; i < childCount(); i++)
	{
		Union *aUnion = dynamic_cast<Union *> (childAt(i));
		if (spid == aUnion->getSpouse(!isFemale())->getId())
			return aUnion;
	}
	return NULL;
}

bool Individual::hasOffspring () const
{
	return offspringCount() > 0;
}

bool Individual::isOffspring (const IDTYPE chid, const IDTYPE spid) const
{
	return getOffspringWithId (chid, spid) != NULL;
}

long Individual::offspringCount (const IDTYPE spid) const
{
	if (spid == 0)
	{
		long count = 0;
		for (int i = 0; i < childCount(); i++)
		{
			Union *aUnion = dynamic_cast<Union *> (childAt(i));
			count += aUnion->childCount();
		}
		return count;
	}
	else
	{
		Union *aUnion = getUnionWithSpouseID (spid);
		if (aUnion)
			return aUnion->childCount();
		else
			return -1;
	}
}

QString Individual::offspringCountAsString (const unsigned long spid) const
{
	return toStringRepresentationOfNumber (offspringCount (spid), "child", "children");
}

long Individual::offspringNumber (const IDTYPE spid, const IDTYPE chid) const
{
	Union *aUnion = getUnionWithSpouseID (spid);
	for (int i = 0; i < aUnion->childCount(); i++)
	{
		Individual *child = dynamic_cast<Individual *> (aUnion->childAt(i));
		if (chid == child->getId())
			return i;
	}

	return -1;
}

Individual* Individual::getOffspring (const long chidx, const IDTYPE spid) const
{
	if (chidx < 0)
		return NULL;

	Union *aUnion = getUnionWithSpouseID (spid);
	if (chidx >= aUnion->childCount())
		return NULL;

	return dynamic_cast<Individual *> (aUnion->childAt(chidx));
}

Individual* Individual::getOffspringWithId (const IDTYPE chid, const IDTYPE spid) const
{
	if (spid)
		return getOffspring (offspringNumber(spid, chid), spid);
	else
	{
		for (int i = 0; i < childCount(); i++)
		{
			Union *aUnion = dynamic_cast<Union *> (childAt(i));
			for (int j = 0; j < aUnion->childCount(); j++)
			{
				Individual *child = dynamic_cast<Individual *> (aUnion->childAt(j));
				if (chid == child->getId())
					return child;
			}
		}
		return NULL;
	}
}

Event* Individual::getMarriageEvent (const IDTYPE spid) const
{
	Union *aUnion = getUnionWithSpouseID (spid);
	if (aUnion)
		return aUnion->getMarriageEvent ();

	return NULL;
}

Event* Individual::getDivorceEvent (const IDTYPE spid) const
{
	Union *aUnion = getUnionWithSpouseID (spid);
	if (aUnion)
		return aUnion->getDivorceEvent ();

	return NULL;
}

Event* Individual::getMarriageEventIndex (const long spidx) const
{
	Individual *spouse = getSpouse (spidx);
	if (!spouse)
		return NULL;

	return getMarriageEvent (spouse->getId ());
}

Event* Individual::getDivorceEventIndex (const long spidx) const
{
	Individual *spouse = getSpouse (spidx);
	if (!spouse)
		return NULL;

	return getDivorceEvent (spouse->getId ());
}

bool Individual::setMarriageEvent(const IDTYPE spid, Event *marriageEvent)
{
	Union *aUnion = getUnionWithSpouseID (spid);
	if (aUnion)
		return aUnion->setMarriageEvent(marriageEvent);

	return false;
}

bool Individual::setDivorceEvent(const IDTYPE spid, Event *divorceEvent)
{
	Union *aUnion = getUnionWithSpouseID (spid);
	if (aUnion)
		return aUnion->setDivorceEvent(divorceEvent);

	return false;
}

bool Individual::hasSibling () const
{
	return siblingCount() > 0;
}

bool Individual::hasElderSibling () const
{
	return _parentUnion && _parentUnion->childNumber (this) != 0;
}

bool Individual::hasYoungerSibling () const
{
	return _parentUnion && (_parentUnion->childNumber (this) < _parentUnion->childCount () - 1);
}

long Individual::siblingCount () const
{
	if (!_parentUnion)
		return 0;

	return _parentUnion->childCount () - 1;
}

QString Individual::siblingCountAsString () const
{
	return toStringRepresentationOfNumber (siblingCount (), "sibling", "siblings");
}

long Individual::lastElderSibling () const
{
	if (!_parentUnion)
		return -1;

	return _parentUnion->childNumber (this)- 1;
}

long Individual::firstYoungerSibling () const
{
	if (!_parentUnion)
		return -1;

	long index = _parentUnion->childNumber (this) + 1;
	if (index >= _parentUnion->childCount ())
		return -1;
	return index;
}

Individual* Individual::getSibling (const long sbidx) const
{
	if (!_parentUnion)
		return NULL;

	if (sbidx < 0 || sbidx >= _parentUnion->childCount () ||
	    sbidx == _parentUnion->childNumber (this))
		return NULL;

	return dynamic_cast<Individual *>(_parentUnion->childAt (sbidx));
}

bool Individual::isOlderThan (const Individual *left, const Individual *right)
{
	try
	{
		return left->isOlderThan (right);
	}
	catch (NoBirthEventException &e)
	{
		if (e.getID () == left->getId ())
			return false;
		if (e.getID () == right->getId ())
			return true;
		return left->getName () > right->getName ();
	}
}

bool Individual::isYoungerThan (const Individual *left, const Individual *right)
{
	try
	{
		return right->isOlderThan (left);
	}
	catch (NoBirthEventException &e)
	{
		if (e.getID () == left->getId ())
			return false;
		if (e.getID () == right->getId ())
			return true;
		return left->getName () < right->getName ();
	}
}

bool Individual::isOlderThan (const Individual *individual) const
{
	Event *e1 = getBirthEvent ();
	Event *e2 = individual->getBirthEvent ();
	if (!e1 && !e2) throw (NoBirthEventException (0));
	if (!e1) throw (NoBirthEventException (getId ()));
	if (!e2) throw (NoBirthEventException (individual->getId ()));

	return *e1 > *e2;
}

QString Individual::deathBias(const EnglishWord ew, const bool capitalise) const
{
	QString temp;
	switch (ew)
	{
	case hashad:
		temp = (_dead ? "had" : "has" );
		break;
	case iswas:
		temp = (_dead ? "was" : "is");
		break;
	default:
		break;
	}

	if (capitalise)
		Idable::toTitleCase(temp);

	return temp;
}

QString Individual::getAnchor (const IDTYPE spouseIdForCurrentEntryOfFemale, bool useFullName,
			       const bool createAnchor)
{
	if (createAnchor)
	{
		QString toolTip = getToolTip (false, spouseIdForCurrentEntryOfFemale);
		toolTip.replace ("<", "&lt;");
		toolTip.replace (">", "&gt;");
		return QString("<a href=\"iid:%1%4\" title=\"%3\">%2</a>")
				.arg(_id).arg(useFullName ? getFullName() : getUnderlinedName ())
				.arg(toolTip)
				.arg(spouseIdForCurrentEntryOfFemale ? QString ("#%1").arg(spouseIdForCurrentEntryOfFemale) : "");
	}

	if (useFullName)
		return "<u>" + getFullName() + "</u>";

	return getUnderlinedName();
}

QString Individual::getListOfAnchors(const int type, const bool useFullName,
				     const bool createAnchor)
{
	switch (AnchorType(type))
	{
	case bornTo:
		if (getFather () && getMother () && _family)
		{
			Individual *father = getFather ();
			Individual *mother = getMother ();
			return QString (" to %1%2 in the %3 family")
					.arg(father->getAnchor(0, useFullName, createAnchor))
					.arg(mother->isDummy() ? ""
						: QString (" and %1")
						.arg(mother->getAnchor(0, useFullName, createAnchor)))
					.arg(_family->getAnchor(0, useFullName, createAnchor));
		}
		break;
	case spouses:
		if (hasSpouse())
		{
			QList <IdableWithChildren *> spouseList;
			int nonDummySpouseCount = 0;
			for (int i = 0; i < spouseCount(); i++)
			{
				Individual *spouse = getSpouse (i);
				if (spouse->isDummy ())
					continue;
				nonDummySpouseCount ++;
				spouseList.append(spouse);
			}
			QString ret;
			ret = getAnchor(0, useFullName, createAnchor) + " ";
			ret += deathBias(hashad, false) + " ";
			ret += spouseCountAsString () + ".";
			if (!nonDummySpouseCount)
				return ret;

			ret += " " + genderBias(hisher, true) + " ";
			ret += pluralBias(spousespouses, spouseCount(), false) + " ";
			if (_dead)
				ret += pluralBias(waswere, spouseCount(), false);
			else
				ret += pluralBias(isare, spouseCount(), false);
			ret += " ";
			ret += IdableWithChildren::getListOfAnchors(spouseList, useFullName,
								    createAnchor);
			ret += ".";
			if (nonDummySpouseCount < spouseCount ())
				ret += " (A few spouses are not known.)";
			return ret;
		}
		break;
	case spousesWithOffspring:
		{
			QString ret = getListOfAnchors(spouses, useFullName, createAnchor);

			for (int i = 0; i < spouseCount(); i++)
			{
				Individual *spouse = getSpouse(i);
				if (offspringCount(spouse->getId()))
				{
					QList <IdableWithChildren *> offspringList;
					for (int i = 0; i < offspringCount(spouse->getId()); i++)
						offspringList.append(getOffspring(i, spouse->getId()));

					ret += " ";
					ret += genderBias(heshe, true) + " ";
					ret += deathBias(iswas, false) + " the ";
					ret += genderBias(fathermother, false) + " of ";
					ret += IdableWithChildren::getListOfAnchors(offspringList, useFullName, createAnchor);
					if (!spouse->isDummy ())
					{
						ret += " with ";
						ret += genderBias(hisher, false) + " spouse ";
						ret += spouse->getAnchor(_id, useFullName, createAnchor);
					}
					ret += ".";
				}
				else
				{
					ret += " ";
					ret += genderBias(heshe, true) + " ";
					ret += deathBias(hashad, false) + " no children with ";
					ret += genderBias(hisher, false) + " spouse ";
					ret += spouse->getAnchor(_id, useFullName, createAnchor) + ".";
				}
			}

			return ret;
		}
		break;
	case siblings:
		if (hasSibling())
		{
			QString ret = genderBias(heshe, true) + " ";
			ret += deathBias(hashad, false) + " ";
			ret += siblingCountAsString () + ".";
			if (hasElderSibling())
			{
				QList <IdableWithChildren *> siblingList;
				for (int i = 0; i <= lastElderSibling(); i++)
					siblingList.append(getSibling(i));
				ret += " ";
				ret += genderBias(heshe, true) + " ";
				ret += deathBias(iswas, false) + " the younger ";
				ret += genderBias(brothersister, false) + " of ";
				ret += IdableWithChildren::getListOfAnchors (siblingList, useFullName, createAnchor);
				ret += ".";
			}
			if (hasYoungerSibling())
			{
				QList <IdableWithChildren *> siblingList;
				for (int i = firstYoungerSibling(); i < siblingCount() + 1; i++)
					siblingList.append(getSibling(i));
				ret += " ";
				ret += genderBias(heshe, true) + " ";
				ret += deathBias(iswas, false) + " the elder ";
				ret += genderBias(brothersister, false) + " of ";
				ret += IdableWithChildren::getListOfAnchors(siblingList, useFullName, createAnchor);
				ret += ".";
			}
			return ret;
		}
		break;
	}
	return "";
}

QString Individual::getToolTip (const bool createAnchor, const IDTYPE spouseIdForCurrentEntryOfFemale)
{
	QString imageRight = "";

	QString bornToString = "";
	bornToString = getListOfAnchors (bornTo, false, createAnchor);

	QString namex = getFullName();
	namex += " <img src='";
	namex += getDecoration(e_col_name, spouseIdForCurrentEntryOfFemale);
	namex += "' title='";
	namex += data (e_col_sex, Qt::ToolTipRole, spouseIdForCurrentEntryOfFemale).toString();
	namex += "'> <img src='";
	namex += getDecoration(e_col_dead, spouseIdForCurrentEntryOfFemale);
	namex += "' title='";
	namex += data (e_col_dead, Qt::ToolTipRole, spouseIdForCurrentEntryOfFemale).toString();
	namex += "'> <img src='";
	namex += getDecoration(e_col_sex, spouseIdForCurrentEntryOfFemale);
	namex += "'>";
	if (_facebookID.size ())
	{
		namex += " <a href='http://facebook.com/";
		namex += data (e_col_facebookID, Qt::DisplayRole, 0).toString ();
		namex += "'><img title='View Facebook Profile for ";
		namex += getFullName ();
		namex += "' src=':images/facebook.png' height='20'/></a>";
	}
	if (createAnchor)
	namex += QString(" <a href='edit:%1' title='Edit this entry'>"
			 "<img src=':images/editentry.png' height='20'></a>").arg(_id);
	QString aliases = "";
	if (_aliases.count())
	{
		aliases += "<abbr title='Also Known As'>aka</abbr> <i>" + _aliases.at(0);
		if (_aliases.count() > 1)
		{
			for (int i = 1; i < _aliases.count() - 1; i++)
				aliases += ", " + _aliases.at(i);
			aliases += ", and " + _aliases.at(_aliases.count() - 1);
		}
		aliases += "</i>";
	}
	QString name = "<h2>" + namex + "</h2>";
	if (_village.length())
		name += " (village <i>" + _village + "</i>";
	name += aliases;

	QString birthDeathInfoString;

	if (_birthEvent || _deathEvent)
	{
		QString place = "";
		if (_birthEvent)
			place = _birthEvent->getPlace().length() ?
				QString(" at <b>%1</b>").arg(_birthEvent->getPlace()) : "";

		int age;

		QDate today = QDate::currentDate();
		if (_dead && _deathEvent)
		{
			int deathAge = Dated(today) - _deathEvent->getDated();
			QString ago = deathAge > 1 ? QString(", <b>%1</b> years ago").arg(deathAge)
				      : (deathAge > 0 ? QString(", <b>one</b> year ago") : "");
			if (_birthEvent)
			{
				age = _deathEvent->getDated() - _birthEvent->getDated();
				if (age >= 0)
				{
					QString young = age < 55 ? " young" : "";
					birthDeathInfoString = QString ("%1 was born%2 on <b>%3</b>%4 and died at the%5 age of %6 on <b>%7</b>%8.")
							       .arg(name)
							       .arg(bornToString)
							       .arg(_birthEvent->toDisplayDate())
							       .arg(place)
							       .arg(young)
							       .arg(age > 1 ? QString ("<b>%1</b> years").arg(age) : "<b>one</b> year")
							       .arg(_deathEvent->toDisplayDate())
							       .arg(ago);
				}
				else
				{
					birthDeathInfoString = QString ("%1 was born%2 on <b>%3</b>%4 and died on <b>%5</b>%6.")
							       .arg(name)
							       .arg(bornToString)
							       .arg(_birthEvent->toDisplayDate())
							       .arg(place)
							       .arg(_deathEvent->toDisplayDate())
							       .arg(ago);
				}
			}
			else
			{
				birthDeathInfoString = QString ("%1 was born%2 and died on <b>%5</b>%6.")
						.arg(name)
						.arg(bornToString)
						.arg(_deathEvent->toDisplayDate())
						.arg(ago);
			}
		}
		else if (_birthEvent)
		{
			age = Dated (today) - _birthEvent->getDated();

			QString ago = _dead ?
				      (age > 1 ? QString(", <b>%1</b> years ago. Sadly though he is no longer with us").arg(age)
				       : (age > 0 ? ", <b>one</b> year ago. Sadly though he is no longer with us" : ""))
				      :
				      (age > 1 ? QString(". %1 is currently <b>%2</b> years old").arg(genderBias(heshe, true)).arg(age)
				       : (age > 0 ? QString(". %1 is currently <b>one</b> year old.").arg(genderBias(heshe, true)) : ""));

			birthDeathInfoString = QString("%1 was born%2 on <b>%3</b>%4%5.")
					       .arg(name)
					       .arg(bornToString)
					       .arg(_birthEvent->toDisplayDate())
					       .arg(place)
					       .arg(ago);
		}
	}
	else if (bornToString.length())
		birthDeathInfoString = name + " was born " + bornToString.replace(",", ".") + ".";
	else
		birthDeathInfoString = name;

	if (hasSibling())
		birthDeathInfoString += " " + getListOfAnchors(siblings, false, createAnchor);

	QString marriageString = "";
	if (hasSpouse())
		marriageString = getListOfAnchors(spousesWithOffspring, false, createAnchor);

	imageRight += "<p>" + birthDeathInfoString + "</p>";
	imageRight += "<p>" + marriageString + "</p>";

	QString ret = "";
	ret += QString ("<a href='image:%1'><img src='%2' width='%3' title='%4' align='left' /></a>"
			"<p>%5</p>")
			.arg(_id).arg(getImagePath ())
			.arg(SettingsManager::s_MaxEditDialogImageWidth)
			.arg((_imagePath.isEmpty() ? "Place for image of " : "Image of ") + getName ())
			.arg(imageRight);

	if (!_statsUpToDate)
		updateStatistics ();

	QString selfLink = getAnchor(spouseIdForCurrentEntryOfFemale, false, createAnchor);

	if (!isFemale ())
		ret += "<p>" + selfLink +  getStatisticsHTML () + "</p>";

	return ret;
}

QString Individual::getItemViewPage (const IDTYPE spouseIdForCurrentEntryOfFemale)
{
	if (isDummy())
		return "This is a dummy individual. We don't know much about this individual."
				" Remove the associated dummy tag to view/edit the information.";

	QString ret = "<table width=\"100%\" border=\"0\">";
	ret += "<tr><td>" + Familier::getItemViewPage(spouseIdForCurrentEntryOfFemale) + "</td></tr>";

	if (SettingsManager::s_DisplayIndianRelations)
	{
		QString relativeDisplayString = getRelativesAsDisplayString ();
		if (relativeDisplayString.size ())
			ret += "<tr><td>" + relativeDisplayString + "</td></tr>";
	}

	QList<Event> eventList;
	foreach (Event *event, Event::s_EventList)
		if (event->hasEventMember(_id))
			eventList.append(*event);

	if (eventList.size())
	{
		qSort (eventList);

		ret += "<tr><td><h3>Event Information</h3>";
		ret += "<ol>";

		foreach (Event event, eventList)
			ret += "<li>" + event.getEventInformation() + "</li>";

		ret += "</ol></td></tr>";
	}

	if (_notes.length())
	{
		QString convertedNotes;
		Utilities::convertFTMLToDisplayFormat(_notes, convertedNotes, false, true, -1, this);
		ret += "<tr><td><hr><h3>Notes</h3>" + convertedNotes + "</td></tr>";
	}

	ret += "</table>";

	return ret;
}

bool Individual::fullTextSearch (const QRegExp &searchRegExp, QStringList &resultsList) const
{
	bool found = Familier::fullTextSearch (searchRegExp, resultsList);

	if (_dummy && searchRegExp.pattern ().toLower ()== "dummy")
	{
		found = true;
		resultsList << "Dummy";
	}

	if (_dead)
	{
		if (searchRegExp.pattern ().toLower () == "dead")
		{
			found = true;
			resultsList << "Dead";
		}
	}
	else
	{
		if (searchRegExp.pattern ().toLower () == "alive")
		{
			found = true;
			resultsList << "Alive";
		}
	}

	if (_family && _family->getName ().contains (searchRegExp))
	{
		found = true;
		resultsList << QString ("Family Name: %1").arg (_family->getName ());
	}

	if (_village.contains (searchRegExp))
	{
		found = true;
		resultsList << QString ("Village: %1").arg (_village);
	}

	if (_address.contains (searchRegExp))
	{
		found = true;
		resultsList << QString ("Address: %1").arg (_address);
	}

	if (_imagePath.contains (searchRegExp))
	{
		found = true;
		resultsList << QString ("Image Path: %1").arg (_imagePath);
	}

	QString expandedHTMLNotes;
	QString expandedTextNotes = Utilities::convertFTMLToDisplayFormat (
				_notes, expandedHTMLNotes, true, false, -1,
				const_cast <Individual *>(this));
	if (expandedTextNotes.contains (searchRegExp))
	{
		found = true;
		resultsList << QString ("Notes:<br>%1<br>--- END ---").arg (expandedTextNotes);
	}

	return found;
}

QList <Individual*> Individual::getElderBrothers () const
{
	QList <Individual *> retList;
	for (int i = 0; i <= lastElderSibling(); i++)
	{
		Individual *sibling = getSibling (i);
		if (sibling->isFemale ())
			continue;
		retList << sibling;
	}
	return retList;
}

QList <Individual*> Individual::getYoungerBrothers () const
{
	QList <Individual *> retList;
	int youngerSiblingIndex = firstYoungerSibling();
	if (youngerSiblingIndex < 0)
		return retList;

	for (int i = youngerSiblingIndex; i < siblingCount() + 1; i++)
	{
		Individual *sibling = getSibling (i);
		if (sibling->isFemale ())
			continue;
		retList << sibling;
	}
	return retList;
}

QList <Individual*> Individual::getBrothers () const
{
	QList <Individual *> retList = getElderBrothers ();
	retList << getYoungerBrothers ();
	return retList;
}

QList <Individual*> Individual::getSisters () const
{
	QList <Individual *> retList;
	foreach (Individual *sibling, getSiblings ())
	{
		if (sibling->isMale ())
			continue;
		retList << sibling;
	}
	return retList;
}

QList <Individual*> Individual::getSiblings () const
{
	QList <Individual *> retList;

	for (int i = 0; i < siblingCount () + 1; i++)
	{
		Individual *sibling = getSibling (i);
		if (!sibling || sibling == this)
			continue;
		retList << sibling;
	}

	return retList;
}

QList <Individual*> Individual::getElderMaleCousins (bool &allok) const
{
	return getOlderOrYoungCousins (true, allok);
}

QList <Individual*> Individual::getYoungerMaleCousins (bool &allok) const
{
	return getOlderOrYoungCousins (false, allok);
}

QList <Individual*> Individual::getMaleCousins (bool &allok) const
{
	QList <Individual *> retList;
	foreach (Individual *cousin, getCousins (allok))
	{
		if (cousin->isFemale ())
			continue;
		retList << cousin;
	}
	return retList;
}

QList <Individual*> Individual::getFemaleCousins (bool &allok) const
{
	QList <Individual *> retList;
	foreach (Individual *cousin, getCousins (allok))
	{
		if (cousin->isMale ())
			continue;
		retList << cousin;
	}
	return retList;
}

QList <Individual*> Individual::getCousins (bool &allok) const
{
	return getChachereCousins (allok) + getFufereCousins (allok)
			+ getMamereCousins (allok) + getMausereCousins (allok);
}

QList <Individual*> Individual::getJijaBhabhis (bool &allok) const
{
	QList <Individual *> retList;
	foreach (Individual *sibling, getSiblings () + getCousins (allok))
		retList << sibling->getSpouses ();
	return retList;
}

QList <Individual*> Individual::getStepParents () const
{
	QList <Individual *> retList;
	Individual *father = getFather ();
	Individual *mother = getMother ();
	if (mother && mother->isNonDummy ())
	{
		for (int i = 0; i < mother->spouseCount (); i++)
		{
			Individual *stepFather = mother->getSpouse (i);
			if (stepFather == father)
				continue;
			retList << stepFather;
		}
	}
	if (father)
	{
		for (int i = 0; i < father->spouseCount (); i++)
		{
			Individual *stepMother = father->getSpouse (i);
			if (stepMother == mother)
				continue;
			retList << stepMother;
		}
	}
	return retList;
}

QList <Individual*> Individual::getHalfSiblngs () const
{
	QList <Individual *> retList;

	Individual *father = getFather ();
	Individual *mother = getMother ();
	IDTYPE fatherID = father ? father->getId () : 0;
	IDTYPE motherID = mother ? mother->getId () : 0;
	QList <Individual *> stepParents = getStepParents ();
	foreach (Individual *stepParent, stepParents)
	{
		IDTYPE spid = stepParent->isFemale () ? fatherID : motherID;
		Union *aUnion = stepParent->getUnionWithSpouseID (spid);
		foreach (IdableWithChildren *child, aUnion->getChildren ())
			retList << dynamic_cast<Individual *> (child);
	}

	return retList;
}

QList <Individual*> Individual::getDadaDadis (bool &allok) const
{
	Individual *father = getFather ();
	if (!father || !father->getFather ())
		return QList <Individual *> ();

	QList <Individual *> retList;
	getGrandFatherMothers (father, retList, allok);
	return retList;
}

QList <Individual*> Individual::getTauTais (bool &allok) const
{
	//Mother's jeth-jethani's may not be the same as tau-tais.
	Individual *father = getFather ();
	if (!father)
		return QList <Individual *> ();

	QList <Individual *> retList;
	foreach (Individual *brotherOrCousin, father->getElderBrothers ()
		 + father->getElderMaleCousins (allok))
		addIndividualAndSpouse (brotherOrCousin, retList);
	return retList;
}

QList <Individual*> Individual::getChachaChachis (bool &allok) const
{
	//Mother's devar-devarani's may not be the same as chacha-chachis.
	Individual *father = getFather ();
	if (!father)
		return QList <Individual *> ();

	QList <Individual *> retList;
	foreach (Individual *brotherOrCousin, father->getYoungerBrothers ()
		 + father->getYoungerMaleCousins (allok))
		addIndividualAndSpouse (brotherOrCousin, retList);
	return retList;
}

QList <Individual*> Individual::getChachereCousins (bool &allok) const
{
	Individual *father = getFather ();
	if (!father || !father->hasSibling ())
		return QList <Individual *> ();

	QList <Individual *> retList;
	QList <Individual *> uncleAuntList = getTauTais (allok) + getChachaChachis (allok);
	foreach (Individual *uncleAunt, uncleAuntList)
		addChildrenForUncle (uncleAunt, retList);
	return retList;
}

QList <Individual*> Individual::getFufaBuas (bool &allok) const
{
	// Mother's nanad-nandois may not be the same as bua-fufas.
	Individual *father = getFather ();
	if (!father)
		return QList <Individual *> ();

	QList <Individual *> retList;
	foreach (Individual *bua, father->getSisters () + father->getFemaleCousins (allok))
		addIndividualAndSpouse (bua, retList);
	return retList;
}

QList <Individual*> Individual::getFufereCousins (bool &allok) const
{
	QList <Individual *> retList;
	QList <Individual *> uncleAuntList = getFufaBuas (allok);
	foreach (Individual *uncleAunt, uncleAuntList)
		addChildrenForAunt (uncleAunt, retList);
	return retList;
}

QList <Individual*> Individual::getBhatijaBhatijis (bool &allok) const
{
	return getSiblingOffsprings (false, allok);
}

QList <Individual*> Individual::getBhanjaBhanjis (bool &allok) const
{
	return getSiblingOffsprings (true, allok);
}

QList <Individual*> Individual::getNanaNanis (bool &allok) const
{
	Individual *mother = getMother ();
	if (!mother || !mother->getMother ())
		return QList <Individual *> ();

	QList <Individual *> retList;
	getGrandFatherMothers (mother, retList, allok);
	return retList;
}

QList <Individual*> Individual::getMamaMamis (bool &allok) const
{
	Individual *mother = getMother ();
	if (!mother)
		return QList <Individual *> ();

	QList <Individual *> retList;
	QList <Individual *> uncleAuntList = mother->getSiblings ()
			+ mother->getFemaleCousins (allok);
	foreach (Individual *uncleAunt, uncleAuntList)
		addIndividualAndSpouseForMale (uncleAunt, retList);
	return retList;
}

QList <Individual*> Individual::getMamereCousins (bool &allok) const
{
	QList <Individual *> retList;
	QList <Individual *> uncleAuntList = getMamaMamis (allok);
	foreach (Individual *uncleAunt, uncleAuntList)
		addChildrenForUncle (uncleAunt, retList);
	return retList;
}

QList <Individual*> Individual::getMausaMausis (bool &allok) const
{
	Individual *mother = getMother ();
	if (!mother || !mother->hasSibling ())
		return QList <Individual *> ();

	QList <Individual *> retList;
	QList <Individual *> uncleAuntList = mother->getSiblings ()
			+ mother->getFemaleCousins (allok);
	foreach (Individual *uncleAunt, uncleAuntList)
		addIndividualAndSpouseForFemale (uncleAunt, retList);
	return retList;
}

QList <Individual*> Individual::getMausereCousins (bool &allok) const
{
	QList <Individual *> retList;
	QList <Individual *> uncleAuntList = getMausaMausis (allok);
	foreach (Individual *uncleAunt, uncleAuntList)
		addChildrenForAunt (uncleAunt, retList);
	return retList;
}

QList <Individual*> Individual::getSpouses () const
{
	QList <Individual *> retList;
	for (int i = 0; i < spouseCount (); i++)
	{
		Individual *spouse = getSpouse (i);
		if (spouse->isDummy ())
			continue;

		retList << spouse;
	}
	return retList;
}

QList <Individual*> Individual::getOffsprings () const
{
	QList <Individual *> retList;
	// We are not using getSpouses() here because it will not
	// return dummy spouses. And so we will miss out on those
	// offsprings.
	for (int i = 0; i < spouseCount (); i++)
	{
		Individual *spouse = getSpouse (i);
		IDTYPE spid = spouse->getId ();
		for (int j = 0; j < offspringCount (spid); j++)
			retList << getOffspring (j, spid);
	}
	return retList;
}

QList <Individual*> Individual::getDamadsAndBahus () const
{
	QList <Individual *> retList;
	foreach (Individual *offspring, getOffsprings ())
		retList << offspring->getSpouses ();
	return retList;
}

QList <Individual*> Individual::getPotaPotis () const
{
	return getGrandChildren (false);
}

QList <Individual*> Individual::getNaatiNaatins () const
{
	return getGrandChildren (true);
}

QString Individual::getRelatives (bool &listMayBeIncomplete) const
{
	bool allok;
	listMayBeIncomplete = false;
	QString ret = "";
	QList <Individual*> individuals; // This is used in the macros below.

	if (getFather ())
		ret += DEFINE (Father, Pitaji) ": " + getFather ()->getAnchor (0, true, true)
				+ "<br>";
	if (getMother () && getMother ()->isNonDummy ())
		ret += DEFINE (Mother, Maa) ": " + getMother ()->getAnchor (0, true, true)
				+ "<br>";

	GETRELATIVESHELPERFUNCTION (Broter, Bhai, Sister, Bahen, getSiblings ());
	GETRELATIVESHELPERFUNCTION (Step Father, Sautela Pita, Step Mother, Sauteli Maa, getStepParents ());
	GETRELATIVESHELPERFUNCTION (Step Brother, Sautela Bhai, Step Sister, Sautela Bhai, getHalfSiblngs ());
	GETRELATIVESHELPERFUNCTION (Dada, Grandfather, Dadi, Grandmother, getDadaDadis (allok));

	GETRELATIVESHELPERFUNCTION (Tau, Uncle, Tai, Aunt, getTauTais (allok));
	GETRELATIVESHELPERFUNCTION (Chacha, Uncle, Chachi, Aunt, getChachaChachis (allok));
	GETRELATIVESHELPERFUNCTION (Chachera Bhai, Cousin, Chacheri Bahen, Cousin, getChachereCousins (allok));
	GETRELATIVESHELPERFUNCTION (Fufa, Uncle, Bua, Aunt, getFufaBuas (allok));
	GETRELATIVESHELPERFUNCTION (Fufera Bhai, Cousin, Fuferi Bahen, Cousin, getFufereCousins (allok));

	GETRELATIVESHELPERFUNCTION (Nana, Grandfather, Nani, Grandmother, getNanaNanis (allok));

	GETRELATIVESHELPERFUNCTION (Mama, Uncle, Mami, Aunt, getMamaMamis (allok));
	GETRELATIVESHELPERFUNCTION (Mamera Bhai, Cousin, Mameri Bahen, Cousin, getMamereCousins (allok));

	GETRELATIVESHELPERFUNCTION (Mausa, Uncle, Mausi, Aunt, getMausaMausis (allok));
	GETRELATIVESHELPERFUNCTION (Mausera Bhai, Cousin, Mauseri Bahen, Cousin, getMausereCousins (allok));

	GETRELATIVESHELPERFUNCTION (Jija, Brother-in-law, Bhabhi, Sister-in-law, getJijaBhabhis (allok));
	GETRELATIVESHELPERFUNCTION (Bhatija, Nephew, Bhatiji, Niece, getBhatijaBhatijis (allok));
	GETRELATIVESHELPERFUNCTION (Bhanja, Nephew, Bhanji, Niece, getBhanjaBhanjis (allok));

	GETRELATIVESHELPERFUNCTION (Husband, Pati, Wife, Patni, getSpouses ());
	GETRELATIVESHELPERFUNCTION (Son, Beta, Daughter, Beti, getOffsprings ());
	GETRELATIVESHELPERFUNCTION (Son-in-law, Damad, Daughter-in-law, Bahu, getDamadsAndBahus ());
	GETRELATIVESHELPERFUNCTION (Pota, Grand-son, Poti, Grand-daughter, getPotaPotis ());
	GETRELATIVESHELPERFUNCTION (Naati, Grand-son, Naatin, Grand-daughter, getNaatiNaatins ());

	GETRELATIVESHELPERFUNCTION (Sala, Sister-in-law, Salhaj, Sister-in-law, getSalaSalhajs (allok));
	GETRELATIVESHELPERFUNCTION (Saadhu, Brother-in-law, Sali, Sister-in-law, getSaadhuSalis (allok));

	GETRELATIVESHELPERFUNCTION (Jeth, Brother-in-law, Jethani, Sister-in-law, getJethJethanis (allok));
	GETRELATIVESHELPERFUNCTION (Devar, Brother-in-law, Devarani, Sister-in-law, getDevarDevarani (allok));
	GETRELATIVESHELPERFUNCTION (Nanadoi, Brother-in-law, Nanad, Sister-in-law, getNanadNandoi (allok));

	return ret;
}

void Individual::getGrandFatherMothers (const Individual *fatherOrMother,
					QList<Individual *> &retList, bool &allok) const
{
	Individual *grandFather = fatherOrMother->getFather ();
	if (grandFather)
		addIndividualAndSpouse (grandFather, retList);
	retList << fatherOrMother->getTauTais (allok);
	retList << fatherOrMother->getChachaChachis (allok);
	retList << fatherOrMother->getFufaBuas (allok);
}

QList <Individual*> Individual::getSiblingOffsprings (bool female, bool &allok) const
{
	QList <Individual *> retList;
	foreach (Individual *sibling, getSiblings () + getCousins (allok))
		addChildrenForUncleOrAunt (sibling, retList, !female);
	return retList;
}

QList <Individual*> Individual::getGrandChildren (bool female) const
{
	QList <Individual *> retList;
	foreach (Individual *offspring, getOffsprings ())
	{
		if (offspring->isMale ()== female)
			continue;

		retList << offspring->getOffsprings ();
	}
	return retList;
}

QList <Individual*> Individual::getOlderOrYoungCousins (bool older, bool &allok) const
{
	QList <Individual *> retList;
	allok = true;
	foreach (Individual *cousin, getMaleCousins (allok))
	{
		try
		{
			if (isOlderThan (cousin) == older)
				retList << cousin;
		}
		catch (NoBirthEventException noBirthEventException)
		{
			// Adding to Devar list in case birth date is missing.
			if (!older)
				retList << cousin;
			allok = false;
		}
	}
	return retList;

}

QString Individual::getRelativesAsDisplayString () const
{
	bool listMayBeIncomplete = true;
	QString ret = getRelatives (listMayBeIncomplete);

	if (listMayBeIncomplete)
		ret += "The list above might be inaccurate to some extent, since some of the cousins have missing birth dates.<br>";
	if (ret.size ())
		ret = "<h3>List of Relatives</h3>" + ret;

	return ret;
}

void Individual::getRelativesHelperFunction (const QString maleRelation,
					     const QString femaleRelation,
					     const QList <Individual *> relatives,
					     QList<QPair<QString, Individual *> > &relationList) const
{
	foreach (Individual *individual, relatives)
	{
		if (individual->isMale ())
			relationList << QPair<QString, Individual*> (maleRelation, individual);
		else
			relationList << QPair<QString, Individual*> (femaleRelation, individual);
	}
}

void Individual::addIndividualAndSpouse (const Individual *individual,
					 QList<Individual *> &retList) const
{
	retList << const_cast<Individual*>(individual);
	for (int i = 0; i < individual->spouseCount (); i++)
	{
		Individual *spouse = individual->getSpouse (i);
		if (spouse->isDummy ())
			continue;
		retList << spouse;
	}
}

void Individual::addIndividualAndSpouseForMale (const Individual *individual,
						QList <Individual *> &retList) const
{
	if (individual->isFemale ())
		return;

	addIndividualAndSpouse (individual, retList);
}

void Individual::addIndividualAndSpouseForFemale (const Individual *individual,
						  QList<Individual *> &retList) const
{
	if (individual->isMale ())
		return;

	addIndividualAndSpouse (individual, retList);
}

void Individual::addChildrenForUncleOrAunt (const Individual *uncleAunt,
					    QList <Individual *> &retList,
					    bool forUncle) const
{
	if (uncleAunt->isFemale () == forUncle)
		return;

	retList << uncleAunt->getOffsprings ();
}

void Individual::addChildrenForUncle (const Individual *uncleAunt,
				      QList<Individual *> &retList) const
{
	addChildrenForUncleOrAunt (uncleAunt, retList, true);
}

void Individual::addChildrenForAunt (const Individual *uncleAunt,
				     QList<Individual *> &retList) const
{
	addChildrenForUncleOrAunt (uncleAunt, retList, false);
}
