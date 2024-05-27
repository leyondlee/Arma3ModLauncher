#ifndef UTIL_H
#define UTIL_H

#include <QStringList>
#include <QDir>
#include <QTreeWidgetItem>

class Util
{
public:
    Util() = delete;

    static QString joinPaths(QStringList);
    static QString getFilename(QString);
    static bool hasItemInTreeWidgetItem(QTreeWidgetItem *item, QString text, QVariant data, int column);
    static void removeTreeWidgetItem(QTreeWidgetItem *item);
};

#endif // UTIL_H
