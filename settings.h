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

#include "modgroupstreewidgetitem.h"

#define ARMA3EXECUTABLE_KEY "arma3Executable"
#define MODFOLDERS_KEY "modFolders"
#define MODGROUPS_KEY "modGroups"

#define MODGROUPS_ISCHECKED_KEY "isChecked"
#define MODGROUPS_PATH_KEY "path"

class Settings : public QObject
{
    Q_OBJECT

public:
    explicit Settings(QMainWindow *mainWindow);

    void save();
    QVariant get(QString key);

private:
    QSettings *settings;
    QLineEdit *arma3ExecutableLineEdit;
    QListWidget *modFoldersListWidget;
    QTreeWidget *modGroupsTreeWidget;

    QStringList getModFolders();
    QString getModGroupsJson();

signals:
};

#endif // SETTINGS_H
