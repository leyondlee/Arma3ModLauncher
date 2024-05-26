#ifndef CUSTOMTREEWIDGET_H
#define CUSTOMTREEWIDGET_H

#include <QTreeWidget>
#include <QDragEnterEvent>

class CustomTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    enum Type {
        None = 1,
        Sender = 2,
        Receiver = 4
    };
    Q_DECLARE_FLAGS(Types, Type)

    explicit CustomTreeWidget(QWidget *parent = nullptr);

    void setType(CustomTreeWidget::Types type);
    bool hasItem(QString, QVariant data = QVariant(), int column = 0);

private:
    CustomTreeWidget::Types type;

protected:
    void dragEnterEvent(QDragEnterEvent *) override;
    void dragLeaveEvent(QDragLeaveEvent *) override;
    void dragMoveEvent(QDragMoveEvent *) override;
    void dropEvent(QDropEvent *) override;

signals:
    void dragEnterEventSignal(QDragEnterEvent *);
    void dragMoveEventSignal(QDragMoveEvent *);
    void dropEventSignal(QDropEvent *);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CustomTreeWidget::Types)

#endif // CUSTOMTREEWIDGET_H
