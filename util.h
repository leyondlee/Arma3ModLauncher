#ifndef UTIL_H
#define UTIL_H

#include <QStringList>
#include <QDir>
#include <QTreeWidgetItem>
#include <QListWidget>
#include <QMessageBox>

class Util
{
public:
    Util() = delete;

    static QString cleanPath(QString path);
    static QString joinPaths(QStringList paths);
    static QString getFilename(QString path);
    static bool hasItemInTreeWidgetItem(QTreeWidgetItem *item, QString text, QVariant data, int column);
    static void removeTreeWidgetItem(QTreeWidgetItem *item);
    static bool hasItemInListWidget(QListWidget *listWidget, QString text, QVariant data);
    static void showWarningMessage(QString title, QString message, QWidget *parent = nullptr);
    static QTreeWidgetItem *getItemInTreeWithData(QTreeWidget *treeWidget, QVariant data);
};

#endif // UTIL_H
