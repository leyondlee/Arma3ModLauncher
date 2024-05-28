#include "optionstab.h"

OptionsTab::OptionsTab(QWidget *optionsTab, ModsTab *modsTab, Settings *settings)
    : QObject{optionsTab}
{
    this->modsTab = modsTab;
    this->settings = settings;
    this->arma3ExecutableLineEdit = optionsTab->findChild<QLineEdit *>("arma3ExecutableLineEdit");
    this->modFoldersListWidget = optionsTab->findChild<QListWidget *>("modFoldersListWidget");
    this->modFoldersAddPushButton = optionsTab->findChild<QPushButton *>("modFoldersAddPushButton");
    this->browseArma3ExecutablePushButton = optionsTab->findChild<QPushButton *>("browseArma3ExecutablePushButton");

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

            insertIntoModFoldersListWidget(folder);
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
                insertIntoModFoldersListWidget(workshopPath, 0);
            }

            this->settings->save();
            this->modsTab->refreshMods();
        }
    } else {
        arma3Executable = arma3ExecutableSetting.toString();
        setArma3Executable(arma3Executable);
    }

    this->modFoldersListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->modFoldersListWidget, &QListWidget::customContextMenuRequested, this, &OptionsTab::modFoldersListWidgetCustomContextMenuRequestedHandler);
    connect(this->modFoldersAddPushButton, &QPushButton::clicked, this, &OptionsTab::modFoldersAddPushButtonClicked);
    connect(this->browseArma3ExecutablePushButton, &QPushButton::clicked, this, &OptionsTab::browseArma3ExecutablePushButtonClicked);
}

void OptionsTab::modFoldersListWidgetCustomContextMenuRequestedHandler(QPoint pos)
{
    this->modFoldersListWidget->clearSelection();

    QListWidgetItem *item = this->modFoldersListWidget->itemAt(pos);
    if (item == nullptr) {
        return;
    }

    item->setSelected(true);

    QMenu *menu = new QMenu(this->modFoldersListWidget);
    QAction *removeAction = new QAction("Remove", menu);
    menu->addAction(removeAction);
    connect(removeAction, &QAction::triggered, this, &OptionsTab::modFoldersListWidgetRemoveActionTriggered);

    menu->exec(this->modFoldersListWidget->mapToGlobal(pos));
}

void OptionsTab::modFoldersListWidgetRemoveActionTriggered(bool checked)
{
    QListWidgetItem *item = this->modFoldersListWidget->currentItem();
    if (item == nullptr) {
        return;
    }

    QModelIndex index = this->modFoldersListWidget->indexFromItem(item);
    if (!index.isValid()) {
        return;
    }

    QListWidgetItem *itemTaken = this->modFoldersListWidget->takeItem(index.row());
    Q_ASSERT(itemTaken == item);

    delete itemTaken;

    this->settings->save();
    this->modsTab->refreshMods();
}

void OptionsTab::modFoldersAddPushButtonClicked(bool checked)
{
    QString folder = QFileDialog::getExistingDirectory(this->modFoldersAddPushButton, tr("Select Mod Folder"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (folder.isEmpty()) {
        return;
    }

    insertIntoModFoldersListWidget(folder);
    this->settings->save();
    this->modsTab->refreshMods();
}

void OptionsTab::browseArma3ExecutablePushButtonClicked(bool checked)
{
    QString filename = QFileDialog::getOpenFileName(this->browseArma3ExecutablePushButton, "Select Arma 3 Executable", QString(), tr("Executable Files (*.exe)"));
    if (filename.isEmpty()) {
        return;
    }

    setArma3Executable(filename);
    this->settings->save();
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
    this->arma3ExecutableLineEdit->setToolTip(path);
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

void OptionsTab::insertIntoModFoldersListWidget(QString text, int row)
{
    QListWidgetItem *item = new QListWidgetItem();
    item->setText(text);
    item->setToolTip(text);

    if (row == -1) {
        this->modFoldersListWidget->addItem(item);
        return;
    }

    this->modFoldersListWidget->insertItem(row, item);
}
