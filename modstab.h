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
    explicit ModsTab(QWidget *, Settings *);
    void loadAvailableMods();

private:
    Settings *settings;
    AvailableModsTreeWidget *availableModsTreeWidget;
    ModGroupsTreeWidget *modGroupsTreeWidget;

    void init();

signals:
};

#endif // MODSTAB_H
