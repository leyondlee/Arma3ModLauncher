#include "launcher.h"

Launcher::Launcher(QMainWindow *mainWindow)
    : QObject{mainWindow}
{
    this->launchGameToolButton = mainWindow->findChild<QToolButton *>("launchGameToolButton");
    this->modGroupsTreeWidget = mainWindow->findChild<ModGroupsTreeWidget *>("modGroupsTreeWidget");
    this->arma3FolderLineEdit = mainWindow->findChild<QLineEdit *>("arma3FolderLineEdit");
    this->parametersGroupBox = mainWindow->findChild<QGroupBox *>("parametersGroupBox");
    this->additionalParametersListWidget = mainWindow->findChild<QListWidget *>("additionalParametersListWidget");

    initLaunchGameButton();
}

void Launcher::initLaunchGameButton()
{
    QMenu *menu = new QMenu(this->launchGameToolButton);
    QAction *launchWithoutModsAction = new QAction("Launch without mods", menu);
    menu->addAction(launchWithoutModsAction);
    connect(launchWithoutModsAction, &QAction::triggered, this, &Launcher::launchWithoutModsActionTriggered);

    this->launchGameToolButton->setMenu(menu);
    this->launchGameToolButton->setPopupMode(QToolButton::MenuButtonPopup);

    connect(this->launchGameToolButton, &QToolButton::clicked, this, &Launcher::launchGameToolButtonClicked);
}

void Launcher::launchWithoutModsActionTriggered(bool checked)
{
    startGameProcess();
}

void Launcher::launchGameToolButtonClicked(bool checked)
{
    startGameProcess(getMods());
}

void Launcher::startGameProcess(QStringList mods)
{
    QString arma3Folder = this->arma3FolderLineEdit->text();
    if (arma3Folder.isEmpty() || !QFileInfo(arma3Folder).isDir()) {
        Util::showWarningMessage("Launch Game", "Invalid Arma 3 folder.", this->launchGameToolButton);
        return;
    }

    QString arma3Executable = Util::joinPaths({arma3Folder, ARMA3_EXECUTABLE});
    QFileInfo arma3ExecutableInfo(arma3Executable);
    if (!arma3ExecutableInfo.exists() || !arma3ExecutableInfo.isExecutable()) {
        Util::showWarningMessage("Launch Game", "Invalid Arma 3 executable file.", this->launchGameToolButton);
        return;
    }

    QStringList arguments;
    for (auto &mod : mods) {
        QString argument = QString("-mod=%1").arg(Util::cleanPath(mod));
        arguments.append(argument);
    }

    QStringList parameters = getParameters();
    for (auto &parameter : parameters) {
        arguments.append(parameter);
    }

    QProcess process;
    process.setProgram(arma3Executable);
    process.setArguments(arguments);
    process.startDetached();
}

QStringList Launcher::getMods()
{
    QHash<QString, void *> modsMap;
    for (int i = 0; i < this->modGroupsTreeWidget->topLevelItemCount(); i += 1) {
        ModGroupsTreeWidgetItem *item = ModGroupsTreeWidgetItem::castTreeWidgetItem(this->modGroupsTreeWidget->topLevelItem(i));
        if (item == nullptr || !item->isFolder()) {
            continue;
        }

        for (int j = 0; j < item->childCount(); j += 1) {
            ModGroupsTreeWidgetItem *childItem = ModGroupsTreeWidgetItem::castTreeWidgetItem(item->child(j));
            if (childItem == nullptr || childItem->checkState(0) != Qt::Checked || childItem->isMissing()) {
                continue;
            }

            QVariant data = childItem->data(0, Qt::UserRole);
            if (data.isNull() || !data.canConvert<QString>()) {
                continue;
            }

            QString mod = data.toString();
            if (modsMap.contains(mod)) {
                continue;
            }

            modsMap.insert(mod, nullptr);
        }
    }

    QStringList mods = modsMap.keys();
    mods.sort(Qt::CaseInsensitive);

    return mods;
}

QStringList Launcher::getParameters()
{
    QHash<QString, void *> parametersMap;
    for (auto object : this->parametersGroupBox->children()) {
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(object);
        if (checkBox == nullptr || checkBox->checkState() != Qt::Checked) {
            continue;
        }

        QVariant parameterVariant = checkBox->property(PARAMETER_CHECKBOX_PROPERTY);
        if (parameterVariant.isNull() || !parameterVariant.canConvert<QString>()) {
            continue;
        }

        QString parameter = parameterVariant.toString();
        if (parametersMap.contains(parameter)) {
            continue;
        }

        parametersMap.insert(parameter, nullptr);
    }

    for (int i = 0; i < this->additionalParametersListWidget->count(); i += 1) {
        QListWidgetItem *item = this->additionalParametersListWidget->item(i);
        if (item == nullptr) {
            continue;
        }

        QString parameter = item->text();
        if (parameter.isEmpty() || parametersMap.contains(parameter)) {
            continue;
        }

        parametersMap.insert(parameter, nullptr);
    }

    return parametersMap.keys();
}


