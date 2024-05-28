#include "modstab.h"

ModsTab::ModsTab(QWidget *modsTab, Settings *settings)
    : QObject{modsTab}
{
    this->settings = settings;
    this->availableModsTreeWidget = modsTab->findChild<AvailableModsTreeWidget *>("availableModsTreeWidget");
    this->modGroupsTreeWidget = modsTab->findChild<ModGroupsTreeWidget *>("modGroupsTreeWidget");
    this->availableModsExpandAllCheckBox = modsTab->findChild<QCheckBox *>("availableModsExpandAllCheckBox");
    this->modGroupsExpandAllCheckBox = modsTab->findChild<QCheckBox *>("modGroupsExpandAllCheckBox");
    this->modGroupsSelectAllCheckBox = modsTab->findChild<QCheckBox *>("modGroupsSelectAllCheckBox");
    this->refreshModsPushButton = modsTab->findChild<QPushButton *>("refreshModsPushButton");

    init();
}

void ModsTab::refreshMods()
{
    loadAvailableMods();
    loadModGroups();
}

void ModsTab::init()
{
    this->modGroupsTreeWidget->setAcceptDrops(true);
    this->modGroupsTreeWidget->setAvailableModsTreeWidget(availableModsTreeWidget);

    connect(this->modGroupsTreeWidget, &ModGroupsTreeWidget::treeChangedSignal, this, &ModsTab::modGroupsTreeChangedHandler);
    connect(this->modGroupsTreeWidget, &ModGroupsTreeWidget::itemCheckStateChangedSignal, this, &ModsTab::modGroupsTreeItemCheckStateChangedHandler);
    connect(this->availableModsExpandAllCheckBox, &QCheckBox::stateChanged, this, &ModsTab::availableModsExpandAllCheckBoxStateChanged);
    connect(this->modGroupsExpandAllCheckBox, &QCheckBox::stateChanged, this, &ModsTab::modGroupsExpandAllCheckBoxStateChanged);
    connect(this->modGroupsSelectAllCheckBox, &QCheckBox::stateChanged, this, &ModsTab::modGroupsSelectAllCheckBoxStateChanged);
    connect(this->refreshModsPushButton, &QPushButton::clicked, this, &ModsTab::refreshPushButtonClicked);

    refreshMods();
}

void ModsTab::loadAvailableMods()
{
    this->availableModsTreeWidget->clear();

    QVariant modFoldersSettings = this->settings->get(MODFOLDERS_KEY);
    if (modFoldersSettings.isNull() || !modFoldersSettings.canConvert<QStringList>()) {
        return;
    }

    QStringList modFolders;
    for (auto &folder : modFoldersSettings.toStringList()) {
        if (modFolders.contains(folder)) {
            continue;
        }

        modFolders.append(folder);
    }

    this->availableModsTreeWidget->blockSignals(true);
    for (auto &folder : modFolders) {
        QTreeWidgetItem *folderItem = new QTreeWidgetItem(this->availableModsTreeWidget);
        folderItem->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirHomeIcon));
        folderItem->setFlags(folderItem->flags() & ~Qt::ItemIsDragEnabled);
        folderItem->setData(0, Qt::UserRole, folder);

        QString name = Util::getFilename(folder);
        folderItem->setText(0, name);
        folderItem->setToolTip(0, name);

        QString canonicalPath = QDir(folder).canonicalPath();
        QDir dir(canonicalPath);
        if (!dir.exists()) {
            continue;
        }

        QDirIterator it(dir);
        while (it.hasNext()) {
            QFileInfo fileInfo = it.nextFileInfo();
            if (!fileInfo.isDir()) {
                continue;
            }

            QString filename = fileInfo.fileName();
            if (!filename.startsWith("@")) {
                continue;
            }

            QTreeWidgetItem *modItem = new QTreeWidgetItem(folderItem);
            modItem->setText(0, filename);
            modItem->setToolTip(0, filename);

            QString path = Util::joinPaths({folder, filename});
            modItem->setData(0, Qt::UserRole, path);
        }
    }
    this->availableModsTreeWidget->blockSignals(false);
    this->availableModsTreeWidget->doSort();
}

