#include "modgroupstreewidget.h"

ModGroupsTreeWidget::ModGroupsTreeWidget(QWidget *parent)
    : ModsTabTreeWidget{Qt::MoveAction, parent}
{
    this->availableModsTreeWidget = nullptr;

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &ModGroupsTreeWidget::customContextMenuRequested, this, &ModGroupsTreeWidget::customContextMenuRequestedHandler);
}

void ModGroupsTreeWidget::setAvailableModsTreeWidget(AvailableModsTreeWidget *availableModsTreeWidget)
{
    this->availableModsTreeWidget = availableModsTreeWidget;
}

ModGroupsTreeWidgetItem *ModGroupsTreeWidget::addFolder(QString name)
{
    ModGroupsTreeWidgetItem *folderItem = new ModGroupsTreeWidgetItem(name, QVariant(name), true);
    this->addTopLevelItem(folderItem);
    this->doSort();

    return folderItem;
}

void ModGroupsTreeWidget::customContextMenuRequestedHandler(QPoint pos)
{
    QTreeWidgetItem *item = this->itemAt(pos);
    if (item == nullptr) {
        this->clearSelection();

        QMenu *menu = new QMenu(this);
        QAction *addFolderAction = new QAction("Add Folder", menu);
        menu->addAction(addFolderAction);
        connect(addFolderAction, &QAction::triggered, this, &ModGroupsTreeWidget::addFolderActionTriggered);
        menu->exec(this->mapToGlobal(pos));

        return;
    }

    item->setSelected(true);

    /*QList<QTreeWidgetItem *> selectedItems = this->selectedItems();
    for (auto selectedItem : selectedItems) {
        ModGroupsTreeWidgetItem *castedSelectedItem = dynamic_cast<ModGroupsTreeWidgetItem *>(selectedItem);
        if (castedSelectedItem == nullptr || !castedSelectedItem->isFolder()) {
            continue;
        }

        for (int i = 0; i < selectedItem->childCount(); i += 1) {
            QTreeWidgetItem *childItem = selectedItem->child(i);
            if (childItem == nullptr) {
                continue;
            }

            childItem->setSelected(true);
        }
    }*/

    QMenu *menu = new QMenu(this);
    QAction *deleteAction = new QAction("Delete", menu);
    menu->addAction(deleteAction);
    connect(deleteAction, &QAction::triggered, this, &ModGroupsTreeWidget::deleteActionTriggered);
    menu->exec(this->mapToGlobal(pos));
}

void ModGroupsTreeWidget::addFolderActionTriggered(bool checked)
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Add Folder"), tr("Name:"), QLineEdit::Normal, tr(""), &ok);
    if (!ok || name.isEmpty()) {
        return;
    }

    if (this->hasItem(name, QVariant(name), 0)) {
        QMessageBox messageBox(QMessageBox::Warning, "Warning", "Folder already exists.", QMessageBox::NoButton, this);
        messageBox.exec();
        return;
    }

    addFolder(name);

    emit updateSignal();
}

void ModGroupsTreeWidget::deleteActionTriggered(bool checked)
{
    QList<QTreeWidgetItem *> itemsToDelete;

    QList<QTreeWidgetItem *> selectedItems = this->selectedItems();
    for (auto item : selectedItems) {
        QTreeWidgetItem *parentItem = item->parent();
        if (parentItem != nullptr && selectedItems.contains(parentItem)) {
            continue;
        }

        itemsToDelete.append(item);
    }

    for (auto item : itemsToDelete) {
        Util::removeTreeWidgetItem(item);
    }

    emit updateSignal();
}

void ModGroupsTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QTreeWidget *sourceTreeWidget = qobject_cast<QTreeWidget *>(event->source());
    if (sourceTreeWidget == nullptr || (sourceTreeWidget != this && sourceTreeWidget != this->availableModsTreeWidget)) {
        return;
    }

    event->acceptProposedAction();
}

void ModGroupsTreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{

}

void ModGroupsTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QTreeWidgetItem *item = this->itemAt(event->position().toPoint());
    if (item != nullptr) {
        while (item->parent() != nullptr) {
            item = item->parent();
        }
    }

    bool hasSelectedItem = false;
    for (int i = 0; i < this->topLevelItemCount(); i += 1) {
        QTreeWidgetItem *topLevelItem = this->topLevelItem(i);

        ModGroupsTreeWidgetItem *castedtopLevelItem = dynamic_cast<ModGroupsTreeWidgetItem *>(topLevelItem);
        if (castedtopLevelItem == nullptr || !castedtopLevelItem->isFolder()) {
            continue;
        }

        if (topLevelItem == item) {
            hasSelectedItem = true;
            topLevelItem->setSelected(true);
            continue;
        }

        topLevelItem->setSelected(false);
    }

    if (!hasSelectedItem) {
        event->ignore();
        return;
    }

    event->setDropAction(event->dropAction());
    event->accept();
}

void ModGroupsTreeWidget::dropEvent(QDropEvent *event)
{
    QTreeWidget *sourceTreeWidget = qobject_cast<QTreeWidget *>(event->source());
    if (sourceTreeWidget == nullptr || (sourceTreeWidget != this->availableModsTreeWidget && sourceTreeWidget != this)) {
        event->ignore();
        return;
    }

    QTreeWidgetItem *item = this->itemAt(event->position().toPoint());
    if (item == nullptr) {
        event->ignore();
        return;
    }

    QTreeWidgetItem *targetItem = item;
    while (targetItem->parent() != nullptr) {
        targetItem = targetItem->parent();
    }

    ModGroupsTreeWidgetItem *targetItemCasted = dynamic_cast<ModGroupsTreeWidgetItem *>(targetItem);
    if (targetItemCasted == nullptr || !targetItemCasted->isFolder()) {
        event->ignore();
        return;
    }

    ModsTabDragDropData dragDropData;
    dragDropData.load(event->mimeData()->data(JSON_MIME));

    bool hasChanges = false;
    QList<int> keys = dragDropData.keys();
    for (auto key : keys) {
        QTreeWidgetItem *parentItem = sourceTreeWidget->topLevelItem(key);
        if (parentItem == nullptr || parentItem == targetItem) {
            continue;
        }

        QVariant variant = dragDropData.value(key);
        if (variant.isNull() || !variant.canConvert<QList<int>>()) {
            continue;
        }

        QList<QTreeWidgetItem *> itemsToRemove;

        QList<int> values = variant.value<QList<int>>();
        for (auto &value : values) {
            QTreeWidgetItem *item = parentItem->child(value);
            if (item == nullptr) {
                continue;
            }

            QVariant data = item->data(0, Qt::UserRole);
            if (data.isNull() || !data.canConvert<QString>()) {
                continue;
            }

            QTreeWidgetItem *newItem = targetItemCasted->addChildModItem(data.toString());
            if (newItem == nullptr) {
                continue;
            }

            hasChanges = true;

            if (sourceTreeWidget == this) {
                itemsToRemove.append(item);
                newItem->setCheckState(0, item->checkState(0));
            }
        }

        for (auto item : itemsToRemove) {
            Util::removeTreeWidgetItem(item);
        }
    }

    targetItem->setExpanded(true);
    this->doSort();
    event->accept();

    if (hasChanges) {
        emit updateSignal();
    }
}
