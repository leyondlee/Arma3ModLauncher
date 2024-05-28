#include "settings.h"

Settings::Settings(QMainWindow *mainWindow)
    : QObject{mainWindow}
{
    QApplication::setOrganizationName(QApplication::applicationName());

    this->settings = new QSettings(QSettings::Format::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName(), this);
    this->arma3ExecutableLineEdit = mainWindow->findChild<QLineEdit *>("arma3ExecutableLineEdit");
    this->modFoldersListWidget = mainWindow->findChild<QListWidget *>("modFoldersListWidget");
    this->modGroupsTreeWidget = mainWindow->findChild<QTreeWidget *>("modGroupsTreeWidget");
}

void Settings::save()
{
    this->settings->setValue(ARMA3EXECUTABLE_KEY, this->arma3ExecutableLineEdit->text());
    this->settings->setValue(MODFOLDERS_KEY, getModFolders());
    this->settings->setValue(MODGROUPS_KEY, getModGroupsJson());
}

QVariant Settings::get(QString key)
{
    return this->settings->value(key);
}

QStringList Settings::getModFolders()
{
    QStringList modFolders;
    for (int i = 0; i < modFoldersListWidget->count(); i += 1) {
        QListWidgetItem *item = modFoldersListWidget->item(i);
        QString folder = item->text();
        if (modFolders.contains(folder)) {
            continue;
        }

        modFolders.append(folder);
    }

    return modFolders;
}

QString Settings::getModGroupsJson()
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

    return QString(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact));
}
