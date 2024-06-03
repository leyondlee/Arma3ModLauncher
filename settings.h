#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QMainWindow>
#include <QLineEdit>
#include <QSettings>
#include <QApplication>
#include <QVariant>
#include <QListWidget>
#include <QTreeWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QGroupBox>
#include <QCheckBox>

#include "modgroupstreewidgetitem.h"

#define ARMA3FOLDER_KEY "arma3Folder"
#define PARAMETERS_KEY "parameters"
#define ADDITIONALPARAMETERS_KEY "additionalParameters"
#define MODFOLDERS_KEY "modFolders"
#define MODFOLDERS_ISEXPANDED_KEY "isExpanded"
#define MODGROUPS_KEY "modGroups"
#define MODGROUPS_ISCHECKED_KEY "isChecked"
#define MODGROUPS_PATH_KEY "path"
#define MODGROUPS_ISEXPANDED_KEY "isExpanded"
#define MODGROUPS_MODS_KEY "mods"
#define BATTLEYE_KEY "battleye"

#define PARAMETER_CHECKBOX_PROPERTY "parameter_value"

class Settings : public QObject
{
    Q_OBJECT

public:
    explicit Settings(QMainWindow *mainWindow);

    void save();
    QJsonValue get(QString key);

private:
    QLineEdit *arma3FolderLineEdit;
    QGroupBox *parametersGroupBox;
    QListWidget *additionalParametersListWidget;
    QListWidget *modFoldersListWidget;
    QTreeWidget *availableModsTreeWidget;
    QTreeWidget *modGroupsTreeWidget;
    QCheckBox *battleyeCheckBox;
    QString saveFilename;

    void init();
    QJsonValue getParametersJson();
    QJsonValue getAdditionalParametersJson();
    QJsonValue getModFoldersJson();
    QJsonValue getModGroupsJson();

signals:
};

#endif // SETTINGS_H
