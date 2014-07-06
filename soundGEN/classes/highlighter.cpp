#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b" << "\\breturn\\b"
                    << "\\bM_PI\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    numberFormat.setFontWeight(QFont::Bold);
    numberFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\-?\\d+(\\.\\d{0,})?");
    rule.format = numberFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::gray);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::gray);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    UTextBlockData *data = new UTextBlockData();

    /// скобки
    insertBrackets(RoundBrackets, data, text);
    insertBrackets(CurlyBraces, data, text);
    insertBrackets(SquareBrackets, data, text);
    /// скобки

    setCurrentBlockUserData(data);

    if(commentStartExpression.isEmpty() || commentEndExpression.isEmpty()) return;

    setCurrentBlockState(0);

    int startIndex = 0;
    if(previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0)
    {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;

        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }else
            commentLength = endIndex - startIndex
                    + commentEndExpression.matchedLength();

        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

void Highlighter::insertBrackets(QChar leftChar, QChar rightChar, UTextBlockData *data, QString text)
{
    int leftPosition = text.indexOf(leftChar);

    while(leftPosition != -1)
    {
        UBracketInfo *info = new UBracketInfo();
        info->character = leftChar;
        info->position = leftPosition;

        data->insert(info);
        leftPosition = text.indexOf(leftChar, leftPosition + 1);
    }


    int rightPosition = text.indexOf(rightChar);

    while(rightPosition != -1)
    {
        UBracketInfo *info = new UBracketInfo();
        info->character = rightChar;
        info->position = rightPosition;

        data->insert(info);
        rightPosition = text.indexOf(rightChar, rightPosition + 1);
    }
}

void Highlighter::insertBrackets(UBrackets brackets, UTextBlockData *data, QString text)
{
    QChar leftChar = '0';
    QChar rightChar = '0';

    switch(brackets)
    {
    case RoundBrackets:
        leftChar = '('; rightChar = ')';
        break;

    case CurlyBraces:
        leftChar = '{'; rightChar = '}';
        break;

    case SquareBrackets:
        leftChar = '['; rightChar = ']';
        break;
    }

    insertBrackets(leftChar, rightChar, data, text);
}
