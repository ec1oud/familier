#include "Timed.h"

Timed::Timed (const QDateTime &creationDateTime,
	      const QDateTime &modificationDateTime,
	      const unsigned long modificationCount)
		      : _creationDateTime (creationDateTime),
		      _modificationDateTime (modificationDateTime),
		      _modificationCount(modificationCount)
{
	_readModificationCount = _modificationCount;
	_recordingModificationsBlocked = false;
}

Timed::Timed (const QDomElement &domElement)
{
	QDomNodeList metadataNodeList = domElement.elementsByTagName("MetaData");
	QDomNode metadataNode = metadataNodeList.at(0);
	if (metadataNode.isElement())
	{
		QDomElement metadataElement = metadataNode.toElement();
		QString creationDateTime = metadataElement.attribute("creationdatetime", QDateTime::currentDateTime().toString());
		_creationDateTime = QDateTime::fromString(creationDateTime);
		QString modificationDateTime = metadataElement.attribute("modificationdatetime", QDateTime::currentDateTime().toString());
		_modificationDateTime = QDateTime::fromString(modificationDateTime);
		QString modificationCount = metadataElement.attribute("modificationcount", "0");
		_modificationCount = modificationCount.toULong();
	}
	else
	{
		_creationDateTime = QDateTime::currentDateTime();
		_modificationDateTime = QDateTime::currentDateTime();
		_modificationCount = 0;
	}
	_readModificationCount = _modificationCount;
	_recordingModificationsBlocked = false;
}

Timed::Timed (const Timed &timed)
{
	_creationDateTime = timed._creationDateTime;
	_modificationDateTime = timed._modificationDateTime;
	_modificationCount = timed._modificationCount;
}

Timed::~Timed ()
{
}

/*bool Timed::isValid() const
{
	return _creationDateTime.isValid() && _modificationDateTime.isValid();
}*/

bool Timed::setCreationTime (const QDateTime creationDateTime)
{
	_creationDateTime = creationDateTime;
	if (_modificationDateTime < creationDateTime)
		_modificationDateTime = creationDateTime;
	return true;
}

bool Timed::fullTextSearch (const QRegExp &searchRegExp, QStringList &resultsList) const
{
	Q_UNUSED (searchRegExp);
	Q_UNUSED (resultsList);

	return false;
}

QTextStream& operator << (QTextStream &stream, Timed &timed)
{
	stream << "\t\t<MetaData creationdatetime=\"" << timed._creationDateTime.toString()
		<< "\" modificationdatetime=\"" << timed._modificationDateTime.toString() << "\"";
	if (timed._modificationCount)
		stream << " modificationcount=\"" << timed._modificationCount << "\"";
	stream << " />" << endl;

	return stream;
}

QDebug operator <<(QDebug debug, Timed &timed)
{
	debug << "Creation Date Time:" << timed._creationDateTime << endl;
	debug << "Modification Date Time: " << timed._modificationDateTime << endl;
	debug << "Modification Count:" << timed._modificationCount << endl;
	return debug;
}

bool Timed::updateModificationTime (QDateTime modificationDateTime)
{
	if (_recordingModificationsBlocked)
		return false;

	if (_modificationCount == _readModificationCount)
		_modificationCount ++;
	_modificationDateTime = modificationDateTime;
	return true;
}

