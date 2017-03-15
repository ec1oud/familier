#ifndef DATED_H
#define DATED_H

#include "Common.h"
#include <QDate>
#include <QString>
#include <QStringList>
#include <QMetaType>

class Dated
{
	bool			_dateWasParsed;
	bool			_isString;
	int			_day, _month, _year;
	QString			_dateString;

public:
	Dated			();
	Dated			(const QString &dated);
	Dated			(const QDate &qDate);
	virtual ~Dated		();

private:
	void			parseDate 			(const QString &date);
	QString			getDayth			() const;

public:
	bool			wasDateParsed			() const { return _dateWasParsed; }
	bool			isString			() const { return _isString; }
	int			day				() const { return _day; }
	int			month				() const { return _month; }
	int			year				() const { return _year; }
	QDate			getDate				() const { return QDate (_year, _month, _day); }
	QString			getDateString			() const { return _dateString; }

	bool			setDate				(const QString &date);
	bool			setDate				(const int day, const int month, const int year);

	friend QTextStream &	operator <<			(QTextStream &stream, Dated &dated);

	friend QDebug		operator <<			(QDebug debug, Dated &dated);

	int			operator -			(const Dated &dated) const;

	QString			toDisplayString			() const;
	QString			toDisplayString			(const Dated &refDated) const;
	int			quay				() const;

	static QString		getDayth			(int day);

	virtual bool		fullTextSearch			(const QRegExp &searchRegExp,
								 QStringList &resultsList) const;

	bool			operator ==			(const Dated &dated) const;
	bool			operator !=			(const Dated &dated) const;
	bool			operator <			(const Dated &dated) const;
	bool			operator <=			(const Dated &dated) const;
	bool			operator >			(const Dated &dated) const;
	bool			operator >=			(const Dated &dated) const;
};

Q_DECLARE_METATYPE(Dated)

#endif /* DATED_H */

