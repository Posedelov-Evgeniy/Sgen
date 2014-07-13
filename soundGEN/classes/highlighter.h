#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QtGui>
#include <QSyntaxHighlighter>
#include "./utextblockdata.h"

enum UBrackets { NoBrackets, RoundBrackets, CurlyBraces, SquareBrackets };

class Highlighter : public QSyntaxHighlighter
{
public:
    explicit Highlighter(QTextDocument *parent);
protected:
    struct HighlightingRule
    {
        HighlightingRule() {}
        HighlightingRule(QRegExp _pattern,QTextCharFormat _format) {pattern = _pattern;format = _format;}
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;
    QTextCharFormat keywordFormat, classFormat, singleLineCommentFormat, multiLineCommentFormat, quotationFormat, functionFormat, numberFormat;

    virtual void highlightBlock(const QString &text);
    void insertBrackets(QChar leftChar, QChar rightChar, UTextBlockData *data, QString text);
    void insertBrackets(UBrackets brackets, UTextBlockData *data, QString text);
};

#endif // HIGHLIGHTER_H
