#include "utextblockdata.h"

QVector<UBracketInfo *> UTextBlockData::brackets()
{
    return m_brackets;
}

void UTextBlockData::insert(UBracketInfo *info)
{
    int i = 0;

    while(i < m_brackets.size()
          && info->position > m_brackets.at(i)->position)
        i++;

    m_brackets.insert(i, info);
}
