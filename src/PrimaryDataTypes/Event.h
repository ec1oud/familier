#ifndef EVENT_H
#define EVENT_H

#include "IdableWithChildren.h"
#include "Dated.h"

#include <QList>

class Individual;
class Union;

class Event : public IdableWithChildren
{
	DECLAREAUTOITEMMANAGEMENT(Event)

public:
	enum AnchorType { members, membersListingOnly };
	enum EventType { e_None, e_BirthEvent, e_DeathEvent, e_MarriageEvent, e_DivorceEvent };
	enum NameRepresentation { e_Anchor, e_PlainText, e_Ids };

protected:
	Dated			_dated;
	QString			_place;
	QString			_description;

public:
	Event (const QDateTime &creationDateTime = QDateTime::currentDateTime(),
	       const QDateTime &modificationDateTime = QDateTime::currentDateTime(),
	       const unsigned long modificationCount = 0);
	Event (const QDomElement &domElement);
	Event (const Event &event);
	virtual ~Event ();
	virtual void setupItemLinks (const QDomElement &domElement);
	virtual IdableWithChildren* getItemWithId (const IDTYPE id) const;

public:
	virtual QString		getSpecialEventName		(NameRepresentation nameRepresentation = e_PlainText) const;
	GETFUNCTION	(Place,		QString,		place)
	GETFUNCTION	(Dated,		Dated,			dated)
	GETFUNCTION	(Description,	QString,		description)

	GETSETMAXIDHEAD (Event)

	SETFUNCTIONDECL	(Dated,		Dated &,		dated)
	SETFUNCTIONDECL	(Dated,		QString &,		dated)
	SETFUNCTIONDECL	(Place,		QString &,		place)
	SETFUNCTIONDECL	(Description,	QString &,		description)

	virtual QString		getChildTypeName	() const;

	friend QTextStream &	operator <<		(QTextStream &stream, Event &event);
	QTextStream &		toGEDCOM		(QTextStream &stream) const;
	void			fromGEDCOM		(const QStringList &gedcomEntryStringList,
							 const QHash <QString, QStringList> &xRefToNoteGEDCOMInfoHash);

	friend QDebug		operator <<		(QDebug debug, Event &event);

	bool			operator <		(const Event &event) const;
	bool			operator >		(const Event &event) const;

	virtual QString		getListOfAnchors	(const int type, const bool useFullName,
							 const bool createAnchor);

	bool			wasDateParsed		() const { return _dated.wasDateParsed (); }

	QString			toDisplayDate		(Dated &refDate) const;
	QString			toDisplayDate		(Event *refEvent) const;
	QString			toDisplayDate		() const;

	bool			hasDescription		() const;
	bool			hasEventMember		(const IDTYPE id) const;
	QString			getDescriptionInDisplayFormat ();
	QString			getEventInformation	();

	bool			didEventHappenToday	() const;

	virtual bool		fullTextSearch		(const QRegExp &searchRegExp,
							 QStringList &resultsList) const;
};

#endif /* EVENT_H */
