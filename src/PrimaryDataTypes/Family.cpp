#include "Family.h"
#include "Individual.h"
#include "TreeModel.h"
#include <QColor>
#include <QFont>

DEFINEAUTOITEMMANAGEMENT(Family)

QMap<QString, Family *> Family::s_NameToBaseFamilyMap;
QMap<QString, Family *> Family::s_NameToNonBaseFamilyMap;

bool Family::familyExists (const QString &familyName)
{
	return getFamilyWithName (familyName) != NULL;
}

Family* Family::getFamilyWithName (const QString &familyName)
{
	foreach (Family *family, s_FamilyList)
		if (family->getName () == familyName)
			return family;
	return NULL;
}

Family::Family (QDateTime creationDateTime, QDateTime modificationDateTime,
		const unsigned long modificationCount)
			: Familier (++s_MaxFamilyId, creationDateTime, modificationDateTime, modificationCount)
{
	init();
}

Family::Family (const IDTYPE id, QDateTime creationDateTime, QDateTime modificationDateTime,
		const unsigned long modificationCount)
			: Familier (id, creationDateTime, modificationDateTime, modificationCount)
{
	init ();
}

Family::Family (const QDomElement &domElement) : Familier (domElement)
{
	_base = domElement.attribute ("base", "0").trimmed().toULong();

	initLinks();
}

Family::Family(const Family &family) : Familier (family)
{
	_base = family._base;
}

Family::~Family ()
{
	REMOVEITEMFROMITEMMANAGEMENT(Family)

	if (_base)
		Family::s_NameToBaseFamilyMap.remove(getUniqueName());
	else
		Family::s_NameToNonBaseFamilyMap.remove(getUniqueName());
}

void Family::setupItemLinks(const QDomElement &domElement)
{
	blockRecordingOfItemModifications();

	Familier::setupItemLinks (domElement);

	foreach (IdableWithChildren *idableWithChildren, _children)
	{
		Individual *individual = dynamic_cast<Individual*>(idableWithChildren);
		individual->blockRecordingOfItemModifications ();
		individual->setFamily (this);
		individual->setParentUnion (NULL);
		individual->unblockRecordingOfItemModifications ();
	}

	unblockRecordingOfItemModifications ();
}

/*bool Family::isValid () const
{
	return Familier::isValid();
}*/

void Family::init()
{
	_base = false;
	initLinks();
}

void Family::initLinks()
{
	INSERTITEMINTOITEMMANAGEMENT(Family)

	initAsPerBase ();
}

void Family::initAsPerBase()
{
	if (_base)
	{
		if (_id)
			s_NameToBaseFamilyMap[getUniqueName()] = this;
		s_NameToNonBaseFamilyMap.remove(getUniqueName());
	}
	else
	{
		s_NameToBaseFamilyMap.remove(getUniqueName());
		if (_id)
			s_NameToNonBaseFamilyMap[getUniqueName()] = this;
	}
}

bool Family::setName (const QString &name)
{
	QString tname = name.trimmed();
	if (!tname.length() || _name == tname)
		return false;

	foreach (Family *family, s_IdToFamilyHash)
		if (family->getName () == tname)
			return false;

	s_NameToBaseFamilyMap.remove(getUniqueName());
	s_NameToNonBaseFamilyMap.remove(getUniqueName());
	_name = tname;
	initAsPerBase();
	updateModificationTime();
	return true;
}

bool Family::appendChild (const IdableWithChildren *child)
{
	if (IdableWithChildren::appendChild(child))
	{
		unsetStatsUpToDate();
		return true;
	}

	return false;
}

bool Family::removeChild (const IdableWithChildren *child)
{
	if (IdableWithChildren::removeChild(child))
	{
		unsetStatsUpToDate();
		return true;
	}

	return false;
}

bool Family::removeChild (const IDTYPE chid)
{
	if (IdableWithChildren::removeChild(chid))
	{
		unsetStatsUpToDate();
		return true;
	}

	return false;
}

bool Family::insertChild (const long chidx, const IdableWithChildren *child)
{
	if (IdableWithChildren::insertChild(chidx, child))
	{
		unsetStatsUpToDate();
		return true;
	}

	return false;
}

bool Family::removeChildAt (const long chidx)
{
	if (IdableWithChildren::removeChildAt(chidx))
	{
		unsetStatsUpToDate();
		return true;
	}

	return false;
}

IdableWithChildren* Family::takeChildAt (const long chidx)
{
	unsetStatsUpToDate();
	return IdableWithChildren::takeChildAt(chidx);
}

DEFINITIONOFGETITEMWITHID(Family,Individual)

void Family::setupModel (TreeModel *model, int row, QModelIndex &parent, QProgressDialog &progress)
{
	blockRecordingOfItemModifications();

	model->insertRow(const_cast<Family *>(this), true, row, parent);

	QModelIndex index = model->index(row, 0, parent);
	for (int crow = 0; crow < _children.count (); crow++)
	{
		Individual *individual = dynamic_cast<Individual *>(_children.value (crow));
		individual->setFamily (this);
		individual->setupModel(model, crow, index, progress);
	}

	unblockRecordingOfItemModifications();

	Familier::setupModel(model, row, parent, progress);
}

GETSETMAXIDCPP (Family)

bool Family::setBase (const bool base)
{
	if (_base == base)
		return false;

	_base = base;
	initAsPerBase();
	updateModificationTime();
	return true;
}

