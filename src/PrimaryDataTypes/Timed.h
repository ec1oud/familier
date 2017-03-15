#ifndef TIMED_H
#define TIMED_H

#include "Common.h"
#include <QDateTime>
#include <QStringList>

class Timed
{
	bool			_recordingModificationsBlocked;
	QDateTime		_creationDateTime;
	QDateTime		_modificationDateTime;
	unsigned long		_readModificationCount;
	unsigned long		_modificationCount;

public:
	Timed (const QDateTime &creationDateTime = QDateTime::currentDateTime(),
	       const QDateTime &modificationDateTime = QDateTime::currentDateTime(),
	       const unsigned long modificationCount = 0);
	Timed (const QDomElement &domElement);
	Timed (const Timed &timed);
	virtual ~Timed ();
	//virtual bool isValid () const;

	GETFUNCTION	(CreationTime,		QDateTime,		creationDateTime)
	GETFUNCTION	(ModificationTime,	QDateTime,		modificationDateTime)
	GETFUNCTION	(ModificationCount,	unsigned long,		modificationCount)

	bool			setCreationTime		(const QDateTime creationDateTime = QDateTime::currentDateTime ());

	virtual bool		fullTextSearch		(const QRegExp &searchRegExp,
							 QStringList &resultsList) const;

	friend QTextStream &	operator <<		(QTextStream &stream, Timed &timed);

	friend QDebug		operator <<		(QDebug debug, Timed &timed);

	void			blockRecordingOfItemModifications () { _recordingModificationsBlocked = true; }
	void			unblockRecordingOfItemModifications () { _recordingModificationsBlocked = false; }

protected:
	bool			updateModificationTime 	(const QDateTime modificationDateTime = QDateTime::currentDateTime());
};

#endif /* TIMED_H */

