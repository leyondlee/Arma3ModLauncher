#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>
#include <QMainWindow>
#include <QLineEdit>
#include <QGroupBox>
#include <QListWidget>
#include <QTreeWidgetItem>
#include <QCheckBox>
#include <QListWidgetItem>
#include <QHash>
#include <QToolButton>
#include <QProcess>

#include "modgroupstreewidget.h"
#include "settings.h"

#define ARMA3_EXECUTABLE "arma3_x64.exe"

class Launcher : public QObject
{
    Q_OBJECT

public:
    explicit Launcher(QMainWindow *mainWindow);

    QToolButton *launchGameToolButton;
    ModGroupsTreeWidget *modGroupsTreeWidget;
    QLineEdit *arma3FolderLineEdit;
    QGroupBox *parametersGroupBox;
    QListWidget *additionalParametersListWidget;

private:
    void initLaunchGameButton();
    void launchWithoutModsActionTriggered(bool checked);
    void launchGameToolButtonClicked(bool checked);
    void startGameProcess(QStringList mods = QStringList());
    QStringList getMods();
    QStringList getParameters();

signals:
};

#endif // LAUNCHER_H
