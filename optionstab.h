#ifndef OPTIONSTAB_H
#define OPTIONSTAB_H

#include <QObject>
#include <QMainWindow>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QMenu>
#include <QCheckBox>
#include <QHash>
#include <QGroupBox>

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
    void refreshTab();

private:
    ModsTab *modsTab;
    Settings *settings;
    QLineEdit *arma3ExecutableLineEdit;
    QPushButton *arma3ExecutableAutoDetectPushButton;
    QPushButton *arma3ExecutableBrowsePushButton;
    QGroupBox *parametersGroupBox;
    QListWidget *additionalParametersListWidget;
    QPushButton *additionalParametersAddPushButton;
    QListWidget *modFoldersListWidget;
    QPushButton *modFoldersAddPushButton;
    QHash<QString, QString> parameterCheckBoxMap {
        {"-skipIntro", "skipIntroCheckBox"},
        {"-noSplash", "noSplashCheckBox"},
        {"-window", "windowCheckBox"},
        {"-filePatching", "filePatchingCheckBox"},
        {"-noLogs", "noLogsCheckBox"},
        {"-world=empty", "emptyWorldCheckBox"},
        {"-showScriptErrors", "showScriptErrorsCheckBox"}
    };

    void init();
    void initParameterCheckBoxes();
    void loadArma3Executable();
    void loadParameters();
    void loadAdditionalParameters();
    void loadModFolders();
    void arma3ExecutableAutoDetectPushButtonClicked(bool checked);
    void arma3ExecutableBrowsePushButtonClicked(bool checked);
    void parametersCheckBoxStateChanged(int state);
    void additionalParametersAddPushButtonClicked(bool checked);
    void additionalParametersListWidgetCustomContextMenuRequestedHandler(QPoint pos);
    void additionalParametersEditActionTriggered(bool checked);
    void additionalParametersRemoveActionTriggered(bool checked);
    void modFoldersListWidgetCustomContextMenuRequestedHandler(QPoint pos);
    void modFoldersRemoveActionTriggered(bool checked);
    void modFoldersAddPushButtonClicked(bool checked);
    QString getDetectedArma3Folder();
    void setArma3Executable(QString path);
    bool addToAdditionalParametersList(QString value);
    bool addToModFoldersList(QString text, int row = -1);

signals:
};

#endif // OPTIONSTAB_H
