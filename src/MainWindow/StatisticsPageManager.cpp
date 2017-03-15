#include "StatisticsPageManager.h"
#include "Family.h"
#include "Individual.h"

StatisticsPageManager::StatisticsPageManager (Ui::MainWindow *_ui, QObject *parent) :
		QObject(parent), ui (_ui),
		_treeModel (dynamic_cast<TreeModel*>(ui->treeView->model ())),
		_tableModel (dynamic_cast<TableModel*>(ui->tableView->model ()))
{
	setupColorList ();
	initializeFamilyAllIndividualsStatsPieView ();
	initializeFamilyMaleIndividualsStatsPieView ();
	initializeGenderStatsPieView ();
	initializeAliveOrDeadStatsPieView ();
	initializeMarriageStatsPieView ();
	initializeHaveChildrenStatsPieView ();
	initializeSpouseCountStatsPieView ();
	initializeChildCountStatsPieView ();
	initializeChildCountForMarriedStatsPieView ();
	initializeSameNameCountStatsModel ();

	regenerateAllPieViews ();
	connect (_treeModel, SIGNAL(rowInserted()), this, SLOT(regenerateAllPieViews()));
	connect (_treeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(regenerateAllPieViews()));
	connect (_treeModel, SIGNAL(modelReset()), this, SLOT(regenerateAllPieViews()));
}

void StatisticsPageManager::setupColorList ()
{
	_colorList.clear();
	_colorList.append("blue");
	_colorList.append("blueviolet");
	_colorList.append("brown");
	_colorList.append("burlywood");
	_colorList.append("cadetblue");
	_colorList.append("chartreuse");
	_colorList.append("chocolate");
	_colorList.append("coral");
	_colorList.append("cornflowerblue");
	_colorList.append("crimson");
	_colorList.append("darkblue");
	_colorList.append("darkcyan");
	_colorList.append("darkgoldenrod");
	_colorList.append("darkgray");
	_colorList.append("darkgreen");
	_colorList.append("darkgrey");
	_colorList.append("darkkhaki");
	_colorList.append("darkmagenta");
	_colorList.append("darkolivegreen");
	_colorList.append("darkorange");
	_colorList.append("darkorchid");
	_colorList.append("darkred");
	_colorList.append("darksalmon");
	_colorList.append("darkseagreen");
	_colorList.append("darkslateblue");
	_colorList.append("darkslategray");
	_colorList.append("darkslategrey");
	_colorList.append("darkturquoise");
	_colorList.append("darkviolet");
	_colorList.append("deeppink");
	_colorList.append("deepskyblue");
	_colorList.append("dimgray");
	_colorList.append("dimgrey");
	_colorList.append("dodgerblue");
	_colorList.append("firebrick");
	_colorList.append("floralwhite");
	_colorList.append("forestgreen");
	_colorList.append("fuchsia");
	_colorList.append("gainsboro");
	_colorList.append("ghostwhite");
	_colorList.append("gold");
	_colorList.append("goldenrod");
	_colorList.append("gray");
	_colorList.append("green");
	_colorList.append("greenyellow");
	_colorList.append("grey");
	_colorList.append("honeydew");
	_colorList.append("hotpink");
	_colorList.append("indianred");
	_colorList.append("indigo");
	_colorList.append("ivory");
	_colorList.append("khaki");
	_colorList.append("lavender");
	_colorList.append("lavenderblush");
	_colorList.append("lawngreen");
	_colorList.append("lemonchiffon");
	_colorList.append("lightblue");
	_colorList.append("lightcoral");
	_colorList.append("lightcyan");
	_colorList.append("lightgoldenrodyellow");
	_colorList.append("lightgray");
	_colorList.append("lightgreen");
	_colorList.append("lightgrey");
	_colorList.append("lightpink");
	_colorList.append("lightsalmon");
	_colorList.append("lightseagreen");
	_colorList.append("lightskyblue");
	_colorList.append("lightslategray");
	_colorList.append("lightslategrey");
	_colorList.append("lightsteelblue");
	_colorList.append("lightyellow");
	_colorList.append("lime");
	_colorList.append("limegreen");
	_colorList.append("linen");
	_colorList.append("magenta");
	_colorList.append("maroon");
	_colorList.append("mediumaquamarine");
	_colorList.append("mediumblue");
	_colorList.append("mediumorchid");
	_colorList.append("mediumpurple");
	_colorList.append("mediumseagreen");
	_colorList.append("mediumslateblue");
	_colorList.append("mediumspringgreen");
	_colorList.append("mediumturquoise");
	_colorList.append("mediumvioletred");
	_colorList.append("midnightblue");
	_colorList.append("mintcream");
	_colorList.append("mistyrose");
	_colorList.append("moccasin");
	_colorList.append("navajowhite");
	_colorList.append("navy");
	_colorList.append("oldlace");
	_colorList.append("olive");
	_colorList.append("olivedrab");
	_colorList.append("orange");
	_colorList.append("orangered");
	_colorList.append("orchid");
	_colorList.append("palegoldenrod");
	_colorList.append("palegreen");
	_colorList.append("paleturquoise");
	_colorList.append("palevioletred");
	_colorList.append("papayawhip");
	_colorList.append("peachpuff");
	_colorList.append("peru");
	_colorList.append("pink");
	_colorList.append("plum");
	_colorList.append("powderblue");
	_colorList.append("purple");
	_colorList.append("red");
	_colorList.append("rosybrown");
	_colorList.append("royalblue");
	_colorList.append("saddlebrown");
	_colorList.append("salmon");
	_colorList.append("sandybrown");
	_colorList.append("seagreen");
	_colorList.append("seashell");
	_colorList.append("sienna");
	_colorList.append("silver");
	_colorList.append("skyblue");
	_colorList.append("slateblue");
	_colorList.append("slategray");
	_colorList.append("slategrey");
	_colorList.append("snow");
	_colorList.append("springgreen");
	_colorList.append("steelblue");
	_colorList.append("tan");
	_colorList.append("teal");
	_colorList.append("thistle");
	_colorList.append("tomato");
	_colorList.append("transparent");
	_colorList.append("turquoise");
	_colorList.append("violet");
	_colorList.append("wheat");
	_colorList.append("white");
	_colorList.append("whitesmoke");
	_colorList.append("yellow");
	_colorList.append("yellowgreen");
}

