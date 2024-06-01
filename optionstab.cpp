#include "optionstab.h"

OptionsTab::OptionsTab(QWidget *optionsTab, ModsTab *modsTab, Settings *settings)
    : QObject{optionsTab}
{
    this->modsTab = modsTab;
    this->settings = settings;
    this->arma3ExecutableLineEdit = optionsTab->findChild<QLineEdit *>("arma3ExecutableLineEdit");
    this->arma3ExecutableBrowsePushButton = optionsTab->findChild<QPushButton *>("arma3ExecutableBrowsePushButton");
    this->parametersGroupBox = optionsTab->findChild<QGroupBox *>("parametersGroupBox");
    this->additionalParametersListWidget = optionsTab->findChild<QListWidget *>("additionalParametersListWidget");
    this->additionalParametersAddPushButton = optionsTab->findChild<QPushButton *>("additionalParametersAddPushButton");
    this->modFoldersListWidget = optionsTab->findChild<QListWidget *>("modFoldersListWidget");
    this->modFoldersAddPushButton = optionsTab->findChild<QPushButton *>("modFoldersAddPushButton");

    init();
}

void OptionsTab::tabChanged()
{
    this->arma3ExecutableLineEdit->setCursorPosition(0);

    refreshTab();
}

void OptionsTab::refreshTab()
{
    loadArma3Executable();
    loadParameters();
    loadAdditionalParameters();
    loadModFolders();
}

void OptionsTab::init()
{
    initParameterCheckBoxes();

    this->arma3ExecutableLineEdit->setReadOnly(true);
    this->additionalParametersListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->additionalParametersListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    this->modFoldersListWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this->arma3ExecutableBrowsePushButton, &QPushButton::clicked, this, &OptionsTab::arma3ExecutableBrowsePushButtonClicked);
    connect(this->additionalParametersListWidget, &QListWidget::customContextMenuRequested, this, &OptionsTab::additionalParametersListWidgetCustomContextMenuRequestedHandler);
    connect(this->additionalParametersAddPushButton, &QPushButton::clicked, this, &OptionsTab::additionalParametersAddPushButtonClicked);
    connect(this->modFoldersListWidget, &QListWidget::customContextMenuRequested, this, &OptionsTab::modFoldersListWidgetCustomContextMenuRequestedHandler);
    connect(this->modFoldersAddPushButton, &QPushButton::clicked, this, &OptionsTab::modFoldersAddPushButtonClicked);

    refreshTab();
}

void OptionsTab::initParameterCheckBoxes()
{
    for (QHash<QString, QString>::iterator i = this->parameterCheckBoxMap.begin(); i != this->parameterCheckBoxMap.end(); ++i) {
        QCheckBox *checkBox = parametersGroupBox->findChild<QCheckBox *>(i.value());
        if (checkBox == nullptr) {
            continue;
        }

        checkBox->setCheckState(Qt::Unchecked);
        checkBox->setProperty(PARAMETER_CHECKBOX_PROPERTY, i.key());

        connect(checkBox, &QCheckBox::stateChanged, this, &OptionsTab::parametersCheckBoxStateChanged);
    }
}

void OptionsTab::loadArma3Executable()
{
    QJsonValue arma3ExecutableSetting = this->settings->get(ARMA3EXECUTABLE_KEY);
    if (arma3ExecutableSetting.isNull() || !arma3ExecutableSetting.isString()) {
        // Detect and set Arma 3 executable
        QString detectedArma3Folder = getDetectedArma3Folder();
        if (detectedArma3Folder.isEmpty()) {
            setArma3Executable("");
            return;
        }

        QString arma3Executable = Util::joinPaths(QStringList({detectedArma3Folder, ARMA3_EXECUTABLE}));
        setArma3Executable(arma3Executable);

        // Add default workshop path
        QString workshopPath = Util::joinPaths(QStringList({detectedArma3Folder, WORKSHOP_FOLDER}));
        addToModFoldersList(workshopPath, 0);

        this->settings->save();
        this->modsTab->refreshTab();
        return;
    }

    Q_ASSERT(arma3ExecutableSetting.isString());
    setArma3Executable(arma3ExecutableSetting.toString());
}

