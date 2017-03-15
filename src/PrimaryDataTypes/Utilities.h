#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>

class Individual;
class Familier;

class Utilities
{
public:
	Utilities();

	static Familier*	stringToFamilier	(QString input, Individual *individual, QString &tag);

	static QString		convertFTMLToDisplayFormat
							(const QString &input, QString &output,
							 const bool useFullName, const bool createAnchor,
							 const int rotateAt = -1,
							 Individual *individual = NULL);

private:
	static void			convertFTMLToDisplayFormatHelperFunction
							(QString &output, QString &endOut,
							 const int &rotateAt, QString &tempOut,
							 QString &plainOutput);
};

#endif // UTILITIES_H
