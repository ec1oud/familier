#include "IdableWithChildren.h"
#include <QStringList>

int IdableWithChildren::s_commonNotesID = 0;

IdableWithChildren::IdableWithChildren (const IDTYPE id,
					const QDateTime &creationDateTime, const QDateTime &modificationDateTime,
					const unsigned long modificationCount)
		: Idable (id, creationDateTime, modificationDateTime, modificationCount)
{
	_copyConstructed = false;
}

IdableWithChildren::IdableWithChildren (const QDomElement &domElement)
	: Idable (domElement)
{
	_copyConstructed = false;
}

IdableWithChildren::IdableWithChildren (const IdableWithChildren &idableWithChildren)
	: Idable (idableWithChildren)
{
	_copyConstructed = true;
	_children = idableWithChildren._children;
}

IdableWithChildren::~IdableWithChildren ()
{
}

void IdableWithChildren::setupItemLinks(const QDomElement &domElement)
{
	QDomNodeList childNodeList = domElement.elementsByTagName(getChildTypeName());
	for (int i = 0; i < childNodeList.count(); i++)
	{
		QDomNode childNode = childNodeList.at(i);
		IDTYPE id = childNode.toElement().text().trimmed().toULong();
		IdableWithChildren *child = getItemWithId (id);
		_children.append(child);
	}
}

/*bool IdableWithChildren::isValid () const
{
	return Idable::isValid();
}*/

bool IdableWithChildren::appendChild (const IdableWithChildren *child)
{
	if (isChild (child->getId()))
		return false;

	_children.append (const_cast<IdableWithChildren *> (child));
	updateModificationTime ();
	return true;
}

bool IdableWithChildren::removeChild (const IdableWithChildren *child)
{
	bool ret = _children.removeOne (const_cast<IdableWithChildren *> (child));
	if (ret)
		updateModificationTime ();
	return ret;
}

bool IdableWithChildren::removeChild (const IDTYPE chid)
{
	long chidx = indexOfChildFromId (chid);
	if (chidx < 0)
		return false;

	return removeChildAt (chidx);
}

bool IdableWithChildren::removeChildAt (const long chidx)
{
	if (chidx < 0 || chidx >= _children.size())
		return false;

	_children.removeAt (chidx);
	updateModificationTime ();
	return true;
}

IdableWithChildren* IdableWithChildren::childAt (const long chidx) const
{
	if (chidx < 0 || chidx >= _children.size())
		return NULL;

	return _children.at (chidx);
}

IdableWithChildren* IdableWithChildren::takeChildAt (const long chidx)
{
	if (chidx < 0 || chidx >= _children.size())
		return NULL;

	updateModificationTime ();
	return _children.takeAt (chidx);
}

bool IdableWithChildren::insertChild (const long chidx, const IdableWithChildren *child)
{
	if (chidx < 0 || chidx > _children.size() || isChild (child->getId()))
		return false;

	_children.insert (chidx, const_cast<IdableWithChildren *> (child));
	updateModificationTime ();
	return true;
}

bool IdableWithChildren::swapChildren (const long chidx1, const long chidx2)
{
	if (chidx1 == chidx2 || chidx1 < 0 || chidx2 < 0 || chidx1 >= _children.size () || chidx2 >= _children.size ())
		return false;

	_children.swap (chidx1, chidx2);
	updateModificationTime ();
	return true;
}

bool IdableWithChildren::moveChildUp (const long index)
{
	return swapChildren (index - 1, index);
}

bool IdableWithChildren::moveChildDown (const long index)
{
	return swapChildren (index, index + 1);
}

QList<IdableWithChildren *> IdableWithChildren::getChildren () const
{
	return _children;
}

void IdableWithChildren::clearChildren ()
{
	_children.clear();
}

long IdableWithChildren::childNumber (const IdableWithChildren *child) const
{
	return _children.indexOf (const_cast<IdableWithChildren *>(child));
}

IDTYPE IdableWithChildren::idOfChildFromIndex (const long index) const
{
	if (index < 0 && index >= _children.size ())
		return 0;

	return _children.at(index)->getId();
}

long IdableWithChildren::indexOfChildFromId (const IDTYPE chid) const
{
	for (int i = 0; i < _children.count(); i++)
		if (chid == _children.at(i)->getId())
			return i;
	return -1;
}

bool IdableWithChildren::isChild (const IDTYPE chid) const
{
	return indexOfChildFromId (chid) != -1;
}

