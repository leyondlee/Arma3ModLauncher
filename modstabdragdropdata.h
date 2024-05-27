#ifndef MODSTABDRAGDROPDATA_H
#define MODSTABDRAGDROPDATA_H

#include <QObject>
#include <QMap>
#include <QJsonDocument>

class ModsTabDragDropData : public QObject
{
    Q_OBJECT
public:
    explicit ModsTabDragDropData(QObject *parent = nullptr);

    QStringList keys();
    QVariant value(QString key);
    void insert(QString key, QString value);
    QByteArray data();
    void load(QByteArray data);

private:
    QMap<QString, QStringList> map;

signals:
};

#endif // MODSTABDRAGDROPDATA_H
