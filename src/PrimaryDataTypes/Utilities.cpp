#include "Utilities.h"
#include "Individual.h"
#include "Family.h"

Utilities::Utilities()
{
}

void Utilities::convertFTMLToDisplayFormatHelperFunction (QString &output, QString &endOut,
							  const int &rotateAt, QString &tempOut,
							  QString &plainOutput)
{
	if (rotateAt > plainOutput.length () + tempOut.length ()) endOut += tempOut;
	else if (rotateAt < plainOutput.length ())
		output += tempOut;
	else
	{
		endOut += tempOut.mid (0, rotateAt - plainOutput.length ());
		output += tempOut.mid (rotateAt - plainOutput.length ());
	}
	plainOutput += tempOut;
}

Familier* Utilities::stringToFamilier (QString input, Individual* individual, QString &tag)
{
	QStringList tags = input.split("=", QString::SkipEmptyParts);

	Familier *fam = NULL;

	if (individual && tags.count() == 1)
	{
		tag = tags.at(0);

		if (0 == tag.compare ("self", Qt::CaseInsensitive))
			fam = individual;
		else if (0 == tag.compare ("father", Qt::CaseInsensitive))
			fam = individual->getFather ();
		else if (0 == tag.compare ("mother", Qt::CaseInsensitive))
			fam = individual->getMother ();
		else if (0 == tag.compare ("family", Qt::CaseInsensitive))
			fam = individual->getFamily ();
		else if (0 == tag.compare ("spouse", Qt::CaseInsensitive) && individual->hasSpouse())
			fam = individual->getSpouse(0);
		else if (0 == tag.compare ("sibling", Qt::CaseInsensitive) && individual->hasSibling())
		{
			if (individual->hasElderSibling())
				fam = individual->getSibling(0);
			else if (individual->hasYoungerSibling())
				fam = individual->getSibling(individual->firstYoungerSibling());
		}
	}
	else if (tags.count() == 2)
	{
		QString key = tags.at(0);
		tag = key;
		int value = tags.at(1).toInt();

		if (value > 0)
		{
			if (individual)
			{
				if (0 == key.compare ("sibling", Qt::CaseInsensitive))
					fam = individual->getSibling(value - 1);
				else if (0 == key.compare ("spouse", Qt::CaseInsensitive))
					fam = individual->getSpouse(value - 1);
				else if (0 == key.compare ("child", Qt::CaseInsensitive))
				{
					int seen = 0;
					for (int i = 0; i < individual->spouseCount(); i++)
					{
						IDTYPE spid = individual->getSpouse(i)->getId ();
						int count = individual->offspringCount(spid);
						if (seen + count > value - 1)
						{
							fam = individual->getOffspring(value - 1 - seen, spid);
							break;
						}
						else
							seen += count;
					}
				}
			}
			if (0 == key.compare ("iid", Qt::CaseInsensitive))
			{
				if (Individual::s_IdToIndividualHash.contains(value))
					fam = Individual::s_IdToIndividualHash.value(value);
			}
			else if (0 == key.compare ("fid", Qt::CaseInsensitive))
			{
				if (Family::s_IdToFamilyHash.contains(value))
					fam = Family::s_IdToFamilyHash.value(value);
			}
		}
	}

	return fam;
}

QString Utilities::convertFTMLToDisplayFormat (const QString &input, QString &output,
					       const bool useFullName, const bool createAnchor,
					       const int rotateAt, Individual *individual)
{
	if (input.isEmpty ()|| -1 == input.indexOf("[["))
	{
		output = input;
		return output;
	}

	QString endOut;
	QString tempOut;
	QString plainOutput;
	output.clear ();
	endOut.clear ();
	tempOut.clear ();
	plainOutput.clear ();
	int start = 0;
	int end = 0;

	while (-1 != (start = input.indexOf("[[", end)))
	{
		tempOut = input.mid (end, start - end);
		convertFTMLToDisplayFormatHelperFunction (output, endOut, rotateAt, tempOut, plainOutput);

		end = input.indexOf("]]", start);
		if (end == -1)
		{
			tempOut = input.mid(start + 2);
			convertFTMLToDisplayFormatHelperFunction (output, endOut, rotateAt, tempOut, plainOutput);
			break;
		}

		if (start == end)
			continue;

		QString FTML = input.mid(start + 2, end - start - 2);

		QString tag;
		Familier *fam = stringToFamilier (FTML, individual, tag);

		bool replaceTag = false;
		if (tag.length () && 0 != tag.compare ("iid", Qt::CaseInsensitive)
				&& 0 != tag.compare ("fid", Qt::CaseInsensitive))
			replaceTag = true;

		if (fam)
		{
			QString aTag = fam->getAnchor(0, useFullName, createAnchor)
					.replace (QRegExp (">.*</a>"), ">");
			QString aEndTag = "</a>";
			QString name;
			if (replaceTag)
				name = tag + " [";
			name += fam->getAnchor (0, useFullName, false)
					.replace ("<u>", "").replace ("</u>", "");
			if (replaceTag)
				name += "]";

			QString tEndOut, tOutput;
			convertFTMLToDisplayFormatHelperFunction (tOutput, tEndOut, rotateAt,
								  name, plainOutput);

			if (tOutput.size ())
				output += aTag + tOutput + aEndTag;
			if (tEndOut.size ())
				endOut += aTag + tEndOut + aEndTag;
		}
		else
		{
			tempOut = input.mid(start, end - start);
			convertFTMLToDisplayFormatHelperFunction (output, endOut, rotateAt, tempOut, plainOutput);
			qDebug () << FTML << "is not a valid tag, skipped.";
		}
		end += 2;
	}

	tempOut = input.mid(end);
	convertFTMLToDisplayFormatHelperFunction (output, endOut, rotateAt, tempOut, plainOutput);

	output = output + " " + endOut;

	return plainOutput;
}