void StatisticsPageManager::initializeModelAndPieView (PieView *view, QStandardItemModel *&model,
						       int rows, QString column1, QString column2)
{
	model = new QStandardItemModel (rows, 2);
	model->setHeaderData (0, Qt::Horizontal, column1);
	model->setHeaderData (1, Qt::Horizontal, column2);

	view->setModel (model);
	view->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
	view->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
}

void StatisticsPageManager::initializeFamilyAllIndividualsStatsPieView ()
{
	initializeModelAndPieView (ui->familyAllIndividualsStatsPieView,
				   _familyAllIndividualsStatsModel,
				   SettingsManager::s_MaxPieViewEntries,
				   "Family: All Members Count", "Members");
}

void StatisticsPageManager::initializeFamilyMaleIndividualsStatsPieView ()
{
	initializeModelAndPieView (ui->familyMaleIndividualsStatsPieView,
				   _familyMaleIndividualsStatsModel,
				   SettingsManager::s_MaxPieViewEntries,
				   "Family: Male Members Count", "Male Members");
}

void StatisticsPageManager::initializeGenderStatsPieView ()
{
	initializeModelAndPieView (ui->genderStatsPieView, _genderStatsModel,
				   3, "Gender", "Count");
}

void StatisticsPageManager::initializeAliveOrDeadStatsPieView ()
{
	initializeModelAndPieView (ui->aliveOrDeadStatsPieView, _aliveOrDeadStatsModel,
				   3, "Vitality", "Count");
}

void StatisticsPageManager::initializeMarriageStatsPieView ()
{
	initializeModelAndPieView (ui->marriageStatsPieView, _marriageStatsModel,
				   3, "Marital Status", "Count");
}

