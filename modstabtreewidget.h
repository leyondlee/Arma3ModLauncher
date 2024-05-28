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

    QTreeWidgetItem *getItem(QString text, QVariant data, int column);
    bool hasItem(QString text, QVariant data, int column);
    void doSort();
    void expandAll();
    void collapseAll();

private:
    Qt::DropAction dropAction;
    QPoint dragStartPosition;

protected:
    bool isItemDraggable(QTreeWidgetItem *item);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // MODSTABTREEWIDGET_H
