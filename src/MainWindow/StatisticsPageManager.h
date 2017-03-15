#ifndef STATISTICSPAGEMANAGER_H
#define STATISTICSPAGEMANAGER_H

#include <QObject>
#include "ui_MainWindow.h"
#include "PieView.h"
#include "TreeModel.h"
#include <QStandardItemModel>
#include "SettingsManager.h"
#include "TableModel.h"

class StatisticsPageManager : public QObject
{
	Q_OBJECT

	Ui::MainWindow *		ui;
	TreeModel *			_treeModel;
	TableModel *			_tableModel;

	QStringList			_colorList;

	QStandardItemModel *		_familyAllIndividualsStatsModel;
	QStandardItemModel *		_familyMaleIndividualsStatsModel;
	QStandardItemModel *		_genderStatsModel;
	QStandardItemModel *		_aliveOrDeadStatsModel;
	QStandardItemModel *		_marriageStatsModel;
	QStandardItemModel *		_haveChildrenStatsModel;
	QStandardItemModel *		_spouseCountStatsModel;
	QStandardItemModel *		_childCountStatsModel;
	QStandardItemModel *		_childCountForMarriedStatsModel;
	QStandardItemModel *		_sameNameCountStatsModel;

public:
	explicit StatisticsPageManager (Ui::MainWindow *_ui, QObject *parent = 0);

private:
	void		setupColorList		();

	void		initializeModelAndPieView	(PieView *view, QStandardItemModel *&model,
							 int rows, QString column1, QString column2);

	void		initializeFamilyAllIndividualsStatsPieView ();
	void		initializeFamilyMaleIndividualsStatsPieView ();
	void		initializeGenderStatsPieView ();
	void		initializeAliveOrDeadStatsPieView ();
	void		initializeMarriageStatsPieView ();
	void		initializeHaveChildrenStatsPieView ();
	void		initializeSpouseCountStatsPieView ();
	void		initializeChildCountStatsPieView ();
	void		initializeChildCountForMarriedStatsPieView ();
	void		initializeSameNameCountStatsModel ();

	void		updatePieModelBinary		(QStandardItemModel *model,
							 QString name1, int value1,
							 QString name2, int value2,
							 QString color1 = "darkgreen",
							 QString color2 = "darkred");
	void		updatePieModel			(QStandardItemModel *model,
							 QMap <int, QString> &countToNameMap,
							 int totalCount,
							 QStringList &colorList);

	void		regenerateFamilyAllIndividualsStatsPieView ();
	void		regenerateFamilyMaleIndividualsStatsPieView ();
	void		regenerateGenderStatsPieView ();
	void		regenerateAliveOrDeadStatsPieView ();
	void		regenerateMarriageStatsPieView ();
	void		regenerateHaveChildrenStatsPieView ();
	void		regenerateSpouseCountStatsPieView ();
	void		regenerateChildCountForMarriedStatsPieView ();
	void		regenerateChildCountStatsPieView ();
	void		regenerateSameNameCountStatsModel ();

signals:

public slots:
	void		regenerateAllPieViews		();
};

#endif // STATISTICSPAGEMANAGER_H
