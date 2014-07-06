#ifndef UTEXTEDIT_H
#define UTEXTEDIT_H

#include <QTextEdit>
#include "classes/utextblockdata.h"
#include "classes/highlighter.h"

class UTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit UTextEdit(QWidget *parent = 0);
    ~UTextEdit();

    bool matchLeftBrackets(QTextBlock currentBlock, int index, int numberLeftBracket);
    bool matchRightBrackets(QTextBlock currentBlock, int index, int numberRightBracket);
    void createBracketsSelection(int position);
protected:
    Highlighter *base_highlighter;
signals:

public slots:

protected slots:
    void matchBrackets();
};

#endif // UTEXTEDIT_H