void StatisticsPageManager::initializeHaveChildrenStatsPieView ()
{
	initializeModelAndPieView (ui->haveChildrenStatsPieView, _haveChildrenStatsModel,
				   3, "Have Children", "Count");
}

void StatisticsPageManager::initializeSpouseCountStatsPieView ()
{
	initializeModelAndPieView (ui->spouseCountStatsPieView,
				   _spouseCountStatsModel,
				   SettingsManager::s_MaxPieViewEntries,
				   "Spouse Count", "Count");
}

void StatisticsPageManager::initializeChildCountStatsPieView ()
{
	initializeModelAndPieView (ui->childCountStatsPieView, _childCountStatsModel,
				   SettingsManager::s_MaxPieViewEntries,
				   "Children Count For Males", "Count");
}

void StatisticsPageManager::initializeChildCountForMarriedStatsPieView ()
{
	initializeModelAndPieView (ui->childCountStatsForMarriedPieView, _childCountForMarriedStatsModel,
				   SettingsManager::s_MaxPieViewEntries,
				   "Children Count For Married Couples", "Count");
}

void StatisticsPageManager::initializeSameNameCountStatsModel ()
{
	initializeModelAndPieView (ui->sameNameCountStatsPieView, _sameNameCountStatsModel,
				   SettingsManager::s_MaxPieViewEntries,
				   "Same Name Count", "Count");
}

void StatisticsPageManager::updatePieModelBinary (QStandardItemModel *model,
						  QString name1, int value1,
						  QString name2, int value2,
						  QString color1, QString color2)
{
	QMap <int, QString> valueToNameMap;
	valueToNameMap.insertMulti (value1, name1);
	valueToNameMap.insertMulti (value2, name2);
	int totalCount = value1 + value2;

	QStringList colorList;
	colorList << color1;
	colorList << color2;

	updatePieModel (model, valueToNameMap, totalCount, colorList);
}

void StatisticsPageManager::updatePieModel (QStandardItemModel *model,
					    QMap <int, QString> &countToNameMap,
					    int totalCount,
					    QStringList &colorList)
{
	int colorListIteration = 0;

	QString maxRowDisplayString = "Rest";
	int maxRowCount = 0;
	int maxRows = model->rowCount (QModelIndex());

	for (int i = 0; i < maxRows; i++)
	{
		QModelIndex col0Index = model->index(i, 0, QModelIndex());
		QModelIndex col1Index = model->index(i, 1, QModelIndex());
		model->setData(col0Index, QVariant());
		model->setData(col1Index, QVariant());
		model->setData(col0Index, QVariant(), Qt::DecorationRole);
		model->setData(col0Index, QVariant(), Qt::ToolTipRole);
	}

	if (!totalCount)
		return;

	int row = 0;
	for (QMap <int, QString>::const_iterator i = countToNameMap.constEnd () - 1;
		i + 1 != countToNameMap.constBegin (); i--)
	{
		int value = i.key ();
		int percentage = qRound(value*100.0/totalCount);
		QString name = QString ("%1 [%2%]").arg (i.value ()).arg (percentage);

		if (row + 1 >= maxRows)
		{
			maxRowCount += value;
			row ++;
			continue;
		}
		QModelIndex col0Index = model->index(row, 0, QModelIndex());
		QModelIndex col1Index = model->index(row, 1, QModelIndex());
		model->setData(col0Index, name);
		model->setData(col1Index, value);
		model->setData(col0Index, QColor(colorList.value (colorListIteration)), Qt::DecorationRole);
		QString toolTipString = "<b>" + name + "</b>:<br>" +
					QString("%1% (%2 out of %3)")
					.arg(percentage)
					.arg(value).arg(totalCount);
		model->setData(col0Index, toolTipString, Qt::ToolTipRole);
		model->setData (col1Index, toolTipString, Qt::ToolTipRole);
		if (colorListIteration < colorList.size() -1)
			colorListIteration++;
		else
			colorListIteration = 0;
		row ++;
	}

	if (maxRowCount)
	{
		int percentage = qRound (maxRowCount*100.0/totalCount);
		QString name = QString ("%1 [%2%]").arg (maxRowDisplayString).arg (percentage);
		QModelIndex col0Index = model->index(maxRows - 1, 0, QModelIndex());
		QModelIndex col1Index = model->index(maxRows - 1, 1, QModelIndex());
		model->setData(col0Index, name);
		model->setData(col1Index, maxRowCount);
		model->setData(col0Index, QColor(colorList.value (colorListIteration)), Qt::DecorationRole);
		QString toolTipString = "<b>" + name + "</b>:<br>" +
					QString("%1% (%2 out of %3)")
					.arg(percentage).arg(maxRowCount).arg(totalCount);
		model->setData(col0Index, toolTipString, Qt::ToolTipRole);
		model->setData(col1Index, toolTipString, Qt::ToolTipRole);
	}
}

