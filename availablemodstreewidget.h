#ifndef AVAILABLEMODSTREEWIDGET_H
#define AVAILABLEMODSTREEWIDGET_H

#include <QTreeWidget>

#include "modstabtreewidget.h"

class AvailableModsTreeWidget : public ModsTabTreeWidget
{
    Q_OBJECT

public:
    explicit AvailableModsTreeWidget(QWidget *parent = nullptr);

private:
    QPoint dragStartPosition;

protected:
    bool isItemDraggable(QTreeWidgetItem *item) override;
};

#endif // AVAILABLEMODSTREEWIDGET_H
