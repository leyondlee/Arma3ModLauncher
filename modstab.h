#ifndef MODSTAB_H
#define MODSTAB_H

#include <QObject>
#include <QMainWindow>
#include <QTreeWidget>
#include <QDirIterator>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>

#include "util.h"
#include "settings.h"
#include "customtreewidget.h"

class ModsTab : public QObject
{
    Q_OBJECT

public:
    explicit ModsTab(QMainWindow *, Settings *);
    void loadAvailableMods();

private:
    Settings *settings;

    QMainWindow *mainWindow;
    CustomTreeWidget *availableModsTreeWidget;
    CustomTreeWidget *modGroupsTreeWidget;

    void init();
    void modGroupsTreeWidgetDragEnterEventSignal(QDragEnterEvent *);
    void modGroupsTreeWidgetDragMoveEventSignal(QDragMoveEvent *);
    void modGroupsTreeWidgetDropEventSignal(QDropEvent *);
    void modGroupsTreeWidgetCustomContextMenuRequested(QPoint);
    void modGroupsTreeAddFolderActionTriggered(bool);

signals:
};

#endif // MODSTAB_H
