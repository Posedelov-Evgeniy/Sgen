#include "utextedit.h"

UTextEdit::UTextEdit(QWidget *parent):
    QTextEdit(parent)
{
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    setFont(font);
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(matchBrackets()));
    base_highlighter = new Highlighter(this->document());
}

UTextEdit::~UTextEdit()
{
    delete base_highlighter;
}

void UTextEdit::matchBrackets()
{
    QList <QTextEdit::ExtraSelection> selections;
    setExtraSelections(selections);

    QTextBlock textBlock = textCursor().block();

    UTextBlockData *data = static_cast <UTextBlockData *> (textBlock.userData());

    if(data)
    {
        QVector <UBracketInfo *> brackets = data->brackets();
        int position = textCursor().block().position();

        for(int i = 0; i < brackets.size(); i++)
        {
            UBracketInfo *bracket = brackets.at(i);
            int currentPosition = textCursor().position() - textBlock.position();

            // Clicked on a left brackets?
            if (bracket->position == currentPosition - 1
                 && isLeftBrackets(bracket->character))
            {
                if (matchLeftBrackets(textBlock, i + 1, 0))
                    createBracketsSelection(position + bracket->position);
            }

            // Clicked on a right brackets?
            if (bracket->position == currentPosition - 1
                 && isRightBrackets(bracket->character))
            {
                if (matchRightBrackets(textBlock, i - 1, 0))
                    createBracketsSelection(position + bracket->position);
            }
        }
    }
}

/** Test left brackets match **/
bool UTextEdit::matchLeftBrackets(QTextBlock currentBlock, int index, int numberLeftBracket)
{
    UTextBlockData *data = static_cast <UTextBlockData *> (currentBlock.userData());

    QVector<UBracketInfo *> brackets = data->brackets();

    int positionInDocument = currentBlock.position();

    // Match in same line?
    for (; index < brackets.count(); index++)
    {
        UBracketInfo *bracket = brackets.at(index);

        if (isLeftBrackets(bracket->character))
        {
            ++numberLeftBracket;
            continue;
        }

        if (isRightBrackets(bracket->character)
             && numberLeftBracket == 0)
        {
            createBracketsSelection(positionInDocument + bracket->position);
            return true;
        }else
            --numberLeftBracket;
    }

    // No match yet? Then try next block
    currentBlock = currentBlock.next();
    if (currentBlock.isValid())
        return matchLeftBrackets(currentBlock, 0, numberLeftBracket);

    // No match at all
    return false;
}

/** Test right brackets match **/
bool UTextEdit::matchRightBrackets(QTextBlock currentBlock, int index, int numberRightBracket)
{
    UTextBlockData *data = static_cast <UTextBlockData *> (currentBlock.userData());

    QVector<UBracketInfo *> brackets = data->brackets();
    int positionInDocument = currentBlock.position();

    // Match in same line?
    for (int i = index; i >= 0; i--)
    {
        UBracketInfo *bracket = brackets.at(i);

        if (isRightBrackets(bracket->character))
        {
            ++numberRightBracket;
            continue;
        }

        if (isLeftBrackets(bracket->character)
             && numberRightBracket == 0)
        {
            createBracketsSelection(positionInDocument + bracket->position);
            return true;
        } else
            --numberRightBracket;
    }

    // No match yet? Then try previous block
    currentBlock = currentBlock.previous();
    if (currentBlock.isValid())
    {

        // Recalculate correct index first
        UTextBlockData *data = static_cast <UTextBlockData *> (currentBlock.userData());

        QVector <UBracketInfo *> brackets = data->brackets();

        return matchRightBrackets(currentBlock, brackets.count() - 1, numberRightBracket);
    }

    // No match at all
    return false;
}

/** Set brackets highlighter at pos **/
void UTextEdit::createBracketsSelection(int position)
{
    if (!hasFocus()) return;
    QList <QTextEdit::ExtraSelection> listSelections = extraSelections();

    QTextEdit::ExtraSelection selection;

    QTextCharFormat format = selection.format;
    format.setForeground(Qt::red);
    format.setBackground(QBrush(Qt::green));
    format.setFontWeight(QFont::Bold);
    selection.format = format;

    QTextCursor cursor = textCursor();
    cursor.setPosition(position);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

    selection.cursor = cursor;

    listSelections.append(selection);

    setExtraSelections(listSelections);
}

void UTextEdit::insertFromMimeData(const QMimeData * source)
{
    insertPlainText(source->text());
}

void UTextEdit::focusInEvent(QFocusEvent *e)
{
    QTextEdit::focusInEvent(e);
    matchBrackets();
}

void UTextEdit::focusOutEvent(QFocusEvent *e)
{
    QTextEdit::focusOutEvent(e);
    matchBrackets();
}

void UTextEdit::selectedShift(bool reversed)
{
    QTextCursor cur = textCursor();
    int a = cur.anchor();
    int p = cur.position();
    if (a>p) {
        int tmp = a;
        a = p;
        p = tmp;
    }

    cur.setPosition(a);
    cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
    a = cur.position();
    cur.setPosition(a);
    cur.setPosition(p, QTextCursor::KeepAnchor);
    QString str = cur.selection().toPlainText();
    QStringList list = str.split("\n");

    if (list.count()<=1)
    {
        insertPlainText("    ");
    } else
    {
        for (int i = 0; i < list.count(); i++)
        {
            if (reversed) {
                list[i].replace(QRegExp("^(\\s){,4}"), "");
            } else {
                list[i].insert(0,"    ");
            }
        }

        str=list.join("\n");
        cur.removeSelectedText();
        cur.insertText(str);
        cur.setPosition(a);
        cur.setPosition(str.length(), QTextCursor::KeepAnchor);

        setTextCursor(cur);
    }
}

void UTextEdit::completeReturn()
{
    insertPlainText("\n");
}

bool UTextEdit::event(QEvent *e)
{
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*> (e);
        if (keyEvent->key() == Qt::Key_Tab) {
            selectedShift(false);
            return true;
        } else if(keyEvent->key() == Qt::Key_Backtab) {
            selectedShift(true);
            return true;
        } else if(keyEvent->key() == Qt::Key_Return) {
            completeReturn();
            return true;
        }
    }
    return QTextEdit::event(e);
}

void UTextEdit::keyPressEvent(QKeyEvent *e)
{
    QTextEdit::keyPressEvent(e);
}
