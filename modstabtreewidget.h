#ifndef MODSTABTREEWIDGET_H
#define MODSTABTREEWIDGET_H

#include <QTreeWidget>
#include <QHeaderView>
#include <QMouseEvent>
#include <QApplication>
#include <QMimeData>
#include <QDrag>
#include <QTreeWidgetItem>
#include <QMimeType>

#include "modstabdragdropdata.h"

#define JSON_MIME "application/json"

class ModsTabTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit ModsTabTreeWidget(Qt::DropAction, QWidget *parent = nullptr);

private:
    Qt::DropAction dropAction;
    QPoint dragStartPosition;

protected:
    bool isItemDraggable(QTreeWidgetItem *item);
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // MODSTABTREEWIDGET_H