Qt::ItemFlags Family::flags(const int col, const bool isSpouseItem) const
{
	Qt::ItemFlags itemFlags = Qt::ItemIsEnabled;
	switch (col)
	{
	case e_col_dummyOrBase:
		itemFlags |= Qt::ItemIsEditable;
		break;
	default:
		itemFlags = Familier::flags(col, isSpouseItem);
		break;
	}

	return itemFlags;
}

QVariant Family::data(const int col, const int role, const IDTYPE spouseID)
{
	Q_UNUSED(spouseID)

	if (col == e_col_name)
	{
		if (role == Qt::DecorationRole)
		{
			return QPixmap (":images/family.png");
		}
		else if (role == Qt::FontRole)
		{
			QFont font;
			font.setBold(true);
			font.setCapitalization(QFont::AllUppercase);
			font.setPointSize(20);
			return QVariant(font);
		}
		else if (role == Qt::ForegroundRole)
		{
			return QVariant (QColor ("Yellow"));
		}
		else if (role == Qt::WhatsThisRole)
		{
			return QString("Family name. Sorted base families are shown first.<br><br>This field can be edited inline.");
		}
		else if (role == Qt::ToolTipRole)
		{
			return getToolTip(false, 0);
		}
	}
	else if (col == e_col_dummyOrBase)
	{
		if (role == Qt::DisplayRole)
		{
			if (_base)
				return "Is Base";
			else
				return "Not Base";
		}
		else if (role == Qt::EditRole)
			return _base;
	}
	return Familier::data(col, role, spouseID);
}

bool Family::setData(const int col, const QVariant &value, const IDTYPE spouseID)
{
	switch (col)
	{
	case e_col_dummyOrBase: return setBase(value.toBool());
	default:
		break;
	}

	return Familier::setData(col, value, spouseID);
}

QString Family::getChildTypeName () const
{
	return "Member";
}

QTextStream& operator << (QTextStream &stream, Family &family)
{
	stream << "\t<Family ";
	stream << dynamic_cast <Idable &> (family);
	if (family._base)
		stream << " base=\"" << family._base << "\"";
	stream << ">" << endl;
	stream << dynamic_cast <Familier &> (family);
	stream << "\t</Family>" << endl;
	return stream;
}

QDebug operator << (QDebug debug, Family &family)
{
	debug.nospace() << "Family Start" << endl;
	debug << dynamic_cast <Idable &> (family);
	debug.nospace() << "Base:" << family._base << endl;
	debug << dynamic_cast <Familier &> (family);
	debug.nospace() << "Family End" << endl << endl;
	return debug.space();
}

void Family::updateStatistics ()
{
	_statsUpToDate = true;
	_sonCount = 0;
	_daughterCount = 0;
	_marriedDaughterCount = 0;
	_spouseCount = 0;
	_deadCount = 0;
	_generations = 0;

	for (int i = 0; i < _children.count(); i++)
	{
		Individual *primary = dynamic_cast<Individual *>(_children[i]);

		if (primary)
		{
			unsigned long sonCount;
			unsigned long daughterCount;
			unsigned long marriedDaughterCount;
			unsigned long spouseCount;
			unsigned long deadCount;
			unsigned long generations;

			primary->getStatistics (sonCount, daughterCount, marriedDaughterCount,
							spouseCount, deadCount, generations);
			_sonCount += sonCount;
			_daughterCount += daughterCount;
			_marriedDaughterCount += marriedDaughterCount;
			_spouseCount += spouseCount;
			_deadCount += deadCount;
			if (_generations < generations)
				_generations = generations;
		}
	}
	_totalCount = _sonCount + _daughterCount - _marriedDaughterCount + _spouseCount;
}

QString Family::getAnchor (const IDTYPE spouseIdForCurrentEntryOfFemale, bool useFullName,
			   const bool createAnchor)
{
	Q_UNUSED (spouseIdForCurrentEntryOfFemale);
	Q_UNUSED (useFullName);

	if (createAnchor)
		return QString("<a href=\"fid:%1\" title=\"%2\">%3</a>")
			.arg(getId()).arg(getToolTip(false, 0)).arg(getName ());

	return getUnderlinedName();
}

QString Family::getListOfAnchors (const int type, const bool useFullName, const bool createAnchor)
{
	switch (AnchorType(type))
	{
	case primary_members:
		return QString ("The primary %1 of %2 family %3 %4.")
				.arg(pluralBias(membermembers, childCount(), false))
				.arg(getAnchor (0, useFullName, createAnchor))
				.arg(pluralBias(isare, childCount(), false))
				.arg(IdableWithChildren::getListOfAnchors(_children, useFullName,
									  createAnchor));
		break;
	default:
		return "";
	}
}

QString Family::getToolTip (const bool createAnchor, const IDTYPE spouseIdForCurrentEntryOfFemale)
{
	Q_UNUSED (spouseIdForCurrentEntryOfFemale);

	if (!_statsUpToDate)
		updateStatistics ();

	QString selfLink = getAnchor(0, false, createAnchor);
	QString ret;
	ret += "<p>The " + selfLink + " family" + getStatisticsHTML () + "</p>";

	ret += getListOfAnchors(primary_members, false, createAnchor);

	return ret;
}

QString Family::getItemViewPage (const IDTYPE spouseIdForCurrentEntryOfFemale)
{
	return Familier::getItemViewPage(spouseIdForCurrentEntryOfFemale);
}

bool Family::fullTextSearch (const QRegExp &searchRegExp, QStringList &resultsList) const
{
	bool found = Familier::fullTextSearch (searchRegExp, resultsList);

	if (_base && searchRegExp.pattern ().toLower () == "base")
	{
		found = true;
		resultsList << "Base";
	}

	return found;
}