void OptionsTab::loadParameters()
{
    QJsonValue parametersSetting = this->settings->get(PARAMETERS_KEY);
    if (parametersSetting.isNull() || !parametersSetting.isArray()) {
        return;
    }

    QJsonArray parametersArray = parametersSetting.toArray();
    for (auto childObject : this->parametersGroupBox->children()) {
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(childObject);
        if (checkBox == nullptr) {
            continue;
        }

        QVariant checkBoxParameter = checkBox->property(PARAMETER_CHECKBOX_PROPERTY);
        if (checkBoxParameter.isNull() || !checkBoxParameter.canConvert<QString>()) {
            continue;
        }

        checkBox->blockSignals(true);
        if (parametersArray.contains(checkBoxParameter.toString())) {
            checkBox->setCheckState(Qt::Checked);
        } else {
            checkBox->setCheckState(Qt::Unchecked);
        }
        checkBox->blockSignals(false);
    }
}

void OptionsTab::loadAdditionalParameters()
{
    this->additionalParametersListWidget->clear();

    QJsonValue additionalParametersSettings = this->settings->get(ADDITIONALPARAMETERS_KEY);
    if (additionalParametersSettings.isNull() || !additionalParametersSettings.isArray()) {
        return;
    }

    QJsonArray additionalParametersArray = additionalParametersSettings.toArray();
    for (auto value : additionalParametersArray) {
        if (!value.isString()) {
            continue;
        }

        addToAdditionalParametersList(value.toString());
    }
}

void OptionsTab::loadModFolders()
{
    this->modFoldersListWidget->clear();

    QJsonValue modFoldersSettings = this->settings->get(MODFOLDERS_KEY);
    if (modFoldersSettings.isNull() || !modFoldersSettings.isObject()) {
        return;
    }

    QJsonObject jsonObject = modFoldersSettings.toObject();
    for (auto &key : jsonObject.keys()) {
        addToModFoldersList(key);
    }
}

void OptionsTab::arma3ExecutableBrowsePushButtonClicked(bool checked)
{
    QString filename = QFileDialog::getOpenFileName(this->arma3ExecutableBrowsePushButton, "Select Arma 3 Executable", QString(), tr("Executable Files (*.exe)")).trimmed();
    if (filename.isEmpty()) {
        return;
    }

    setArma3Executable(filename);
    this->settings->save();
}

void OptionsTab::parametersCheckBoxStateChanged(int state)
{
    this->settings->save();
}

void OptionsTab::additionalParametersListWidgetCustomContextMenuRequestedHandler(QPoint pos)
{
    QListWidgetItem *item = this->additionalParametersListWidget->itemAt(pos);
    if (item == nullptr) {
        return;
    }

    QMenu *menu = new QMenu(this->additionalParametersListWidget);

    QAction *editAction = new QAction("Edit", menu);
    menu->addAction(editAction);
    connect(editAction, &QAction::triggered, this, &OptionsTab::additionalParametersEditActionTriggered);

    QAction *removeAction = new QAction("Remove", menu);
    menu->addAction(removeAction);
    connect(removeAction, &QAction::triggered, this, &OptionsTab::additionalParametersRemoveActionTriggered);

    if (this->additionalParametersListWidget->selectedItems().count() > 1) {
        editAction->setEnabled(false);
    }

    menu->exec(this->additionalParametersListWidget->mapToGlobal(pos));
}

void OptionsTab::additionalParametersEditActionTriggered(bool checked)
{
    if (this->additionalParametersListWidget->selectedItems().size() > 1) {
        return;
    }

    QListWidgetItem *item = this->additionalParametersListWidget->currentItem();
    if (item == nullptr) {
        return;
    }

    QString value = item->text();

    bool ok;
    QString newValue = QInputDialog::getText(this->additionalParametersListWidget, tr("Edit Parameter"), tr("Value:"), QLineEdit::Normal, value, &ok).trimmed();
    if (!ok || newValue.isEmpty() || QString::compare(newValue, value) == 0) {
        return;
    }

    if (Util::hasItemInListWidget(this->additionalParametersListWidget, newValue, QVariant())) {
        Util::showWarningMessage("Edit Parameter", "Parameter already exists.", this->additionalParametersListWidget);
        return;
    }

    item->setText(newValue);
    item->setToolTip(newValue);

    this->settings->save();
}