void ModsTab::loadModGroups()
{
    this->modGroupsTreeWidget->clear();

    QVariant modGroupsSettings = this->settings->get(MODGROUPS_KEY);
    if (modGroupsSettings.isNull() || !modGroupsSettings.canConvert<QJsonObject>()) {
        return;
    }

    this->modGroupsTreeWidget->blockSignals(true);
    QJsonObject jsonObject = modGroupsSettings.toJsonObject();
    for (auto &key : jsonObject.keys()) {
        QJsonValue jsonValue = jsonObject.value(key);
        if (!jsonValue.isObject()) {
            continue;
        }

        QJsonObject folderObject = jsonValue.toObject();
        if (!folderObject.contains(MODGROUPS_ISEXPANDED_KEY) || !folderObject.contains(MODGROUPS_MODS_KEY)) {
            continue;
        }

        QJsonValue isExpandedJsonValue = folderObject.value(MODGROUPS_ISEXPANDED_KEY);
        if (!isExpandedJsonValue.isBool()) {
            continue;
        }

        QJsonValue modsJsonValue = folderObject.value(MODGROUPS_MODS_KEY);
        if (!modsJsonValue.isArray()) {
            continue;
        }

        ModGroupsTreeWidgetItem *folderItem = this->modGroupsTreeWidget->addFolder(key);
        folderItem->setExpanded(isExpandedJsonValue.toBool());

        bool hasChecked = false;
        bool hasUnchecked = false;
        QJsonArray mods = modsJsonValue.toArray();
        for (auto mod : mods) {
            if (!mod.isObject()) {
                continue;
            }

            QJsonObject valueObject = mod.toObject();
            if (!(valueObject.contains(MODGROUPS_ISCHECKED_KEY)) || !(valueObject.contains(MODGROUPS_PATH_KEY))) {
                continue;
            }

            QJsonValue isCheckedJsonValue = valueObject.value(MODGROUPS_ISCHECKED_KEY);
            if (!isCheckedJsonValue.isBool()) {
                continue;
            }

            QJsonValue pathJsonValue = valueObject.value(MODGROUPS_PATH_KEY);
            if (!pathJsonValue.isString()) {
                continue;
            }

            QString path = Util::cleanPath(pathJsonValue.toString());
            ModGroupsTreeWidgetItem *item = folderItem->addModToFolder(path);
            if (item == nullptr) {
                continue;
            }

            if (isCheckedJsonValue.toBool()) {
                hasChecked = true;
                item->setCheckState(Qt::Checked);
            } else {
                hasUnchecked = true;
            }

            if (!this->availableModsTreeWidget->hasItem(Util::getFilename(path), QVariant(path), 0)) {
                item->setMissing(true);
                continue;
            }
        }

        if (hasChecked && hasUnchecked) {
            folderItem->setCheckState(Qt::PartiallyChecked);
        } else if (hasChecked) {
            Q_ASSERT(!hasUnchecked);
            folderItem->setCheckState(Qt::Checked);
        } else {
            Q_ASSERT(!hasChecked);
            folderItem->setCheckState(Qt::Unchecked);
        }
    }
    this->modGroupsTreeWidget->blockSignals(false);
    this->modGroupsTreeWidget->doSort();
}

void ModsTab::modGroupsTreeChangedHandler()
{
    this->settings->save();
}

void ModsTab::modGroupsTreeItemCheckStateChangedHandler(ModGroupsTreeWidgetItem *item)
{
    if (item->isFolder()) {
        this->modGroupsTreeWidget->blockSignals(true);
        for (int i = 0; i < item->childCount(); i += 1) {
            ModGroupsTreeWidgetItem *childItem = ModGroupsTreeWidgetItem::castTreeWidgetItem(item->child(i));
            if (childItem == nullptr) {
                continue;
            }

            childItem->setCheckState(item->checkState(0));
        }
        this->modGroupsTreeWidget->blockSignals(false);

        return;
    }

    ModGroupsTreeWidgetItem *parentItem = ModGroupsTreeWidgetItem::castTreeWidgetItem(item->parent());
    if (parentItem == nullptr || parentItem->parent() != nullptr) {
        return;
    }

    this->modGroupsTreeWidget->blockSignals(true);
    if (parentItem->haveAllChildrenChecked()) {
        parentItem->setCheckState(Qt::Checked);
    } else if (parentItem->getChildrenCheckedCount() > 0) {
        parentItem->setCheckState(Qt::PartiallyChecked);
    } else {
        parentItem->setCheckState(Qt::Unchecked);
    }
    this->modGroupsTreeWidget->blockSignals(false);

    this->settings->save();
}

void ModsTab::availableModsExpandAllCheckBoxStateChanged(int state)
{
    if (state == Qt::Checked) {
        this->availableModsTreeWidget->expandAll();
        return;
    }

    this->availableModsTreeWidget->collapseAll();
}

void ModsTab::modGroupsExpandAllCheckBoxStateChanged(int state)
{
    if (state == Qt::Checked) {
        this->modGroupsTreeWidget->expandAll();
        return;
    }

    this->modGroupsTreeWidget->collapseAll();
}

void ModsTab::modGroupsSelectAllCheckBoxStateChanged(int state)
{
    this->modGroupsTreeWidget->blockSignals(true);
    QTreeWidgetItemIterator it(this->modGroupsTreeWidget);
    while (*it) {
        ModGroupsTreeWidgetItem *item = ModGroupsTreeWidgetItem::castTreeWidgetItem(*it);
        if (item != nullptr) {
            item->setCheckState(Qt::CheckState(state));
        }

        ++it;
    }
    this->modGroupsTreeWidget->blockSignals(false);

    this->settings->save();
}

void ModsTab::refreshPushButtonClicked(bool checked)
{
    refreshMods();
}
