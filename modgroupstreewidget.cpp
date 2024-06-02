#include "modgroupstreewidget.h"

ModGroupsTreeWidget::ModGroupsTreeWidget(QWidget *parent)
    : ModsTabTreeWidget{Qt::MoveAction, parent}
{
    this->availableModsTreeWidget = nullptr;

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &ModGroupsTreeWidget::customContextMenuRequested, this, &ModGroupsTreeWidget::customContextMenuRequestedHandler);
    connect(this, &ModGroupsTreeWidget::itemChanged, this, &ModGroupsTreeWidget::itemChangedHandler);
}

void ModGroupsTreeWidget::setAvailableModsTreeWidget(AvailableModsTreeWidget *availableModsTreeWidget)
{
    this->availableModsTreeWidget = availableModsTreeWidget;
}

ModGroupsTreeWidgetItem *ModGroupsTreeWidget::addFolder(QString name)
{
    QString nameTrimmed = name.trimmed();
    if (nameTrimmed.isEmpty()) {
        return nullptr;
    }

    ModGroupsTreeWidgetItem *folderItem = new ModGroupsTreeWidgetItem(nameTrimmed, QVariant(nameTrimmed), true);
    folderItem->setFlags(folderItem->flags() & ~Qt::ItemIsDragEnabled);

    this->addTopLevelItem(folderItem);
    this->doSort();

    return folderItem;
}

void ModGroupsTreeWidget::customContextMenuRequestedHandler(QPoint pos)
{
    QMenu *menu = new QMenu(this);

    QAction *addFolderAction = new QAction("Add Folder", menu);
    menu->addAction(addFolderAction);
    connect(addFolderAction, &QAction::triggered, this, &ModGroupsTreeWidget::addFolderActionTriggered);

    QAction *renameAction = new QAction("Rename", menu);
    menu->addAction(renameAction);
    connect(renameAction, &QAction::triggered, this, &ModGroupsTreeWidget::renameActionTriggered);

    QAction *removeAction = new QAction("Remove", menu);
    menu->addAction(removeAction);
    connect(removeAction, &QAction::triggered, this, &ModGroupsTreeWidget::removeActionTriggered);

    ModGroupsTreeWidgetItem *item =  ModGroupsTreeWidgetItem::castTreeWidgetItem(this->itemAt(pos));
    if (item == nullptr) {
        // No item at pos
        this->clearSelection();
        renameAction->setEnabled(false);
        removeAction->setEnabled(false);
    } else {
        item->setSelected(true);

        if (!item->isFolder() || this->selectedItems().size() > 1) {
            // Item not a folder or selected more than 1 item
            renameAction->setEnabled(false);
        }
    }

    menu->exec(this->mapToGlobal(pos));
}

void ModGroupsTreeWidget::itemChangedHandler(QTreeWidgetItem *item, int column)
{
    if (column != 0) {
        return;
    }

    ModGroupsTreeWidgetItem *castedItem = ModGroupsTreeWidgetItem::castTreeWidgetItem(item);
    if (castedItem == nullptr) {
        return;
    }

    if (!castedItem->hasCheckStateChanged()) {
        return;
    }

    castedItem->updateSavedCheckState();

    emit itemCheckStateChangedSignal(castedItem);
}

void ModGroupsTreeWidget::addFolderActionTriggered(bool checked)
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Add Folder"), tr("Name:"), QLineEdit::Normal, tr(""), &ok).trimmed();
    if (!ok || name.isEmpty()) {
        return;
    }

    if (this->hasItem(name, QVariant(name), 0)) {
        Util::showWarningMessage("Add Folder", "Folder with the name already exists.", this);
        return;
    }

    ModGroupsTreeWidgetItem *folderItem = addFolder(name);
    if (folderItem == nullptr) {
        return;
    }

    this->clearSelection();
    folderItem->setSelected(true);

    emit treeChangedSignal();
}