void StatisticsPageManager::regenerateFamilyAllIndividualsStatsPieView ()
{
	QMap <int, QString> totalCountToFamilyNameMap;

	int totalCount = 0;
	foreach (Family *family, Family::s_FamilyList)
	{
		int count = family->data (Familier::e_col_totalCount, Qt::DisplayRole, 0).toInt ();
		totalCount += count;
		totalCountToFamilyNameMap.insertMulti (count, family->getName ());
	}

	updatePieModel (_familyAllIndividualsStatsModel, totalCountToFamilyNameMap,
			totalCount, _colorList);
}

void StatisticsPageManager::regenerateFamilyMaleIndividualsStatsPieView ()
{
	QMap <int, QString> maleCountToFamilyNameMap;

	int totalMaleCount = 0;
	foreach (Family *family, Family::s_FamilyList)
	{
		int count = family->data (Familier::e_col_sonCount, Qt::DisplayRole, 0).toInt ();
		totalMaleCount += count;
		maleCountToFamilyNameMap.insertMulti (count, family->getName ());
	}

	updatePieModel (_familyMaleIndividualsStatsModel, maleCountToFamilyNameMap,
			totalMaleCount, _colorList);
}

void StatisticsPageManager::regenerateGenderStatsPieView ()
{
	int maleCount = 0;
	int femaleCount = 0;

	foreach (Individual *individual, Individual::s_IndividualList)
	{
		if (individual->isDummy ())
			continue;

		if (individual->isFemale ())
			femaleCount ++;
		else
			maleCount ++;
	}

	updatePieModelBinary (_genderStatsModel,
			      "Male", maleCount,
			      "Female", femaleCount,
			      "darkgreen", "pink");
}

void StatisticsPageManager::regenerateAliveOrDeadStatsPieView ()
{
	int aliveCount = 0;
	int deadCount = 0;

	foreach (Individual *individual, Individual::s_IndividualList)
	{
		if (individual->isDummy ())
			continue;

		if (individual->isDead ())
			deadCount ++;
		else
			aliveCount ++;
	}

	updatePieModelBinary (_aliveOrDeadStatsModel,
			      "Dead", deadCount,
			      "Alive", aliveCount);
}

void StatisticsPageManager::regenerateMarriageStatsPieView ()
{
	int singleCount = 0;
	int marriedCount = 0;

	foreach (Individual *individual, Individual::s_IndividualList)
	{
		if (individual->isDummy ())
			continue;

		if (individual->hasSpouse ())
			marriedCount ++;
		else
			singleCount ++;
	}

	updatePieModelBinary (_marriageStatsModel,
			      "Single", singleCount,
			      "Married", marriedCount);
}

void StatisticsPageManager::regenerateHaveChildrenStatsPieView ()
{
	int withChildCount = 0;
	int withoutChildCount = 0;

	foreach (Individual *individual, Individual::s_IndividualList)
	{
		if (individual->isDummy ())
			continue;

		if (individual->hasOffspring ())
			withChildCount ++;
		else
			withoutChildCount ++;
	}

	updatePieModelBinary (_haveChildrenStatsModel,
			      "Yes", withChildCount,
			      "No", withoutChildCount);
}

