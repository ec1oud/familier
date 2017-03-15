#ifndef IDABLE_H
#define IDABLE_H

#include "Timed.h"

#define IDTYPE	unsigned long

class Idable : public Timed
{
protected:
	IDTYPE			_id;
	QString			_name;

public:
	Idable (const IDTYPE id,
		const QDateTime &creationDateTime, const QDateTime &modificationDateTime,
		const unsigned long modificationCount);
	Idable (const QDomElement &domElement);
	Idable (const Idable &idable);
	virtual ~Idable ();
	//virtual bool isValid () const;

private:
	bool			checkValidity		();

public:
	GETFUNCTION	(Id,			IDTYPE,			id)
	virtual QString		getName			() const { return _name; }

	virtual SETFUNCTIONDECL	(Id,			IDTYPE,			id)
	virtual SETFUNCTIONDECL	(Name,			QString &,		name)

	friend QTextStream &	operator <<		(QTextStream &stream, Idable &idable);

	friend QDebug		operator <<		(QDebug debug, Idable &idable);

	bool			operator <		(const Idable &) const;
	bool			operator >		(const Idable &) const;

	QString			getUnderlinedName	() const;
	QString			getUniqueName		() const;

	static void		toTitleCase		(QString &after);
	static void		escapeStringForXML	(QString &after);
	static void		unescapeStringFromXML	(QString &after);

	virtual bool		fullTextSearch		(const QRegExp &searchRegExp,
							 QStringList &resultsList) const;
};

#endif /* IDABLE_H */
