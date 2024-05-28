#ifndef MODSTAB_H
#define MODSTAB_H

#include <QObject>
#include <QMainWindow>
#include <QTreeWidget>
#include <QDirIterator>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QApplication>

#include "util.h"
#include "settings.h"
#include "availablemodstreewidget.h"
#include "modgroupstreewidget.h"
#include "modgroupstreewidgetitem.h"

class ModsTab : public QObject
{
    Q_OBJECT

public:
    explicit ModsTab(QWidget *modsTab, Settings *settings);

private:
    Settings *settings;
    AvailableModsTreeWidget *availableModsTreeWidget;
    ModGroupsTreeWidget *modGroupsTreeWidget;

    void init();
    void loadAvailableMods();
    void loadModGroups();
    void modGroupsTreeChangedHandler();
    void modGroupsTreeItemCheckStateChangedHandler(ModGroupsTreeWidgetItem *item);

signals:
};

#endif // MODSTAB_H
