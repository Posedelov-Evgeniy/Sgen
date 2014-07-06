#ifndef UTEXTBLOCKDATA_H
#define UTEXTBLOCKDATA_H

#include <QTextBlockUserData>

struct UBracketInfo
{
    QChar character;
    int position;
};

static bool isLeftBrackets(QChar symbol)
{
    return symbol == '('
            || symbol == '{'
            || symbol == '[';
}

static bool isRightBrackets(QChar symbol)
{
    return symbol == ')'
            || symbol == '}'
            || symbol == ']';
}

class UTextBlockData: public QTextBlockUserData
{
public:
    QVector <UBracketInfo *> brackets();
    void insert(UBracketInfo *info);
private:
    QVector <UBracketInfo *> m_brackets;
};

#endif // UTEXTBLOCKDATA_H
