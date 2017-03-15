#include "Dated.h"
#include "Idable.h"

#define	OPERATOR(X) \
bool Dated::operator X (const Dated &dated) const \
{ \
	return QDate (_year < 0 ? QDate::currentDate().year() : _year, \
		      _month < 0 ? 6 : _month, \
		      _day < 0 ? 15 : _day) \
	X QDate (dated._year < 0 ? QDate::currentDate().year() : dated._year, \
		 dated._month < 0 ? 6 : dated._month, \
		 dated._day < 0 ? 15 : dated._day); \
}

Dated::Dated ()
{
	_year = -1;
	_month = -1;
	_day = -1;

	_isString = false;
	_dateWasParsed = false;
}

Dated::Dated (const QString &dated)
{
	_dateString = dated;
	parseDate (dated);
}

Dated::Dated (const QDate &qDate)
{
	_year = qDate.year();
	_month = qDate.month();
	_day = qDate.day();

	_isString = false;
	_dateWasParsed = true;
	_dateString = qDate.toString("yyyyMMdd");
}

Dated::~Dated ()
{
}

void Dated::parseDate (const QString &date)
{
	_dateWasParsed = true;
	_year = -1;
	_month = -1;
	_day = -1;
	_isString = false;
	QDate qDate = QDate::fromString (date, "yyyyMMdd");
	if (qDate.isValid ())
	{
		_year = qDate.year ();
		_month = qDate.month ();
		_day = qDate.day ();
	}
	else
	{
		qDate = QDate::fromString (date, "yyyyMM");
		if (qDate.isValid ())
		{
			_year = qDate.year ();
			_month = qDate.month ();
		}
		else
		{
			qDate = QDate::fromString (date, "yyyy");
			if (qDate.isValid ())
				_year = qDate.year ();
			else
			{
				qDate = QDate::fromString (date, "MMdd");
				if (qDate.isValid ())
				{
					_month = qDate.month ();
					_day = qDate.day ();
				}
				else
				{
					_isString = true;
					_dateWasParsed = false;
				}
			}
		}
	}
}

QString Dated::getDayth () const
{
	return getDayth (_day);
}

bool Dated::setDate (const QString &date)
{
	if (_dateString == date)
		return false;

	_dateString = date;
	parseDate (date);

	return true;
}

bool Dated::setDate (const int day, const int month, const int year)
{
	if (_year == year && _month == month && _day == day)
		return false;

	_dateWasParsed = false;
	_year = year;
	_month = month;
	_day = day;
	_isString = false;

	QDate qDate (year, month, day);
	_dateString = qDate.toString ("yyyyMMdd");

	return true;
}

QTextStream& operator << (QTextStream &stream, Dated &dated)
{
	if (dated._dateString.length())
	{
		QString dateString = dated._dateString;
		Idable::escapeStringForXML (dateString);
		stream << " date=\"" << dateString << "\"";
	}
	return stream;
}

QDebug operator << (QDebug debug, Dated &dated)
{
	debug << "Date:" << dated.getDateString();
	return debug;
}

int Dated::operator - (const Dated &dated) const
{
	if (_year == -1 || dated._year == -1)
		return -1;

	int age = -1;
	if (_day == -1 || dated._day == -1)
	{
		if (_month == -1 || dated._month == -1)
			age = _year - dated._year;
		else
			age = _year - dated._year  - (_month < dated._month);
	}
	else
		age = _year - dated._year  - (_month - (_day < dated._day) < dated._month);

	return age;
}

QString Dated::toDisplayString () const
{
	if (_isString)
		return _dateString;

	QDate qDate (_year > 0 ? _year : QDate::currentDate().year(), _month > 0 ? _month : 1, _day > 0 ? _day : 1);
	if (_day > 0)
		return qDate.toString(QString("d%1 MMM, yyyy")).arg(getDayth ());
	if (_month > 0)
		return qDate.toString("MMM, yyyy");
	return qDate.toString("yyyy");
}

QString Dated::toDisplayString (const Dated &refDated) const
{
	QString ret = toDisplayString ();

	int age = refDated - *this;
	if (age >=0)
		ret += QString (" (%1)").arg (age);

	return ret;
}

int Dated::quay () const
{
	if (getDate ().isValid ())
		return 3;
	return 0;
}

QString Dated::getDayth (int day)
{
	switch (day)
	{
	case 1:
	case 21:
	case 31:
		return "st";
	case 2:
	case 22:
		return "nd";
	case 3:
	case 23:
		return "rd";
	default:
		return "th";
	}
}

bool Dated::fullTextSearch (const QRegExp &searchRegExp, QStringList &resultsList) const
{
	if (_dateString.contains (searchRegExp))
	{
		QString stringTobeAppended;
		stringTobeAppended = QString ("Dated: %1").arg (_dateString);
		resultsList << stringTobeAppended;
		return true;
	}

	return false;
}

OPERATOR (==)
OPERATOR (!=)
OPERATOR (< )
OPERATOR (<=)
OPERATOR (> )
OPERATOR (>=)
