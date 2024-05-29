#include "settings.h"

Settings::Settings(QMainWindow *mainWindow)
    : QObject{mainWindow}
{
    this->arma3ExecutableLineEdit = mainWindow->findChild<QLineEdit *>("arma3ExecutableLineEdit");
    this->additionalParametersListWidget = mainWindow->findChild<QListWidget *>("additionalParametersListWidget");
    this->modFoldersListWidget = mainWindow->findChild<QListWidget *>("modFoldersListWidget");
    this->modGroupsTreeWidget = mainWindow->findChild<QTreeWidget *>("modGroupsTreeWidget");
    this->saveFilename = QString("%1.json").arg(Util::joinPaths({QStandardPaths::writableLocation(QStandardPaths::AppDataLocation), QApplication::applicationName()}));
}

void Settings::save()
{
    QFile saveFile(this->saveFilename);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        return;
    }

    QJsonObject jsonObject;
    jsonObject.insert(ARMA3EXECUTABLE_KEY, this->arma3ExecutableLineEdit->text());
    jsonObject.insert(ADDITIONALPARAMETERS_KEY, getAdditionalParametersSave());
    jsonObject.insert(MODFOLDERS_KEY, getModFoldersSave());
    jsonObject.insert(MODGROUPS_KEY, getModGroupsSave());

    saveFile.write(QJsonDocument(jsonObject).toJson());
}

QJsonValue Settings::get(QString key)
{
    if (!QFile::exists(this->saveFilename)) {
        return QJsonValue();
    }

    QFile saveFile(this->saveFilename);
    if (!saveFile.open(QIODevice::ReadOnly)) {
        return QJsonValue();
    }

    QByteArray saveData = saveFile.readAll();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(saveData);
    QJsonObject jsonObject = jsonDocument.object();
    if (!jsonObject.contains(key)) {
        return QJsonValue();
    }

    return jsonObject.value(key);
}

QJsonArray Settings::getAdditionalParametersSave()
{
    QJsonArray additionalParameters;
    for (int i = 0; i < this->additionalParametersListWidget->count(); i += 1) {
        QListWidgetItem *item = this->additionalParametersListWidget->item(i);
        QString value = item->text();
        if (additionalParameters.contains(value)) {
            continue;
        }

        additionalParameters.append(value);
    }

    return additionalParameters;
}

QJsonArray Settings::getModFoldersSave()
{
    QJsonArray modFolders;
    for (int i = 0; i < this->modFoldersListWidget->count(); i += 1) {
        QListWidgetItem *item = this->modFoldersListWidget->item(i);
        QString folder = item->text();
        if (modFolders.contains(folder)) {
            continue;
        }

        modFolders.append(folder);
    }

    return modFolders;
}

QJsonObject Settings::getModGroupsSave()
{
    QJsonObject jsonObject;
    for (int i = 0; i < this->modGroupsTreeWidget->topLevelItemCount(); i += 1) {
        ModGroupsTreeWidgetItem *item = ModGroupsTreeWidgetItem::castTreeWidgetItem(this->modGroupsTreeWidget->topLevelItem(i));
        if (item == nullptr || !item->isFolder()) {
            continue;
        }

        QVariant data = item->data(0, Qt::UserRole);
        if (data.isNull() || !data.canConvert<QString>()) {
            continue;
        }

        QJsonArray values;
        for (int i = 0; i < item->childCount(); i += 1) {
            QTreeWidgetItem *childItem = item->child(i);
            if (childItem == nullptr) {
                continue;
            }

            QVariant childData = childItem->data(0, Qt::UserRole);
            if (childData.isNull() || !childData.canConvert<QString>()) {
                continue;
            }

            QJsonObject value;
            value.insert(MODGROUPS_ISCHECKED_KEY, childItem->checkState(0) == Qt::Checked);
            value.insert(MODGROUPS_PATH_KEY, childData.toString());

            values.append(value);
        }

        QJsonObject folderObject;
        folderObject.insert(MODGROUPS_ISEXPANDED_KEY, item->isExpanded());
        folderObject.insert(MODGROUPS_MODS_KEY, values);
        jsonObject.insert(data.toString(), folderObject);
    }

    return jsonObject;
}
