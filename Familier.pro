#-------------------------------------------------
#
# Project created by QtCreator 2010-10-17T13:07:50
#
#-------------------------------------------------

QT       += core gui

TARGET = Familier2
TEMPLATE = app


SOURCES += src/main.cpp\
	src/MainWindow/MainWindow.cpp \
    src/Models/TreeModel.cpp \
    src/Models/TreeItem.cpp \
    src/Models/TableModel.cpp \
    src/PrimaryDataTypes/Union.cpp \
    src/PrimaryDataTypes/Timed.cpp \
    src/PrimaryDataTypes/MaleIndividual.cpp \
    src/PrimaryDataTypes/Individual.cpp \
    src/PrimaryDataTypes/IdableWithChildren.cpp \
    src/PrimaryDataTypes/Idable.cpp \
    src/PrimaryDataTypes/FemaleIndividual.cpp \
    src/PrimaryDataTypes/Family.cpp \
    src/PrimaryDataTypes/Familier.cpp \
    src/PrimaryDataTypes/Event.cpp \
    src/PrimaryDataTypes/Dated.cpp \
    src/Views/PieView.cpp \
    src/MainWindow/FileMenuHandler.cpp \
    src/MainWindow/EditMenuHandler.cpp \
    src/MainWindow/NavigateMenuHandler.cpp \
    src/MainWindow/ViewMenuHandler.cpp \
    src/MainWindow/HelpMenuHandler.cpp \
    src/EditorDialogs/EditIndividualDialog.cpp \
    src/EditorDialogs/EditFamilyDialog.cpp \
    src/Delegates/CompletionDelegate.cpp \
    src/Delegates/DateEditorDelegate.cpp \
    src/EditorDialogs/EditDialog.cpp \
    src/MainWindow/SettingsManager.cpp \
    src/MainWindow/ActionWithIdentity.cpp \
    src/MainWindow/StatusBarManager.cpp \
    src/Models/IconModel.cpp \
    src/MainWindow/ItemViewManager.cpp \
    src/MainWindow/ToolBarAndMenuHandler.cpp \
    src/Models/StringListModel.cpp \
    src/Models/EventModelForComboBox.cpp \
    src/EditorDialogs/DocumentTitleDialog.cpp \
    src/Models/GeneralEventModel.cpp \
    src/Models/EventModel.cpp \
    src/MainWindow/EventPageManager.cpp \
    src/Delegates/TableViewDelegate.cpp \
    src/Models/EventMembersModelForTableView.cpp \
    src/EditorDialogs/RichTextEditor.cpp \
    src/Models/FamilyModel.cpp \
    src/Delegates/RichTextEditorDelegate.cpp \
    src/MainWindow/StatisticsPageManager.cpp \
    src/Views/FamilyTreeView.cpp \
    src/Models/FamilyTreeModel.cpp \
    src/Views/EventTimeLineView.cpp \
    src/Models/EventLineModel.cpp \
    src/Widgets/TextEditWithAutoComplete.cpp \
    src/Widgets/MovingTextLabel.cpp \
    src/MainWindow/Application.cpp \
    src/PrimaryDataTypes/Utilities.cpp \
    src/MainWindow/MainPreferences.cpp

