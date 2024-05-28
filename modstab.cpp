#include "modstab.h"

ModsTab::ModsTab(QWidget *modsTab, Settings *settings)
    : QObject{modsTab}
{
    this->settings = settings;
    this->availableModsTreeWidget = modsTab->findChild<AvailableModsTreeWidget *>("availableModsTreeWidget");
    this->modGroupsTreeWidget = modsTab->findChild<ModGroupsTreeWidget *>("modGroupsTreeWidget");

    init();
}

void ModsTab::init()
{
    this->modGroupsTreeWidget->setAcceptDrops(true);
    this->modGroupsTreeWidget->setAvailableModsTreeWidget(availableModsTreeWidget);

    connect(this->modGroupsTreeWidget, &ModGroupsTreeWidget::treeChangedSignal, this, &ModsTab::modGroupsTreeChangedHandler);
    connect(this->modGroupsTreeWidget, &ModGroupsTreeWidget::itemCheckStateChangedSignal, this, &ModsTab::modGroupsTreeItemCheckStateChangedHandler);

    loadAvailableMods();
    loadModGroups();
}

void ModsTab::loadAvailableMods()
{
    this->availableModsTreeWidget->blockSignals(true);
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

    this->availableModsTreeWidget->doSort();
    this->availableModsTreeWidget->blockSignals(false);
}

void ModsTab::loadModGroups()
{
    this->modGroupsTreeWidget->blockSignals(true);
    this->modGroupsTreeWidget->clear();

    QVariant modGroupsSettings = this->settings->get(MODGROUPS_KEY);
    if (modGroupsSettings.isNull() || !modGroupsSettings.canConvert<QString>()) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(modGroupsSettings.toString().toUtf8());
    if (jsonDocument.isNull()) {
        return;
    }

    QJsonObject jsonObject = jsonDocument.object();
    for (auto &key : jsonObject.keys()) {
        QJsonValue jsonValue = jsonObject.value(key);
        if (!jsonValue.isArray()) {
            continue;
        }

        ModGroupsTreeWidgetItem *folderItem = this->modGroupsTreeWidget->addFolder(key);

        QJsonArray values = jsonValue.toArray();
        for (auto value : values) {
            if (!value.isObject()) {
                continue;
            }

            QJsonObject valueObject = value.toObject();
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

            QString path = pathJsonValue.toString();
            ModGroupsTreeWidgetItem *item = folderItem->addChildModItem(path);
            if (item == nullptr) {
                continue;
            }

            if (isCheckedJsonValue.toBool()) {
                item->setCheckState(Qt::Checked);
            }

            if (!this->availableModsTreeWidget->hasItem(Util::getFilename(path), QVariant(path), 0)) {
                item->setMissing(true);
                continue;
            }
        }
    }

    this->modGroupsTreeWidget->doSort();
    this->modGroupsTreeWidget->blockSignals(false);
}

void ModsTab::modGroupsTreeChangedHandler()
{
    this->settings->save();
}

void ModsTab::modGroupsTreeItemCheckStateChangedHandler(ModGroupsTreeWidgetItem *item)
{
    qDebug() << item->text(0);
}
