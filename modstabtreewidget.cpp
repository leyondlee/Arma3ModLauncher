#include "modstabtreewidget.h"

ModsTabTreeWidget::ModsTabTreeWidget(Qt::DropAction dropAction, QWidget *parent)
    : QTreeWidget{parent}
{
    this->dropAction = dropAction;

    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setHeaderHidden(true);
    this->header()->setStretchLastSection(false);
    this->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    this->setDragEnabled(false);
    this->setAcceptDrops(false);
}

QTreeWidgetItem *ModsTabTreeWidget::getItem(QString text, QVariant data, int column)
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

bool ModsTabTreeWidget::hasItem(QString text, QVariant data, int column)
{
    return getItem(text, data, column) != nullptr;
}

void ModsTabTreeWidget::doSort()
{
    this->sortItems(0, Qt::AscendingOrder);
}

void ModsTabTreeWidget::expandAll()
{
    for (int i = 0; i < this->topLevelItemCount(); i += 1) {
        QTreeWidgetItem *item = this->topLevelItem(i);
        if (item == nullptr) {
            continue;
        }

        item->setExpanded(true);
    }
}

void ModsTabTreeWidget::collapseAll()
{
    for (int i = 0; i < this->topLevelItemCount(); i += 1) {
        QTreeWidgetItem *item = this->topLevelItem(i);
        if (item == nullptr) {
            continue;
        }

        item->setExpanded(false);
    }
}

bool ModsTabTreeWidget::isItemDraggable(QTreeWidgetItem *item)
{
    if (item == nullptr) {
        return false;
    }

    return (this->indexOfTopLevelItem(item) == -1);
}

void ModsTabTreeWidget::mousePressEvent(QMouseEvent *event)
{
    QTreeWidget::mousePressEvent(event);

    this->dragStartPosition = event->position().toPoint();
}

void ModsTabTreeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QTreeWidget::mouseReleaseEvent(event);

    if (this->dragStartPosition.isNull()) {
        return;
    }

    this->dragStartPosition.setX(0);
    this->dragStartPosition.setY(0);
    Q_ASSERT(this->dragStartPosition.isNull());
}

void ModsTabTreeWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }

    QPoint pos = event->position().toPoint();
    if ((pos - this->dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }

    QTreeWidgetItem *item = this->itemAt(this->dragStartPosition);
    if (item == nullptr || !isItemDraggable(item)) {
        return;
    }

    item->setSelected(true);

    ModsTabDragDropData dragDropData;
    QList<QTreeWidgetItem *> selectedItems = this->selectedItems();
    for (auto item : selectedItems) {
        QTreeWidgetItem *parentItem = item->parent();
        if (parentItem == nullptr || parentItem->parent() != nullptr) {
            item->setSelected(false);
            continue;
        }

        int key = this->indexOfTopLevelItem(parentItem);
        if (key == -1) {
            item->setSelected(false);
            continue;
        }

        int value = parentItem->indexOfChild(item);
        if (value == -1) {
            item->setSelected(false);
            continue;
        }

        dragDropData.insert(key, value);
    }

    QMimeData *mimeData = new QMimeData();
    mimeData->setData(JSON_MIME, dragDropData.data());

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->exec(this->dropAction);
    this->clearSelection();
}
