#ifndef MODGROUPSTREEWIDGETITEM_H
#define MODGROUPSTREEWIDGETITEM_H

#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QApplication>

class ModGroupsTreeWidgetItem : public QTreeWidgetItem
{
public:
    ModGroupsTreeWidgetItem(QString name, QVariant data, bool isFolder = false);

    bool isFolder();

private:
    bool folder;

    void init(QString name, QVariant data, bool isFolder);
};

#endif // MODGROUPSTREEWIDGETITEM_H