HEADERS  += src/MainWindow/MainWindow.h \
    src/Models/TreeModel.h \
    src/Models/TreeItem.h \
    src/Models/TableModel.h \
    src/PrimaryDataTypes/Union.h \
    src/PrimaryDataTypes/Timed.h \
    src/PrimaryDataTypes/MaleIndividual.h \
    src/PrimaryDataTypes/Individual.h \
    src/PrimaryDataTypes/IdableWithChildren.h \
    src/PrimaryDataTypes/Idable.h \
    src/PrimaryDataTypes/FemaleIndividual.h \
    src/PrimaryDataTypes/Family.h \
    src/PrimaryDataTypes/Familier.h \
    src/PrimaryDataTypes/Event.h \
    src/PrimaryDataTypes/Dated.h \
    src/PrimaryDataTypes/Common.h \
    src/Views/PieView.h \
    src/MainWindow/FileMenuHandler.h \
    src/MainWindow/EditMenuHandler.h \
    src/MainWindow/NavigateMenuHandler.h \
    src/MainWindow/ViewMenuHandler.h \
    src/MainWindow/HelpMenuHandler.h \
    src/EditorDialogs/EditIndividualDialog.h \
    src/EditorDialogs/EditFamilyDialog.h \
    src/Delegates/CompletionDelegate.h \
    src/Delegates/DateEditorDelegate.h \
    src/EditorDialogs/EditDialog.h \
    src/MainWindow/SettingsManager.h \
    src/MainWindow/ActionWithIdentity.h \
    src/MainWindow/StatusBarManager.h \
    src/Models/IconModel.h \
    src/MainWindow/ItemViewManager.h \
    src/MainWindow/ToolBarAndMenuHandler.h \
    src/Models/StringListModel.h \
    src/Models/EventModelForComboBox.h \
    src/EditorDialogs/DocumentTitleDialog.h \
    src/Models/GeneralEventModel.h \
    src/Models/EventModel.h \
    src/MainWindow/EventPageManager.h \
    src/Delegates/TableViewDelegate.h \
    src/Models/EventMembersModelForTableView.h \
    src/EditorDialogs/RichTextEditor.h \
    src/Models/FamilyModel.h \
    src/Delegates/RichTextEditorDelegate.h \
    src/MainWindow/StatisticsPageManager.h \
    src/Views/FamilyTreeView.h \
    src/Models/FamilyTreeModel.h \
    src/Views/EventTimeLineView.h \
    src/Models/EventLineModel.h \
    src/versiondefs.h \
    src/Widgets/TextEditWithAutoComplete.h \
    src/Widgets/MovingTextLabel.h \
    src/MainWindow/Application.h \
    src/PrimaryDataTypes/Utilities.h \
    src/MainWindow/MainPreferences.h

FORMS    += src/MainWindow/MainWindow.ui \
    src/EditorDialogs/EditIndividual.ui \
    src/EditorDialogs/EditFamily.ui \
    src/EditorDialogs/DocumentTitleDialog.ui \
    src/EditorDialogs/RichTextEditor.ui \
    src/MainWindow/MainPreferences.ui

RESOURCES += src/MainWindow/MainWindow.qrc \
    src/EditorDialogs/RichTextEditor.qrc

INCLUDEPATH += src \
    src/MainWindow \
    src/PrimaryDataTypes \
    src/Models \
    src/Views \
    src/EditorDialogs \
    src/Delegates \
    src/Widgets

QT += xml

unix {
    CONFIG += debug \
	warn_on
    CONFIG -= release
    DESTDIR = bin/unix

    # MAKEFILE = Makefile.unix
    BUILDDIR = build/unix
    MOC_DIR = $$BUILDDIR/moc
    UI_DIR = $$BUILDDIR/ui
    OBJECTS_DIR = $$BUILDDIR/obj
    RCC_DIR = $$BUILDDIR/rcc
}

win32 {
    CONFIG += release \
	debug \
	warn_on
    DESTDIR = bin/windows

    # MAKEFILE = Makefile.win
    BUILDDIR = build/windows
    MOC_DIR = $$BUILDDIR/moc
    UI_DIR = $$BUILDDIR/ui
    OBJECTS_DIR = $$BUILDDIR/obj
    RCC_DIR = $$BUILDDIR/rcc
}

win32 {
    BUILDSCRIPTSDIR=.\\buildscripts

    CONFIG += windows
    RC_FILE += rc/appinfo.rc
}

unix { \
    buildinfo.commands = echo \
	-ne \
	"'\\n\\nSource code lines/words/characters that will be built are: '" \
	&& \
	cat \
	$$SOURCES \
	$$HEADERS \
	$$FORMS \
	$$RESOURCES \
	| \
	wc \
	-lwc \
	&& \
	echo \
	&& \
	echo

	QMAKE_EXTRA_TARGETS += buildinfo

	PREFIX = $$(PREFIX)
	isEmpty( $$(PREFIX) ):PREFIX = /usr
	target.path = $$PREFIX/bin
	INSTALLS += target

	icons.path = $$(PREFIX)/share/pixmaps
	icons.files = redist/icons/*.*

#	PRE_TARGETDEPS += buildinfo
}

OTHER_FILES += \
    COPYING \
    AUTHORS \
    INSTALL \
    README \
    ChangeLog
