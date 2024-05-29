#ifndef OPTIONSTAB_H
#define OPTIONSTAB_H

#include <QObject>
#include <QMainWindow>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QMenu>

#include "util.h"
#include "modstab.h"
#include "settings.h"

#define ARMA3_REGISTRY "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\bohemia interactive\\arma 3"
#define ARMA3_EXECUTABLE "arma3_x64.exe"
#define WORKSHOP_FOLDER "!Workshop"

class OptionsTab : public QObject
{
    Q_OBJECT

public:
    explicit OptionsTab(QWidget *optionsTab, ModsTab *modsTab, Settings *settings);

    void tabChanged();

private:
    ModsTab *modsTab;
    Settings *settings;
    QLineEdit *arma3ExecutableLineEdit;
    QListWidget *modFoldersListWidget;
    QPushButton *modFoldersAddPushButton;
    QPushButton *browseArma3ExecutablePushButton;

    void init();
    void modFoldersListWidgetCustomContextMenuRequestedHandler(QPoint pos);
    void modFoldersListWidgetRemoveActionTriggered(bool checked);
    void modFoldersAddPushButtonClicked(bool checked);
    void browseArma3ExecutablePushButtonClicked(bool checked);
    QString getDetectedArma3Folder();
    void setArma3Executable(QString path);
    bool hasModFolder(QString folder);
    void insertIntoModFoldersList(QString text, int row = -1);

signals:
};

#endif // OPTIONSTAB_H
