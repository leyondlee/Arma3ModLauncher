#include "customtreewidget.h"

CustomTreeWidget::CustomTreeWidget(QWidget *parent)
    : QTreeWidget{parent}
{
    setType(CustomTreeWidget::Type::None);
}

void CustomTreeWidget::setType(CustomTreeWidget::Types type)
{
    this->type = type;

    this->setDragEnabled(false);
    this->setAcceptDrops(false);

    if (type | CustomTreeWidget::Receiver) {
        this->setAcceptDrops(true);
    }

    if (type | CustomTreeWidget::Sender) {
        this->setDragEnabled(true);
    }
}

bool CustomTreeWidget::hasItem(QString text, QVariant data, int column)
{
    QTreeWidgetItemIterator it(this);
    while (*it) {
        if ((*it)->text(column) == text && (data.isNull() || (*it)->data(column, Qt::UserRole) == data)) {
            return true;
        }

        ++it;
    }

    return false;
}

void CustomTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    emit dragEnterEventSignal(event);
}

void CustomTreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{

}

void CustomTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    emit dragMoveEventSignal(event);
}

void CustomTreeWidget::dropEvent(QDropEvent *event)
{
    emit dropEventSignal(event);
}
