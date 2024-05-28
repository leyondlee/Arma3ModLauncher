#ifndef MODSTABDRAGDROPDATA_H
#define MODSTABDRAGDROPDATA_H

#include <QObject>
#include <QHash>
#include <QJsonDocument>

class ModsTabDragDropData : public QObject
{
    Q_OBJECT
public:
    explicit ModsTabDragDropData(QObject *parent = nullptr);

    QList<int> keys();
    QVariant value(int key);
    void insert(int key, int value);
    QByteArray data();
    void load(QByteArray data);

private:
    QHash<int, QList<int>> hashMap;

signals:
};

#endif // MODSTABDRAGDROPDATA_H
