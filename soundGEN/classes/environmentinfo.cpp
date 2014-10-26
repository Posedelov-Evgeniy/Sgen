#include "environmentinfo.h"

QString EnvironmentInfo::getHomePath()
{
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
        QString bfile = "";
    #else
        #if defined(ANDROID) || defined(__ANDROID__)
        QString bfile = getConfigsPath();
        #else
        QString bfile = "/home";
        #endif
    #endif
    return bfile;
}

QString EnvironmentInfo::getConfigsPath()
{
    #if !defined(ANDROID) && !defined(__ANDROID__)
        return QCoreApplication::instance()->applicationDirPath();
    #else
        return "/sdcard/soundGEN/";
    #endif;
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
QString EnvironmentInfo::getVCPath()
{
    QString result = "";
    QStringList env_list(QProcess::systemEnvironment());
    int idx = env_list.indexOf(QRegExp("^VS110COMNTOOLS=.*", Qt::CaseInsensitive));
    if (idx > -1)
    {
        QStringList windir = env_list[idx].split('=');
        QDir rdir(windir[1]);
        rdir.cdUp();
        rdir.cdUp();
        rdir.cd("VC");
        result = rdir.absolutePath();
    }
    return result;
}
#endif;
