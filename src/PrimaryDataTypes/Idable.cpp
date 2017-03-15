#include "Idable.h"

Idable::Idable (const IDTYPE id,
		const QDateTime &creationDateTime, const QDateTime &modificationDateTime,
		const unsigned long modificationCount)
		:Timed (creationDateTime, modificationDateTime, modificationCount),
		_id (id), _name (QString("%1 (%2)").arg (FIXME).arg (id))
{
}

Idable::Idable (const QDomElement &domElement) : Timed (domElement)
{
	_id = domElement.attribute("id", "0").trimmed().toULong();
	_name = domElement.attribute("name", "");
	unescapeStringFromXML(_name);
}

Idable::Idable (const Idable &idable) : Timed (idable)
{
	_id = idable._id;
	_name = idable._name;
}

Idable::~Idable ()
{
}

/*bool Idable::isValid() const
{
	return Timed::isValid() && _id && _name.length();
}*/

bool Idable::setId(const unsigned long id)
{
	if (id == 0 || _id == id)
		return false;

	_id = id;
	updateModificationTime();
	return true;
}

bool Idable::setName(const QString &name)
{
	QString tname = name.trimmed();
	if (!tname.length() || _name == tname)
		return false;

	_name = tname;
	updateModificationTime();
	return true;
}

QTextStream& operator << (QTextStream &stream, Idable &idable)
{
	stream << "id=\"" << idable._id << "\"";
	if (idable._name.length())
	{
		QString name = idable._name;
		Idable::escapeStringForXML (name);
		stream << " name=\"" << name << "\"";
	}

	return stream;
}

QDebug operator <<(QDebug debug, Idable &idable)
{
	debug << "ID:" << idable._id << endl;
	debug << "Name:" << idable._name << endl;

	debug << dynamic_cast <Timed &> (idable);

	return debug;
}

bool Idable::operator < (const Idable &idable) const
{
	return _name < idable._name;
}

bool Idable::operator > (const Idable &idable) const
{
	return _name > idable._name;
}

QString Idable::getUnderlinedName () const
{
	return QString ("<u>%1</u>").arg(_name);
}

QString Idable::getUniqueName () const
{
	return QString ("%1 [%2]").arg(_name).arg(_id);
}

void Idable::toTitleCase (QString &after)
{
	QChar firstLetter = after.at(0).toTitleCase();
	after = QString(firstLetter).append (after.mid (1));
}

void Idable::escapeStringForXML(QString &after)
{
	after.replace("&", "&amp;")
			.replace("<", "&lt;")
			.replace(">", "&gt;")
			.replace ("\"", "&quot;");
}

void Idable::unescapeStringFromXML(QString &after)
{
	after.replace ("&quot;", "\"")
			.replace("&gt;", ">")
			.replace("&lt;", "<")
			.replace("&amp;", "&");
}

bool Idable::fullTextSearch (const QRegExp &searchRegExp, QStringList &resultsList) const
{
	bool found = Timed::fullTextSearch (searchRegExp, resultsList);

	QString stringTobeAppended;
	if (_name.contains (searchRegExp))
	{
		found = true;
		stringTobeAppended = QString ("Name: %1").arg (_name);
		resultsList << stringTobeAppended;
	}

	if (QString ("%1").arg (_id).contains (searchRegExp))
	{
		found = true;
		stringTobeAppended = QString ("ID: %1").arg (_id);
		resultsList << stringTobeAppended;
	}

	return found;
}
