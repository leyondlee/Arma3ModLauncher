#ifndef MODGROUPSTREEWIDGET_H
#define MODGROUPSTREEWIDGET_H

#include <QTreeWidget>
#include <QHeaderView>
#include <QDragEnterEvent>

class ModGroupsTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit ModGroupsTreeWidget(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *) override;
    void dragLeaveEvent(QDragLeaveEvent *) override;
    void dragMoveEvent(QDragMoveEvent *) override;
    void dropEvent(QDropEvent *) override;

signals:
    void dragEnterSignal(QDragEnterEvent *);
    void dragLeaveSignal(QDragLeaveEvent *);
    void dragMoveSignal(QDragMoveEvent *);
    void dropSignal(QDropEvent *);
};

#endif // MODGROUPSTREEWIDGET_H
