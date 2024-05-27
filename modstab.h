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
#include <QMimeData>

#include "util.h"
#include "settings.h"
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
    QTreeWidget *availableModsTreeWidget;
    ModGroupsTreeWidget *modGroupsTreeWidget;

    void init();
    bool hasItemInTreeWidget(QTreeWidget *treeWidget, QString text, QVariant data, int column);
    bool hasItemInTreeWidgetItem(QTreeWidgetItem *item, QString text, QVariant data, int column);
    void removeTreeWidgetItem(QTreeWidgetItem *);

    void modGroupsDragEnterSignalHandler(QDragEnterEvent *);
    void modGroupsDragLeaveSignalHandler(QDragLeaveEvent *);
    void modGroupsDragMoveSignalHandler(QDragMoveEvent *);
    void modGroupsDropSignalHandler(QDropEvent *);
    void modGroupsCustomContextMenuRequestedHandler(QPoint);
    void modGroupsTreeAddFolderActionTriggered(bool);

signals:
};

#endif // MODSTAB_H
