#ifndef MODSTABDRAGDROPDATA_H
#define MODSTABDRAGDROPDATA_H

#include <QObject>
#include <QHash>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

class ModsTabDragDropData : public QObject
{
    Q_OBJECT
public:
    explicit ModsTabDragDropData(QObject *parent = nullptr);

    QList<int> keys();
    QJsonValue value(int key);
    void insert(int key, int value);
    QByteArray data();
    void load(QByteArray data);

private:
    QJsonObject jsonObject;
};

#endif // MODSTABDRAGDROPDATA_H
