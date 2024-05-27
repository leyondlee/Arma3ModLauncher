#ifndef OPTIONSTAB_H
#define OPTIONSTAB_H

#include <QObject>
#include <QMainWindow>
#include <QLineEdit>
#include <QListWidget>

#include "util.h"
#include "settings.h"

#define ARMA3_REGISTRY "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\bohemia interactive\\arma 3"
#define ARMA3_EXECUTABLE "arma3_x64.exe"
#define WORKSHOP_FOLDER "!Workshop"

class OptionsTab : public QObject
{
    Q_OBJECT

public:
    explicit OptionsTab(QWidget *, Settings *);

    void tabChanged();

private:
    Settings *settings;
    QLineEdit *arma3ExecutableLineEdit;
    QListWidget *modFoldersListWidget;

    void init();
    QString getDetectedArma3Folder();
    void setArma3Executable(QString);
    bool hasModFolder(QString);

signals:
};

#endif // OPTIONSTAB_H
