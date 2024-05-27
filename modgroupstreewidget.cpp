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

bool ModGroupsTreeWidget::hasItem(QString text, QVariant data, int column)
{
    QTreeWidgetItemIterator it(this);
    while (*it) {
        if (QString::compare((*it)->text(column), text) == 0 && (*it)->data(column, Qt::UserRole) == data) {
            return true;
        }

        ++it;
    }

    return false;
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

    ModGroupsTreeWidgetItem *folderItem = new ModGroupsTreeWidgetItem(name, QVariant(), true);
    this->addTopLevelItem(folderItem);
    doSort();
}

bool ModGroupsTreeWidget::isItemDraggable(QTreeWidgetItem *item)
{
    return true;
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
    this->clearSelection();

    QTreeWidgetItem *item = this->itemAt(event->position().toPoint());
    if (item == nullptr) {
        event->ignore();
        return;
    }

    while (item->parent() != nullptr) {
        item = item->parent();
    }

    ModGroupsTreeWidgetItem *castedItem = dynamic_cast<ModGroupsTreeWidgetItem *>(item);
    if (castedItem == nullptr || !castedItem->isFolder()) {
        event->ignore();
        return;
    }

    item->setSelected(true);

    QString path = event->mimeData()->text();
    if (path.isEmpty()) {
        event->ignore();
        return;
    }

    QString name = Util::getFilename(path);
    QVariant data(path);
    if (Util::hasItemInTreeWidgetItem(item, name, data, 0)) {
        event->ignore();
        return;
    }

    if (event->source() == this) {
        event->setDropAction(Qt::MoveAction);
    } else {
        event->setDropAction(Qt::CopyAction);
    }

    event->accept();
}

void ModGroupsTreeWidget::dropEvent(QDropEvent *event)
{
    QTreeWidget *sourceTreeWidget = qobject_cast<QTreeWidget *>(event->source());
    if (sourceTreeWidget == nullptr || (sourceTreeWidget != this->availableModsTreeWidget && sourceTreeWidget != this)) {
        event->ignore();
        return;
    }

    QTreeWidgetItem *parentItem = this->itemAt(event->position().toPoint());
    if (parentItem == nullptr) {
        event->ignore();
        return;
    }

    while (parentItem->parent() != nullptr) {
        parentItem = parentItem->parent();
    }

    ModGroupsTreeWidgetItem *parentItemCasted = dynamic_cast<ModGroupsTreeWidgetItem *>(parentItem);
    if (parentItemCasted == nullptr || !parentItemCasted->isFolder()) {
        event->ignore();
        return;
    }

    QString path = event->mimeData()->text();
    if (path.isEmpty()) {
        event->ignore();
        return;
    }

    QString name = Util::getFilename(path);

    parentItem->setExpanded(true);

    QVariant data(path);
    if (Util::hasItemInTreeWidgetItem(parentItem, name, data, 0)) {
        event->ignore();
        return;
    }

    if (sourceTreeWidget == this) {
        QTreeWidgetItem *currentItem = sourceTreeWidget->currentItem();
        if (currentItem->parent() == parentItem) {
            event->ignore();
            return;
        }

        Util::removeTreeWidgetItem(currentItem);
    }

    ModGroupsTreeWidgetItem *newItem = new ModGroupsTreeWidgetItem(name, data, false);
    parentItem->addChild(newItem);

    doSort();

    event->accept();
}