void StatisticsPageManager::regenerateSpouseCountStatsPieView ()
{
	QMap <QString, int> individualCountToSpouseCountMap;
	QMap <int, QString> spouseCountToIndividualCountMap;

	int totalCount = 0;
	foreach (Individual *individual, Individual::s_IndividualList)
	{
		QString spouseCountString = individual->spouseCountAsString ();
		totalCount ++;
		individualCountToSpouseCountMap [spouseCountString] ++;
	}

	for (QMap<QString, int>::const_iterator i = individualCountToSpouseCountMap.constBegin ();
		i != individualCountToSpouseCountMap.constEnd (); i++)
		spouseCountToIndividualCountMap [i.value ()] = i.key ();

	updatePieModel (_spouseCountStatsModel, spouseCountToIndividualCountMap,
			totalCount, _colorList);
}

void StatisticsPageManager::regenerateChildCountStatsPieView ()
{
	QMap <QString, int> individualCountToChildCountMap;
	QMap <int, QString> childCountToIndividualCountMap;

	int totalCount = 0;
	foreach (Individual *individual, Individual::s_IndividualList)
	{
		if (individual->isFemale ())
			continue;

		QString childCountString = individual->offspringCountAsString ();
		totalCount ++;
		individualCountToChildCountMap [childCountString] ++;
	}

	for (QMap<QString, int>::const_iterator i = individualCountToChildCountMap.constBegin ();
		i != individualCountToChildCountMap.constEnd (); i++)
		childCountToIndividualCountMap [i.value ()] = i.key ();

	updatePieModel (_childCountStatsModel, childCountToIndividualCountMap,
			totalCount, _colorList);
}

void StatisticsPageManager::regenerateChildCountForMarriedStatsPieView ()
{
	QMap <QString, int> individualCountToChildCountMap;
	QMap <int, QString> childCountToIndividualCountMap;

	int totalCount = 0;
	foreach (Individual *individual, Individual::s_IndividualList)
	{
		if (individual->isFemale () || !individual->hasSpouse ())
			continue;

		QString childCountString = individual->offspringCountAsString ();
		totalCount ++;
		individualCountToChildCountMap [childCountString] ++;
	}

	for (QMap<QString, int>::const_iterator i = individualCountToChildCountMap.constBegin ();
		i != individualCountToChildCountMap.constEnd (); i++)
		childCountToIndividualCountMap [i.value ()] = i.key ();

	updatePieModel (_childCountForMarriedStatsModel, childCountToIndividualCountMap,
			totalCount, _colorList);
}

void StatisticsPageManager::regenerateSameNameCountStatsModel ()
{
	QMap <int, QString> sameNameCountToNameMap;

	int totalCount = _tableModel->rowCount (QModelIndex ());

	QString lastName = "";
	int currentCount = 0;

	for (int row = 0; row < totalCount; ++row)
	{
		QModelIndex index = _tableModel->index (row, 0, QModelIndex ());
		QString currentName = index.data (Qt::EditRole).toString ();

		if (lastName.isEmpty () || lastName == currentName)
			currentCount ++;
		else
		{
			sameNameCountToNameMap.insertMulti (currentCount, lastName);
			currentCount = 1;
		}

		lastName = currentName;
	}

	if (currentCount)
		sameNameCountToNameMap.insertMulti (currentCount, lastName);

	updatePieModel (_sameNameCountStatsModel, sameNameCountToNameMap,
			totalCount, _colorList);
}

void StatisticsPageManager::regenerateAllPieViews ()
{
	regenerateFamilyAllIndividualsStatsPieView ();
	regenerateFamilyMaleIndividualsStatsPieView ();
	regenerateGenderStatsPieView ();
	regenerateAliveOrDeadStatsPieView ();
	regenerateMarriageStatsPieView ();
	regenerateHaveChildrenStatsPieView ();
	regenerateSpouseCountStatsPieView ();
	regenerateChildCountStatsPieView ();
	regenerateChildCountForMarriedStatsPieView ();
	regenerateSameNameCountStatsModel ();
}
