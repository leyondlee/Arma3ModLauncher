#include "settings.h"

Settings::Settings(QMainWindow *mainWindow)
    : QObject{mainWindow}
{
    QApplication::setOrganizationName(QApplication::applicationName());

    this->settings = new QSettings(QSettings::Format::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName(), this);
    this->arma3ExecutableLineEdit = mainWindow->findChild<QLineEdit *>("arma3ExecutableLineEdit");
    this->modFoldersListWidget = mainWindow->findChild<QListWidget *>("modFoldersListWidget");
}

void Settings::save()
{
    this->settings->setValue(ARMA3EXECUTABLE_KEY, this->arma3ExecutableLineEdit->text());

    QStringList modFolders;
    for (int i = 0; i < modFoldersListWidget->count(); i += 1) {
        QListWidgetItem *item = modFoldersListWidget->item(i);
        QString folder = item->text();
        if (modFolders.contains(folder)) {
            continue;
        }

        modFolders.append(folder);
    }
    this->settings->setValue(MODFOLDERS_KEY, modFolders);
}

QVariant Settings::get(QString key)
{
    return this->settings->value(key);
}
