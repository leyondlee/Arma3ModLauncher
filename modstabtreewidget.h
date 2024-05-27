#ifndef MODSTABTREEWIDGET_H
#define MODSTABTREEWIDGET_H

#include <QTreeWidget>
#include <QHeaderView>
#include <QMouseEvent>
#include <QApplication>
#include <QMimeData>
#include <QDrag>
#include <QTreeWidgetItem>

class ModsTabTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit ModsTabTreeWidget(Qt::DropAction, QWidget *parent = nullptr);

private:
    Qt::DropAction dropAction;
    QPoint dragStartPosition;

protected:
    virtual bool isItemDraggable(QTreeWidgetItem *item) = 0;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // MODSTABTREEWIDGET_H
