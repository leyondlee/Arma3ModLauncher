#include "optionstab.h"

OptionsTab::OptionsTab(QWidget *optionsTab, Settings *settings)
    : QObject{optionsTab}
{
    this->settings = settings;
    this->arma3ExecutableLineEdit = optionsTab->findChild<QLineEdit *>("arma3ExecutableLineEdit");
    this->modFoldersListWidget = optionsTab->findChild<QListWidget *>("modFoldersListWidget");

    init();
}

void OptionsTab::tabChanged()
{
    this->arma3ExecutableLineEdit->setCursorPosition(0);
}

void OptionsTab::init()
{
    QVariant modFoldersSettings = this->settings->get(MODFOLDERS_KEY);
    if (!modFoldersSettings.isNull() && modFoldersSettings.canConvert<QStringList>()) {
        QStringList modFolders = modFoldersSettings.toStringList();
        for (auto &folder : modFolders) {
            if (hasModFolder(folder)) {
                continue;
            }

            modFoldersListWidget->addItem(folder);
        }
    }

    this->arma3ExecutableLineEdit->setReadOnly(true);

    QVariant arma3ExecutableSetting = this->settings->get(ARMA3EXECUTABLE_KEY);
    QString arma3Executable;
    if (arma3ExecutableSetting.isNull() || !arma3ExecutableSetting.canConvert<QString>()) {
        QString detectedArma3Folder = getDetectedArma3Folder();
        if (!detectedArma3Folder.isEmpty()) {
            arma3Executable = Util::joinPaths(QStringList({detectedArma3Folder, ARMA3_EXECUTABLE}));
            setArma3Executable(arma3Executable);

            QString workshopPath = Util::joinPaths(QStringList({detectedArma3Folder, WORKSHOP_FOLDER}));
            if (!hasModFolder(workshopPath)) {
                modFoldersListWidget->insertItem(0,workshopPath);
            }

            this->settings->save();
        }
    } else {
        arma3Executable = arma3ExecutableSetting.toString();
        setArma3Executable(arma3Executable);
    }
}

QString OptionsTab::getDetectedArma3Folder()
{
    QSettings settings(ARMA3_REGISTRY, QSettings::NativeFormat);
    QString folderName = settings.value("main", "").toString();

    return folderName;
}

void OptionsTab::setArma3Executable(QString path)
{
    this->arma3ExecutableLineEdit->setText(path);
    this->arma3ExecutableLineEdit->setCursorPosition(0);
}

bool OptionsTab::hasModFolder(QString folder)
{
    for (int i = 0; i < modFoldersListWidget->count(); i += 1) {
        QListWidgetItem *item = modFoldersListWidget->item(i);
        if (QString::compare(item->text(), folder, Qt::CaseSensitive) == 0) {
            return true;
        }
    }

    return false;
}
