#include "util.h"

QString Util::joinPaths(QStringList paths)
{
    return QDir::toNativeSeparators(QDir::cleanPath(paths.join(QDir::separator())));
}

QString Util::getFilename(QString path)
{
    return QFileInfo(path).fileName();
}
