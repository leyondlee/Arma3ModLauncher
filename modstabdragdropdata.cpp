#include "modstabdragdropdata.h"

ModsTabDragDropData::ModsTabDragDropData(QObject *parent)
    : QObject{parent}
{

}

QList<int> ModsTabDragDropData::keys()
{
    QList<int> keys;
    for (auto i = this->jsonObject.begin(); i != this->jsonObject.end(); ++i) {
        QString keyStr = i.key();

        bool ok;
        int key = keyStr.toInt(&ok);
        if (!ok) {
            continue;
        }

        keys.append(key);
    }

    return keys;
}

QJsonValue ModsTabDragDropData::value(int key)
{
    QString keyStr = QString::number(key);
    if (!this->jsonObject.contains(keyStr)) {
        return QJsonValue();
    }

    return this->jsonObject.value(keyStr);
}

void ModsTabDragDropData::insert(int key, int value)
{
    QJsonArray jsonArray;
    QString keyStr = QString::number(key);
    if (this->jsonObject.contains(keyStr)) {
        QJsonValue jsonValue = jsonObject.value(keyStr);
        if (jsonValue.isArray()) {
            jsonArray = jsonValue.toArray();
        }
    }

    jsonArray.append(value);
    this->jsonObject.insert(keyStr, jsonArray);
}

QByteArray ModsTabDragDropData::data()
{
    return QJsonDocument(this->jsonObject).toJson(QJsonDocument::Compact);
}

void ModsTabDragDropData::load(QByteArray data)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    if (!jsonDocument.isObject()) {
        return;
    }

    this->jsonObject = jsonDocument.object();
}


