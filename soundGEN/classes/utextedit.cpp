#include "utextedit.h"

UTextEdit::UTextEdit(QWidget *parent):
    QTextEdit(parent)
{
    connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(matchBrackets()));
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
