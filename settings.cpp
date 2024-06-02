#include "settings.h"

Settings::Settings(QMainWindow *mainWindow)
    : QObject{mainWindow}
{
    this->arma3FolderLineEdit = mainWindow->findChild<QLineEdit *>("arma3FolderLineEdit");
    this->parametersGroupBox = mainWindow->findChild<QGroupBox *>("parametersGroupBox");
    this->additionalParametersListWidget = mainWindow->findChild<QListWidget *>("additionalParametersListWidget");
    this->modFoldersListWidget = mainWindow->findChild<QListWidget *>("modFoldersListWidget");
    this->availableModsTreeWidget = mainWindow->findChild<QTreeWidget *>("availableModsTreeWidget");
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
    jsonObject.insert(ARMA3FOLDER_KEY, this->arma3FolderLineEdit->text());
    jsonObject.insert(PARAMETERS_KEY, getParametersJson());
    jsonObject.insert(ADDITIONALPARAMETERS_KEY, getAdditionalParametersJson());
    jsonObject.insert(MODFOLDERS_KEY, getModFoldersJson());
    jsonObject.insert(MODGROUPS_KEY, getModGroupsJson());

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
    if (!jsonDocument.isObject()) {
        return QJsonValue();
    }

    QJsonObject jsonObject = jsonDocument.object();
    if (!jsonObject.contains(key)) {
        return QJsonValue();
    }

    return jsonObject.value(key);
}

QJsonValue Settings::getParametersJson()
{
    QJsonArray parameters;
    for (auto object : this->parametersGroupBox->children()) {
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(object);
        if (checkBox == nullptr || checkBox->checkState() != Qt::Checked) {
            continue;
        }

        QVariant parameterVariant = checkBox->property(PARAMETER_CHECKBOX_PROPERTY);
        if (parameterVariant.isNull() || !parameterVariant.canConvert<QString>()) {
            continue;
        }

        parameters.append(parameterVariant.toString());
    }

    return parameters;
}

QJsonValue Settings::getAdditionalParametersJson()
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

QJsonValue Settings::getModFoldersJson()
{
    QJsonObject modFoldersData;
    for (int i = 0; i < this->modFoldersListWidget->count(); i += 1) {
        QListWidgetItem *item = this->modFoldersListWidget->item(i);
        QString folder = item->text();
        if (modFoldersData.contains(folder)) {
            continue;
        }

        bool isExpanded;
        QTreeWidgetItem *availableModsTreeWidgetItem = Util::getItemInTreeWithData(this->availableModsTreeWidget, QVariant(folder));
        if (availableModsTreeWidgetItem == nullptr) {
            isExpanded = false;
        } else {
            isExpanded = availableModsTreeWidgetItem->isExpanded();
        }

        QJsonObject folderObject;
        folderObject.insert(MODFOLDERS_ISEXPANDED_KEY, isExpanded);

        modFoldersData.insert(folder, folderObject);
    }

    return modFoldersData;
}

QJsonValue Settings::getModGroupsJson()
{
    QJsonObject modGroupsData;
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
        modGroupsData.insert(data.toString(), folderObject);
    }

    return modGroupsData;
}
