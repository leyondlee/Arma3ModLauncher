#ifndef MODGROUPSTREEWIDGETITEM_H
#define MODGROUPSTREEWIDGETITEM_H

#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QApplication>

#include "util.h"

class ModGroupsTreeWidgetItem : public QTreeWidgetItem
{
public:
    ModGroupsTreeWidgetItem(QString name, QVariant data, bool isFolder = false);

    bool isFolder();
    ModGroupsTreeWidgetItem *addChildModItem(QString path);
    void setMissing(bool isMissing);
    static ModGroupsTreeWidgetItem *castTreeWidgetItem(QTreeWidgetItem *item);
    void setCheckState(Qt::CheckState state);
    bool hasCheckStateChanged();
    void updateSavedCheckState();

private:
    bool folder;
    bool isMissing;
    Qt::CheckState savedCheckState;
};

#endif // MODGROUPSTREEWIDGETITEM_H
