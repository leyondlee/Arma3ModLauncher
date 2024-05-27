#ifndef MODGROUPSTREEWIDGET_H
#define MODGROUPSTREEWIDGET_H

#include <QTreeWidget>
#include <QDragEnterEvent>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>

#include "modstabtreewidget.h"
#include "availablemodstreewidget.h"
#include "modgroupstreewidgetitem.h"
#include "util.h"

class ModGroupsTreeWidget : public ModsTabTreeWidget
{
    Q_OBJECT

public:
    explicit ModGroupsTreeWidget(QWidget *parent = nullptr);

    void setAvailableModsTreeWidget(AvailableModsTreeWidget *);

private:
    AvailableModsTreeWidget *availableModsTreeWidget;

    void doSort();
    bool hasItem(QString text, QVariant data, int column);
    void customContextMenuRequestedHandler(QPoint pos);
    void addFolderActionTriggered(bool);

protected:
    bool isItemDraggable(QTreeWidgetItem *item) override;
    void dragEnterEvent(QDragEnterEvent *) override;
    void dragLeaveEvent(QDragLeaveEvent *) override;
    void dragMoveEvent(QDragMoveEvent *) override;
    void dropEvent(QDropEvent *) override;
};

#endif // MODGROUPSTREEWIDGET_H
