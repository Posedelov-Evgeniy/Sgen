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
            if (bracket->position == currentPosition
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

bool UTextEdit::bracketIsPaired(QTextBlock textBlock, int currentPosition) {
    QList <QTextEdit::ExtraSelection> selections;
    setExtraSelections(selections);

    UTextBlockData *data = static_cast <UTextBlockData *> (textBlock.userData());

    if(data)
    {
        QVector <UBracketInfo *> brackets = data->brackets();

        for(int i = 0; i < brackets.size(); i++)
        {
            UBracketInfo *bracket = brackets.at(i);

            if (bracket->position == currentPosition && isLeftBrackets(bracket->character)) {
                if (matchLeftBrackets(textBlock, i + 1, 0)) return true;
            }
            if (bracket->position == currentPosition && isRightBrackets(bracket->character)) {
                if (matchRightBrackets(textBlock, i - 1, 0)) return true;
            }
        }
    }
    return false;
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

        if (isRightBrackets(bracket->character) && numberLeftBracket == 0)
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

        if (isLeftBrackets(bracket->character) && numberRightBracket == 0)
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
        cur.insertText(str);
        cur.setPosition(a);
        cur.setPosition(str.length(), QTextCursor::KeepAnchor);

        setTextCursor(cur);
    }
}

void UTextEdit::completeReturn()
{
    QTextCursor cur = textCursor();
    int a = cur.anchor();
    int p = cur.position();

    if (a!=p) {
        cur.insertText("");
        cur.setPosition(a);
        setTextCursor(cur);
    }

    cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::KeepAnchor);
    QString str = cur.selection().toPlainText();
    QTextBlock curBlock = cur.block();
    QString result="\n";
    QString dop_result="";

    if (str.length()>0) {
        int fst_s;
        int lst_s;
        for(fst_s=0;fst_s<str.length() && str.at(fst_s).isSpace();fst_s++);
        for(lst_s=str.length()-1;lst_s>0 && str.at(lst_s).isSpace();lst_s--);

        QChar last_c = str.at(lst_s);
        cur.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
        QString endofln = cur.selection().toPlainText().trimmed();

        if (isLeftBrackets(last_c)) {
            QChar close_c = (last_c=='(' ? ')' : (last_c=='{' ? '}' : ']'));
            if (!bracketIsPaired(curBlock, lst_s))
            {
                cur.removeSelectedText();
                dop_result.fill(' ', fst_s);
                dop_result=endofln+"\n"+dop_result+close_c;
                endofln = "";
            }
            fst_s += 4;
        }

        if (endofln.length()>0 && isRightBrackets(endofln.at(0)) && bracketIsPaired(curBlock, a-curBlock.position()+cur.selection().toPlainText().indexOf(endofln.at(0)))) {
            fst_s = fst_s>=4 ? fst_s-4 : 0;
        }

        result.fill(' ', fst_s);
        result="\n"+result+dop_result;
    }

    insertPlainText(result);

    if (!dop_result.isEmpty()) {
        cur.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor);
        cur.movePosition(QTextCursor::EndOfBlock,QTextCursor::MoveAnchor);
        setTextCursor(cur);
    }
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
    if (e->key()==(Qt::Key_Control & Qt::Key_Z)) {
        undo();
        return;
    }
    QTextEdit::keyPressEvent(e);
}
