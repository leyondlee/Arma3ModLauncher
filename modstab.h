#ifndef MODSTAB_H
#define MODSTAB_H

#include <QObject>
#include <QMainWindow>
#include <QTreeWidget>
#include <QDirIterator>
#include <QMenu>
#include <QInputDialog>
#include <QApplication>
#include <QCheckBox>
#include <QPushButton>

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

    void refreshTab();

private:
    Settings *settings;
    AvailableModsTreeWidget *availableModsTreeWidget;
    ModGroupsTreeWidget *modGroupsTreeWidget;
    QCheckBox *availableModsExpandAllCheckBox;
    QCheckBox *modGroupsExpandAllCheckBox;
    QCheckBox *modGroupsSelectAllCheckBox;
    QPushButton *refreshModsPushButton;

    void init();
    void loadAvailableMods();
    void loadModGroups();
    void modGroupsTreeChangedHandler();
    void modGroupsTreeItemCheckStateChangedHandler(ModGroupsTreeWidgetItem *item);
    void availableModsExpandAllCheckBoxStateChanged(int state);
    void modGroupsExpandAllCheckBoxStateChanged(int state);
    void modGroupsSelectAllCheckBoxStateChanged(int state);
    void refreshPushButtonClicked(bool checked);
    void availableModsTreeWidgetItemCollapsed(QTreeWidgetItem *item);
    void availableModsTreeWidgetItemExpanded(QTreeWidgetItem *item);
    void modGroupsTreeWidgetItemCollapsed(QTreeWidgetItem *item);
    void modGroupsTreeWidgetItemExpanded(QTreeWidgetItem *item);

signals:
};

#endif // MODSTAB_H
