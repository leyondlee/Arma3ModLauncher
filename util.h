#ifndef UTIL_H
#define UTIL_H

#include <QStringList>
#include <QDir>

class Util
{
public:
    Util() = delete;

    static QString joinPaths(QStringList);
    static QString getFilename(QString);
};

#endif // UTIL_H
