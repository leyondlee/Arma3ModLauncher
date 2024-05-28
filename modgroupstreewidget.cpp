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

void ModGroupsTreeWidget::doSort()
{
    this->sortItems(0, Qt::AscendingOrder);
}

QTreeWidgetItem *ModGroupsTreeWidget::getItem(QString text, QVariant data, int column)
{
    QTreeWidgetItemIterator it(this);
    while (*it) {
        if (QString::compare((*it)->text(column), text) == 0 && (*it)->data(column, Qt::UserRole) == data) {
            return (*it);
        }

        ++it;
    }

    return nullptr;
}

bool ModGroupsTreeWidget::hasItem(QString text, QVariant data, int column)
{
    return getItem(text, data, column) != nullptr;
}

void ModGroupsTreeWidget::customContextMenuRequestedHandler(QPoint pos)
{
    QMenu *menu = new QMenu(this);

    QAction *addFolderAction = new QAction("Add Folder", menu);
    menu->addAction(addFolderAction);
    connect(addFolderAction, &QAction::triggered, this, &ModGroupsTreeWidget::addFolderActionTriggered);

    menu->exec(this->mapToGlobal(pos));
}

void ModGroupsTreeWidget::addFolderActionTriggered(bool)
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Add Folder"), tr("Name:"), QLineEdit::Normal, tr(""), &ok);

    if (!ok || name.isEmpty()) {
        return;
    }

    if (hasItem(name, QVariant(), 0)) {
        QMessageBox messageBox(QMessageBox::Warning, "Warning", "Folder already exists.", QMessageBox::NoButton, this);
        messageBox.exec();
        return;
    }

    ModGroupsTreeWidgetItem *folderItem = new ModGroupsTreeWidgetItem(name, QVariant(name), true);
    this->addTopLevelItem(folderItem);
    doSort();
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

            QString name = Util::getFilename(data.toString());
            if (Util::hasItemInTreeWidgetItem(targetItem, name, data, 0)) {
                continue;
            }

            ModGroupsTreeWidgetItem *newItem = new ModGroupsTreeWidgetItem(name, data, false);
            targetItem->addChild(newItem);

            if (sourceTreeWidget == this) {
                itemsToRemove.append(item);
            }
        }

        for (auto item : itemsToRemove) {
            Util::removeTreeWidgetItem(item);
        }
    }

    targetItem->setExpanded(true);
    doSort();
    event->accept();
}