void OptionsTab::additionalParametersRemoveActionTriggered(bool checked)
{
    QList<QListWidgetItem *> selectedItems = this->additionalParametersListWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }

    for (auto item : selectedItems) {
        int index = this->additionalParametersListWidget->row(item);
        if (index < 0) {
            continue;
        }

        QListWidgetItem *itemTaken = this->additionalParametersListWidget->takeItem(index);
        Q_ASSERT(itemTaken == item);

        delete itemTaken;
    }

    this->settings->save();
}

void OptionsTab::additionalParametersAddPushButtonClicked(bool checked)
{
    bool ok;
    QString value = QInputDialog::getText(this->additionalParametersAddPushButton, tr("Add Parameter"), tr("Value:"), QLineEdit::Normal, tr(""), &ok).trimmed();
    if (!ok || value.isEmpty()) {
        return;
    }

    if (!addToAdditionalParametersList(value)) {
        Util::showWarningMessage("Add Parameter", "Parameter already exists.", this->additionalParametersAddPushButton);
        return;
    }

    this->settings->save();
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
    connect(removeAction, &QAction::triggered, this, &OptionsTab::modFoldersRemoveActionTriggered);

    menu->exec(this->modFoldersListWidget->mapToGlobal(pos));
}

void OptionsTab::modFoldersRemoveActionTriggered(bool checked)
{
    QListWidgetItem *item = this->modFoldersListWidget->currentItem();
    if (item == nullptr) {
        return;
    }

    int index = this->modFoldersListWidget->row(item);
    if (index < 0) {
        return;
    }

    QListWidgetItem *itemTaken = this->modFoldersListWidget->takeItem(index);
    Q_ASSERT(itemTaken == item);

    delete itemTaken;

    this->settings->save();
    this->modsTab->refreshTab();
}

void OptionsTab::modFoldersAddPushButtonClicked(bool checked)
{
    QString folder = QFileDialog::getExistingDirectory(this->modFoldersAddPushButton, tr("Select Mod Folder"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks).trimmed();
    if (folder.isEmpty()) {
        return;
    }

    if (!addToModFoldersList(folder)) {
        Util::showWarningMessage("Select Mod Folder", "Folder already exists.", this->modFoldersAddPushButton);
        return;
    }

    this->settings->save();
    this->modsTab->refreshTab();
}

QString OptionsTab::getDetectedArma3Folder()
{
    QSettings settings(ARMA3_REGISTRY, QSettings::NativeFormat);
    QString folderName = settings.value("main", "").toString();

    return folderName;
}

void OptionsTab::setArma3Executable(QString path)
{
    QString cleanPath = Util::cleanPath(path);
    this->arma3ExecutableLineEdit->setText(cleanPath);
    this->arma3ExecutableLineEdit->setToolTip(cleanPath);
    this->arma3ExecutableLineEdit->setCursorPosition(0);
}

bool OptionsTab::addToAdditionalParametersList(QString value)
{
    QString valueTrimmed = value.trimmed();
    if (valueTrimmed.isEmpty()) {
        return false;
    }

    if (Util::hasItemInListWidget(this->additionalParametersListWidget, valueTrimmed, QVariant())) {
        return false;
    }

    QListWidgetItem *item = new QListWidgetItem();
    item->setText(valueTrimmed);
    item->setToolTip(valueTrimmed);
    this->additionalParametersListWidget->addItem(item);

    return true;
}

bool OptionsTab::addToModFoldersList(QString path, int row)
{
    if (path.isEmpty()) {
        return false;
    }

    QString cleanPath = Util::cleanPath(path);
    if (Util::hasItemInListWidget(this->modFoldersListWidget, cleanPath, QVariant())) {
        return false;
    }

    QListWidgetItem *item = new QListWidgetItem();
    item->setText(cleanPath);
    item->setToolTip(cleanPath);

    if (row >= 0) {
        this->modFoldersListWidget->insertItem(row, item);
    } else {
        this->modFoldersListWidget->addItem(item);
    }

    return true;
}
