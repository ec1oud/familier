#include "TextEditWithAutoComplete.h"
#include <QShortcut>
#include <QKeySequence>
#include <QScrollBar>
#include <QAbstractItemView>
#include <QKeyEvent>
#include "Individual.h"
#include "Family.h"

TextEditWithAutoComplete::TextEditWithAutoComplete(QWidget *parent) :
		QTextEdit(parent), _isCompleteAndSelected (false),
		_individualCompleter (NULL), _familyCompleter (NULL)
{
}

void TextEditWithAutoComplete::createCompleter (IconModel *iconModel, FamilyModel *familyModel)
{
	_individualCompleter = new QCompleter (this);
	_individualCompleter->setWidget (this);
	_individualCompleter->setCompletionMode (QCompleter::PopupCompletion);
	_individualCompleter->setModel (iconModel);
	_individualCompleter->setCompletionRole (Qt::DisplayRole);
	_individualCompleter->setModelSorting (QCompleter::CaseInsensitivelySortedModel);
	_individualCompleter->setCaseSensitivity (Qt::CaseInsensitive);
	_individualCompleter->setWrapAround (true);

	_familyCompleter = new QCompleter (this);
	_familyCompleter->setWidget (this);
	_familyCompleter->setCompletionMode (QCompleter::PopupCompletion);
	_familyCompleter->setModel (familyModel);
	_familyCompleter->setCompletionRole (Qt::DisplayRole);
	_familyCompleter->setModelSorting (QCompleter::UnsortedModel);
	_familyCompleter->setCaseSensitivity (Qt::CaseInsensitive);
	_familyCompleter->setWrapAround (true);

	connect (this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
	connect (_individualCompleter, SIGNAL(activated(QModelIndex)),
		 this, SLOT(insertIndividualCompletion(const QModelIndex &)));
	connect (_familyCompleter, SIGNAL(activated(QModelIndex)),
		 this, SLOT(insertFamilyCompletion(QModelIndex)));
	new QShortcut (QKeySequence (tr("Ctrl+space", "CompleteIndividual")),
		       this, SLOT(performIndividualCompletion()));
	new QShortcut (QKeySequence (tr("Ctrl+M", "CompleteFamily")),
		       this, SLOT(performFamilyCompletion()));
}

QString TextEditWithAutoComplete::toHtml () const
{
	QString ret = QTextEdit::toHtml ();

	QStringList anchorTypes;
	anchorTypes << "iid" << "fid";
	foreach (QString anchorType, anchorTypes)
	{
		QRegExp regExp ("<a href=\"" + anchorType + ":([0-9]+)\".*</a>");
		regExp.setMinimal (true);
		ret.replace (regExp, "[[" + anchorType + "=\\1]]");
	}

	return ret;
}

void TextEditWithAutoComplete::performCompletion (bool isIndividualCompletion)
{
	QCompleter *completer = isIndividualCompletion ? _individualCompleter : _familyCompleter;
	QTextCursor cursor = textCursor ();
	cursor.select (QTextCursor::WordUnderCursor);
	QString selectedText = cursor.selectedText ();
	if (!selectedText.isEmpty () && selectedText.at (selectedText.length () - 1).isLetter ())
	{
		if (selectedText != completer->completionPrefix ())
		{
			completer->setCompletionPrefix (selectedText);
			completer->popup ()->setCurrentIndex (completer->completionModel ()->index (0, 0));
		}

		if (completer->completionCount () == 1)
		{
			if (isIndividualCompletion)
				insertIndividualCompletion (completer->currentIndex (), true);
			else
				insertFamilyCompletion (completer->currentIndex (), true);
		}
		else
		{
			QRect rect = cursorRect ();
			rect.setWidth (completer->popup ()->sizeHintForColumn (0) +
				       completer->popup ()->verticalScrollBar ()->sizeHint ().width ());
			completer->complete (rect);
		}
	}
}

void TextEditWithAutoComplete::insertCompletion (bool isIndividualCompletion,
						 const QModelIndex &index, bool singleWord)
{
	QCompleter *completer;
	QString anchorType;
	if (isIndividualCompletion)
	{
		completer = _individualCompleter;
		anchorType = "iid";
	}
	else
	{
		completer = _familyCompleter;
		anchorType = "fid";
	}

	QString completion = index.data (completer->completionRole ()).toString ();

	QTextCursor cursor = textCursor ();
	int numberOfCharsToComplete = completion.length () -
				      completer->completionPrefix ().length();
	int insertionPosition = cursor.position ();
	cursor.insertText (completion.right (numberOfCharsToComplete));

	if (singleWord)
	{
		cursor.setPosition (insertionPosition - completer->completionPrefix ().length (),
				    QTextCursor::KeepAnchor);

		QTextCharFormat charFormat = cursor.charFormat ();
		charFormat.setAnchor (true);
		charFormat.setAnchorHref (QString ("%1:%2")
					  .arg (anchorType)
					  .arg (index.data (Qt::UserRole).toInt ()));
		charFormat.setToolTip (index.data (Qt::ToolTipRole).toString ());

		cursor.insertText (completion, charFormat);
		cursor.setPosition (cursor.position () - numberOfCharsToComplete);
		cursor.setPosition (cursor.position () + numberOfCharsToComplete, QTextCursor::KeepAnchor);
		_isCompleteAndSelected = true;
	}

	setTextCursor (cursor);
}

void TextEditWithAutoComplete::keyPressEvent (QKeyEvent *event)
{
	if (_isCompleteAndSelected && handledCompletedAndSelected (event))
		return;

	_isCompleteAndSelected = false;

	QList<QCompleter *> completerList;
	completerList << _individualCompleter << _familyCompleter;
	foreach (QCompleter *completer, completerList)
	{
		if (completer->popup ()->isVisible ())
		{
			switch (event->key ())
			{
			case Qt::Key_Up: // Fallthrough
			case Qt::Key_Down: // Fallthrough
			case Qt::Key_Enter: // Fallthrough
			case Qt::Key_Return: // Fallthrough
			case Qt::Key_Escape: event->ignore (); return;
			default:
				{
					QTextEdit::keyPressEvent (event);
					completer->popup ()->hide ();
					if (completer == _individualCompleter)
						performIndividualCompletion ();
					else
						performFamilyCompletion ();
					return;
				}
				break;
			}
		}
	}

	QTextEdit::keyPressEvent (event);
}

void TextEditWithAutoComplete::mousePressEvent(QMouseEvent *event)
{
	if (_isCompleteAndSelected)
	{
		_isCompleteAndSelected = false;

		QTextCursor cursor = textCursor ();
		cursor.removeSelectedText ();

		cursor.movePosition (QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
		QTextCharFormat textFormat = cursor.charFormat ();
		textFormat.setAnchor (false);
		cursor.setCharFormat (textFormat);
		cursor.movePosition (QTextCursor::EndOfWord);

		setTextCursor (cursor);
		event->accept ();
		return;
	}

	QTextEdit::mousePressEvent (event);
}

bool TextEditWithAutoComplete::handledCompletedAndSelected(QKeyEvent *event)
{
	_isCompleteAndSelected = false;
	QTextCursor cursor = textCursor ();

	switch (event->key ())
	{
	case Qt::Key_Left: // Fallthrough
	case Qt::Key_Enter: // Fallthrough
	case Qt::Key_Return:
		{
			cursor.clearSelection ();
			QTextCharFormat charFormat = cursor.charFormat ();
			charFormat.setAnchor (false);
			charFormat.setAnchorHref ("");
			cursor.setCharFormat (charFormat);
		}
		break;
	case Qt::Key_Escape:
		{
			cursor.removeSelectedText ();

			cursor.movePosition (QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
			QTextCharFormat textFormat = cursor.charFormat ();
			textFormat.setAnchor (false);
			cursor.setCharFormat (textFormat);
			cursor.movePosition (QTextCursor::EndOfWord);
		}
		break;
	default: return false;
	}

	setTextCursor (cursor);
	event->accept ();
	return true;
}

void TextEditWithAutoComplete::performIndividualCompletion ()
{
	performCompletion (true);
}

void TextEditWithAutoComplete::performFamilyCompletion ()
{
	performCompletion (false);
}

void TextEditWithAutoComplete::insertIndividualCompletion (const QModelIndex &index, bool singleWord)
{
	insertCompletion (true, index, singleWord);
}

void TextEditWithAutoComplete::insertFamilyCompletion (const QModelIndex &index, bool singleWord)
{
	insertCompletion (false, index, singleWord);
}

void TextEditWithAutoComplete::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;
	QTextEdit::ExtraSelection selection;
	QBrush highlightColor = palette ().alternateBase ();
	selection.format.setBackground (highlightColor);
	selection.format.setProperty (QTextFormat::FullWidthSelection, true);
	selection.cursor = textCursor ();
	selection.cursor.clearSelection ();
	extraSelections.append (selection);
	setExtraSelections (extraSelections);
}
