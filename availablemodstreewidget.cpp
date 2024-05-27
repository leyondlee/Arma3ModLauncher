#include "availablemodstreewidget.h"

AvailableModsTreeWidget::AvailableModsTreeWidget(QWidget *parent)
    : ModsTabTreeWidget{Qt::CopyAction, parent}
{

}

bool AvailableModsTreeWidget::isItemDraggable(QTreeWidgetItem *item)
{
    if (item == nullptr) {
        return false;
    }

    return (this->indexOfTopLevelItem(item) == -1);
}
