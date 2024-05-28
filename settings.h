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

#include "modgroupstreewidgetitem.h"

#define ARMA3EXECUTABLE_KEY "arma3Executable"
#define MODFOLDERS_KEY "modFolders"
#define MODGROUPS_KEY "modGroups"

#define MODGROUPS_ISCHECKED_KEY "isChecked"
#define MODGROUPS_PATH_KEY "path"
#define MODGROUPS_ISEXPANDED_KEY "isExpanded"
#define MODGROUPS_MODS_KEY "mods"

class Settings : public QObject
{
    Q_OBJECT

public:
    explicit Settings(QMainWindow *mainWindow);

    void save();
    QVariant get(QString key);

private:
    QLineEdit *arma3ExecutableLineEdit;
    QListWidget *modFoldersListWidget;
    QTreeWidget *modGroupsTreeWidget;
    QString saveFilename;

    QJsonArray getModFolders();
    QJsonObject getModGroups();

signals:
};

#endif // SETTINGS_H