void ModGroupsTreeWidget::renameActionTriggered(bool checked)
{
    if (this->selectedItems().size() > 1) {
        return;
    }

    QTreeWidgetItem *item = this->currentItem();
    if (item == nullptr) {
        return;
    }

    ModGroupsTreeWidgetItem *castedItem = ModGroupsTreeWidgetItem::castTreeWidgetItem(item);
    if (castedItem == nullptr || !castedItem->isFolder()) {
        return;
    }

    QString name = item->text(0);

    bool ok;
    QString newName = QInputDialog::getText(this, tr("Rename Folder"), tr("Name:"), QLineEdit::Normal, name, &ok).trimmed();
    if (!ok || newName.isEmpty() || QString::compare(newName, name) == 0) {
        return;
    }

    if (this->hasItem(newName, QVariant(newName), 0)) {
        Util::showWarningMessage("Rename Folder", "Folder with the name already exists.", this);
        return;
    }

    item->setText(0, newName);
    item->setData(0, Qt::UserRole, QVariant(newName));

    this->doSort();

    emit treeChangedSignal();
}

void ModGroupsTreeWidget::removeActionTriggered(bool checked)
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

    emit treeChangedSignal();
}

void ModGroupsTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QTreeWidget *sourceTreeWidget = qobject_cast<QTreeWidget *>(event->source());
    if (sourceTreeWidget == nullptr || (sourceTreeWidget != this && sourceTreeWidget != this->availableModsTreeWidget)) {
        return;
    }

    event->acceptProposedAction();
}

/*void ModGroupsTreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{

}*/

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
        ModGroupsTreeWidgetItem *topLevelItem = ModGroupsTreeWidgetItem::castTreeWidgetItem(this->topLevelItem(i));
        if (topLevelItem == nullptr || !topLevelItem->isFolder()) {
            continue;
        }

        if (topLevelItem == item) {
            // Dragging over folder or any item within
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

    // Destination folder
    QTreeWidgetItem *targetItem = item;
    while (targetItem->parent() != nullptr) {
        // Get top level item
        targetItem = targetItem->parent();
    }

    ModGroupsTreeWidgetItem *targetItemCasted = ModGroupsTreeWidgetItem::castTreeWidgetItem(targetItem);
    if (targetItemCasted == nullptr || !targetItemCasted->isFolder()) {
        event->ignore();
        return;
    }

    ModsTabDragDropData dragDropData;
    dragDropData.load(event->mimeData()->data(JSON_MIME));

    bool hasChanges = false;
    QList<int> keys = dragDropData.keys();
    for (auto key : keys) {
        // Source folder
        QTreeWidgetItem *parentItem = sourceTreeWidget->topLevelItem(key);
        if (parentItem == nullptr || parentItem == targetItem) {
            continue;
        }

        QJsonValue jsonValue = dragDropData.value(key);
        if (!jsonValue.isArray()) {
            continue;
        }

        QList<QTreeWidgetItem *> itemsToRemove;
        QJsonArray jsonArray = jsonValue.toArray();
        for (auto value : jsonArray) {
            QVariant valueVariant = value.toVariant();
            if (!valueVariant.canConvert<int>()) {
                continue;
            }

            // Source item
            QTreeWidgetItem *item = parentItem->child(valueVariant.toInt());
            if (item == nullptr) {
                continue;
            }

            QVariant data = item->data(0, Qt::UserRole);
            if (data.isNull() || !data.canConvert<QString>()) {
                continue;
            }

            // Create new item in destination folder
            ModGroupsTreeWidgetItem *newItem = targetItemCasted->addModToFolder(data.toString());
            if (newItem == nullptr) {
                continue;
            }

            hasChanges = true;

            // Check if source of drop is from within modGroupsTreeWidget
            if (sourceTreeWidget == this) {
                itemsToRemove.append(item);
                newItem->setCheckState(item->checkState(0));
            }
        }

        for (auto item : itemsToRemove) {
            Util::removeTreeWidgetItem(item);
        }
    }

    this->blockSignals(true);
    targetItem->setExpanded(true);
    this->blockSignals(false);

    this->doSort();
    event->accept();

    if (hasChanges) {
        emit treeChangedSignal();
    }
}
