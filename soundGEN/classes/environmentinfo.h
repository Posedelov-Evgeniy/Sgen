#ifndef ENVIRONMENTINFO_H
#define ENVIRONMENTINFO_H

#include <QApplication>
#include <QProcess>
#include <QDir>

class EnvironmentInfo
{
private:
public:
    static QString getConfigsPath();
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
    static QString getVCPath();
    #endif;
};

#endif // ENVIRONMENTINFO_H
