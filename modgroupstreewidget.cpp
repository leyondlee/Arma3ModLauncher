#include "modgroupstreewidget.h"

ModGroupsTreeWidget::ModGroupsTreeWidget(QWidget *parent)
    : QTreeWidget{parent}
{
    this->setHeaderHidden(true);
    this->header()->setStretchLastSection(false);
    this->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}

void ModGroupsTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    emit dragEnterSignal(event);
}

void ModGroupsTreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    emit dragLeaveSignal(event);
}

void ModGroupsTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    emit dragMoveSignal(event);
}

void ModGroupsTreeWidget::dropEvent(QDropEvent *event)
{
    emit dropSignal(event);
}
