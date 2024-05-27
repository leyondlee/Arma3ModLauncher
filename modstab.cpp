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
}

void ModsTab::loadAvailableMods()
{
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

            QString path = Util::joinPaths({canonicalPath, filename});
            modItem->setData(0, Qt::UserRole, path);
        }
    }

    this->availableModsTreeWidget->sortItems(0, Qt::AscendingOrder);
}
