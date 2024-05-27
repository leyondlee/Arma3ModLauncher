#include "modstabdragdropdata.h"

ModsTabDragDropData::ModsTabDragDropData(QObject *parent)
    : QObject{parent}
{

}

QStringList ModsTabDragDropData::keys()
{
    return this->map.keys();
}

QVariant ModsTabDragDropData::value(QString key)
{
    if (!this->map.contains(key)) {
        return QVariant();
    }

    return QVariant(this->map.value(key));
}

void ModsTabDragDropData::insert(QString key, QString value)
{
    if (this->map.contains(key)) {
        QStringList values = this->map.value(key);
        values.append(value);
        this->map.insert(key, values);
        return;
    }

    this->map.insert(key, QStringList(value));
}

QByteArray ModsTabDragDropData::data()
{
    QVariantMap variantMap;

    QStringList keys = this->map.keys();
    for (QString &key : keys) {
        variantMap.insert(key, QVariant(this->map.value(key)));
    }

    return QJsonDocument::fromVariant(variantMap).toJson(QJsonDocument::Compact);
}

void ModsTabDragDropData::load(QByteArray data)
{
    this->map.clear();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    QVariant variant = jsonDocument.toVariant();
    if (!variant.canConvert<QVariantMap>()) {
        return;
    }

    QVariantMap variantMap = variant.toMap();

    QStringList keys = variantMap.keys();
    for (QString &key : keys) {
        QVariant value = variantMap.value(key);
        if (value.isNull() || !value.canConvert<QStringList>()) {
            continue;
        }

        this->map.insert(key, value.toStringList());
    }
}


