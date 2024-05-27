#include "modstabtreewidget.h"

ModsTabTreeWidget::ModsTabTreeWidget(Qt::DropAction dropAction,QWidget *parent)
    : QTreeWidget{parent}
{
    this->dropAction = dropAction;

    this->setHeaderHidden(true);
    this->header()->setStretchLastSection(false);
    this->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    this->setDragEnabled(false);
    this->setAcceptDrops(false);
}

void ModsTabTreeWidget::mousePressEvent(QMouseEvent *event)
{
    QTreeWidget::mousePressEvent(event);

    this->dragStartPosition = event->position().toPoint();
}

void ModsTabTreeWidget::mouseReleaseEvent(QMouseEvent *event)
{
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

    QVariant data = item->data(0, Qt::UserRole);
    if (data.isNull() || !data.canConvert<QString>()) {
        return;
    }

    QMimeData *mimeData = new QMimeData();
    mimeData->setText(data.toString());

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->exec(this->dropAction);
}