QTextStream& operator << (QTextStream &stream, IdableWithChildren &idableWithChildren)
{
	stream << dynamic_cast <Timed &> (idableWithChildren);

	// Idable with children won't call superclass operator <<

	foreach (IdableWithChildren *child, idableWithChildren._children)
		stream << "\t\t<" << idableWithChildren.getChildTypeName() << ">"
			<< child->getId()
			<< "</" << idableWithChildren.getChildTypeName() << ">"
			<< endl;
	return stream;
}

QDebug operator << (QDebug debug, IdableWithChildren &idableWithChildren)
{
	foreach (IdableWithChildren *child, idableWithChildren._children)
		debug << idableWithChildren.getChildTypeName() << ":" << child->getId() << endl;
	return debug;
}

QString IdableWithChildren::getListOfAnchors (const QList <IdableWithChildren *> &iids,
					      const bool useFullName,
					      const bool createAnchor)
{
	QString ret = "";

	if (!iids.size())
		return ret;

	ret += iids.at(0)->getAnchor(0, useFullName, createAnchor);
	if (iids.size () > 1)
	{
		for (int i = 1; i < iids.size() - 1; i++)
			ret += ", " + iids.at(i)->getAnchor(0, useFullName, createAnchor);

		ret += ", and " + iids.at(iids.size()-1)->getAnchor(0, useFullName, createAnchor);
	}

	return ret;
}

QString IdableWithChildren::toStringRepresentationOfNumber (const int number,
							    const QString singular,
							    const QString plural) const
{
	QString ret = "";

	switch (number)
	{
	case 0: return "no " + singular;
	case 1: return "one " + singular;
	case 2: ret += "two"; break;
	case 3: ret += "three"; break;
	case 4: ret += "four"; break;
	case 5: ret += "five"; break;
	case 6: ret += "six"; break;
	case 7: ret += "seven"; break;
	case 8: ret += "eight"; break;
	case 9: ret += "nine"; break;
	case 10: ret += "ten"; break;
	case 11: ret += "eleven"; break;
	case 12: ret += "twelve"; break;
	case 13: ret += "thirteen"; break;
	case 14: ret += "fourteen"; break;
	case 15: ret += "fifteen"; break;
	default: ret += QString ("%1").arg (number);
	}

	return ret + " " + plural;
}

QTextStream& IdableWithChildren::notesToGEDCOM (const QString &notes, QTextStream &stream) const
{
	stream << "0 @N" << s_commonNotesID << "@ NOTE" << endl;

	int start = 0;
	bool firstTime = true;
	while (start < notes.length ())
	{
		QString notePart = notes.mid (start, 160);
		start += 160;

		QStringList notesList = notePart.split("\n");
		for (int i = 0; i < notesList.size(); i++)
		{
			if (!firstTime)
				stream << "1 CONT" << endl;
			stream << "1 CONC " << notesList.at(i) << endl;
			firstTime = false;
		}
		firstTime = false;
	}

	return stream;
}

QString IdableWithChildren::notesFromGEDCOM (const QStringList &noteGedComList)
{
	QString ret = "";

	bool continued = false;
	bool first = true;
	foreach (QString line, noteGedComList)
	{
		QList<QString> list = line.split (QRegExp ("\\s+"));
		if (list.count () <= 1)
			continue;

		QString tag = list.at (1);
		if (tag == "CONC")
		{
			if (first || continued)
			{
				if (list.count () < 3)
					continue;

				QString noteLine = list.at (2);
				for (int i = 3; i < list.length (); i++)
					noteLine += " " + list.at (i);
				ret += noteLine;
				first = false;
				continued = false;
			}
		}
		else if (tag == "CONT")
			continued = true;
	}

	return ret;
}

QString IdableWithChildren::getAnchor (const IDTYPE spouseIdForCurrentEntryOfFemale,
				       const bool useFullName, const bool createAnchor)
{
	Q_UNUSED (spouseIdForCurrentEntryOfFemale)
	Q_UNUSED (useFullName)
	Q_UNUSED (createAnchor)

	return "";
}

QString IdableWithChildren::getListOfAnchors(const int type, const bool useFullName,
					     const bool createAnchor)
{
	Q_UNUSED (type)
	Q_UNUSED (useFullName)
	Q_UNUSED (createAnchor)

	return "";
}

bool IdableWithChildren::fullTextSearch (const QRegExp &searchRegExp,
					 QStringList &resultsList) const
{
	bool found = Idable::fullTextSearch (searchRegExp, resultsList);
	return found;
}
