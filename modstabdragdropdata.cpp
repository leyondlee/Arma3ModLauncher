#include "modstabdragdropdata.h"

ModsTabDragDropData::ModsTabDragDropData(QObject *parent)
    : QObject{parent}
{

}

QList<int> ModsTabDragDropData::keys()
{
    return this->hashMap.keys();
}

QVariant ModsTabDragDropData::value(int key)
{
    if (!this->hashMap.contains(key)) {
        return QVariant();
    }

    return QVariant::fromValue(this->hashMap.value(key));
}

void ModsTabDragDropData::insert(int key, int value)
{
    QList<int> values;
    if (this->hashMap.contains(key)) {
        values = this->hashMap.value(key);
    }

    values.append(value);
    this->hashMap.insert(key, values);
}

QByteArray ModsTabDragDropData::data()
{
    QVariantHash variantHash;
    QList<int> keys = this->hashMap.keys();
    for (auto &key : keys) {
        QVariantList variantValues;
        for (int value : this->hashMap.value(key)) {
            variantValues.append(value);
        }

        variantHash.insert(QString::number(key), variantValues);
    }

    return QJsonDocument::fromVariant(variantHash).toJson(QJsonDocument::Compact);
}

void ModsTabDragDropData::load(QByteArray data)
{
    this->hashMap.clear();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    QVariant variant = jsonDocument.toVariant();
    if (!variant.canConvert<QVariantHash>()) {
        return;
    }

    QVariantHash variantHash = variant.toHash();

    QStringList keys = variantHash.keys();
    for (QString &keyStr : keys) {
        bool ok;
        int key = keyStr.toInt(&ok);
        if (!ok) {
            continue;
        }

        QVariant value = variantHash.value(keyStr);
        if (value.isNull() || !value.canConvert<QVariantList>()) {
            continue;
        }

        QList<int> values;
        for (auto &var : value.toList()) {
            bool ok;
            int varInt = var.toInt(&ok);
            if (!ok) {
                continue;
            }

            values.append(varInt);
        }

        this->hashMap.insert(key, values);
    }
}


