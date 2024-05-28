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

    void setAvailableModsTreeWidget(AvailableModsTreeWidget *availableModsTreeWidget);
    ModGroupsTreeWidgetItem *addFolder(QString name);

private:
    AvailableModsTreeWidget *availableModsTreeWidget;

    void customContextMenuRequestedHandler(QPoint pos);
    void addFolderActionTriggered(bool checked);
    void deleteActionTriggered(bool checked);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void updateSignal();
};

#endif // MODGROUPSTREEWIDGET_H
