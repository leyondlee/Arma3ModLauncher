#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QMainWindow>
#include <QLineEdit>
#include <QSettings>
#include <QApplication>
#include <QVariant>
#include <QListWidget>

#define ARMA3EXECUTABLE_KEY "arma3Executable"
#define MODFOLDERS_KEY "modFolders"

class Settings : public QObject
{
    Q_OBJECT

public:
    explicit Settings(QMainWindow *);

    void save();
    QVariant get(QString);

private:
    QSettings *settings;
    QLineEdit *arma3ExecutableLineEdit;
    QListWidget *modFoldersListWidget;

signals:
};

#endif // SETTINGS_H
