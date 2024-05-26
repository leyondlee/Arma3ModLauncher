#include "modstab.h"
#include "qheaderview.h"

ModsTab::ModsTab(QMainWindow *mainWindow, Settings *settings)
    : QObject{mainWindow}
{
    this->settings = settings;
    this->mainWindow = mainWindow;

    QWidget *modsTab = this->mainWindow->findChild<QWidget *>("modsTab");
    this->availableModsTreeWidget = modsTab->findChild<CustomTreeWidget *>("availableModsTreeWidget");
    this->modGroupsTreeWidget = modsTab->findChild<CustomTreeWidget *>("modGroupsTreeWidget");

    init();
}

void ModsTab::init()
{
    this->availableModsTreeWidget->setHeaderHidden(true);
    this->availableModsTreeWidget->header()->setStretchLastSection(false);
    this->availableModsTreeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    this->availableModsTreeWidget->setType(CustomTreeWidget::Type::Sender);

    this->modGroupsTreeWidget->setHeaderHidden(true);
    this->modGroupsTreeWidget->header()->setStretchLastSection(false);
    this->modGroupsTreeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    this->modGroupsTreeWidget->setType(CustomTreeWidget::Receiver);
    this->modGroupsTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this->modGroupsTreeWidget, &CustomTreeWidget::dragEnterEventSignal, this, &ModsTab::modGroupsTreeWidgetDragEnterEventSignal);
    connect(this->modGroupsTreeWidget, &CustomTreeWidget::dragMoveEventSignal, this, &ModsTab::modGroupsTreeWidgetDragMoveEventSignal);
    connect(this->modGroupsTreeWidget, &CustomTreeWidget::dropEventSignal, this, &ModsTab::modGroupsTreeWidgetDropEventSignal);
    connect(this->modGroupsTreeWidget, &CustomTreeWidget::customContextMenuRequested, this, &ModsTab::modGroupsTreeWidgetCustomContextMenuRequested);
}

void ModsTab::modGroupsTreeWidgetDragEnterEventSignal(QDragEnterEvent *event)
{
    CustomTreeWidget *sourceTreeWidget = qobject_cast<CustomTreeWidget *>(event->source());
    if (sourceTreeWidget == nullptr || (sourceTreeWidget != this->availableModsTreeWidget && sourceTreeWidget != this->modGroupsTreeWidget)) {
        return;
    }

    event->acceptProposedAction();
}

void ModsTab::modGroupsTreeWidgetDragMoveEventSignal(QDragMoveEvent *event)
{
    this->modGroupsTreeWidget->clearSelection();

    QTreeWidgetItem *item = this->modGroupsTreeWidget->itemAt(event->position().toPoint());
    if (item == nullptr) {
        return;
    }

    if (!item->data(0, Qt::UserRole).isNull()) {
        return;
    }

    item->setSelected(true);
}

void ModsTab::modGroupsTreeWidgetDropEventSignal(QDropEvent *event)
{
    CustomTreeWidget *sourceTreeWidget = qobject_cast<CustomTreeWidget *>(event->source());
    if (sourceTreeWidget == nullptr || (sourceTreeWidget != this->availableModsTreeWidget && sourceTreeWidget != this->modGroupsTreeWidget)) {
        return;
    }

    QTreeWidgetItem *currentItem = sourceTreeWidget->currentItem();
    QVariant data = currentItem->data(0, Qt::UserRole);
    if (data.isNull() || !data.canConvert<QString>()) {
        return;
    }

    QString path = data.toString();
    QString name = Util::getFilename(path);

    QTreeWidgetItem *parentItem = this->modGroupsTreeWidget->itemAt(event->position().toPoint());
    QTreeWidgetItem *item;
    if (parentItem == nullptr) {
        item = new QTreeWidgetItem(this->modGroupsTreeWidget);
    } else {
        while (parentItem->parent() != nullptr) {
            parentItem = parentItem->parent();
        }

        if (!parentItem->data(0, Qt::UserRole).isNull()) {
            return;
        }

        item = new QTreeWidgetItem(parentItem);
        parentItem->setExpanded(true);
    }

    item->setText(0, name);
    item->setData(0, Qt::UserRole, data);
    item->setCheckState(0, Qt::Unchecked);

    this->modGroupsTreeWidget->sortItems(0, Qt::AscendingOrder);

    if (sourceTreeWidget == this->modGroupsTreeWidget) {
        QTreeWidgetItem *currentItemParent = currentItem->parent();
        if (currentItemParent == nullptr) {
            int index = this->modGroupsTreeWidget->indexOfTopLevelItem(currentItem);
            if (index == -1) {
                return;
            }

            QTreeWidgetItem *item = this->modGroupsTreeWidget->takeTopLevelItem(index);
            if (item == nullptr) {
                return;
            }

            delete item;
            return;
        }

        currentItemParent->removeChild(currentItem);
        delete currentItem;
    }
}

void ModsTab::modGroupsTreeWidgetCustomContextMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this->modGroupsTreeWidget);

    QAction *addFolderAction = new QAction("Add Folder", menu);
    menu->addAction(addFolderAction);
    connect(addFolderAction, &QAction::triggered, this, &ModsTab::modGroupsTreeAddFolderActionTriggered);

    menu->exec(this->modGroupsTreeWidget->mapToGlobal(pos));
}

void ModsTab::modGroupsTreeAddFolderActionTriggered(bool checked)
{
    bool ok;
    QString name = QInputDialog::getText(this->modGroupsTreeWidget, tr("Add Folder"), tr("Name:"), QLineEdit::Normal, tr(""), &ok);

    if (!ok || name.isEmpty()) {
        return;
    }

    if (this->modGroupsTreeWidget->hasItem(name, QVariant(), 0)) {
        QMessageBox messageBox(QMessageBox::Warning, "Warning", "Folder already exists.", QMessageBox::NoButton, this->modGroupsTreeWidget);
        messageBox.exec();
        return;
    }

    QTreeWidgetItem *folderItem = new QTreeWidgetItem(this->modGroupsTreeWidget);
    folderItem->setIcon(0, this->mainWindow->style()->standardIcon(QStyle::SP_DirHomeIcon));
    folderItem->setText(0, name);
    folderItem->setToolTip(0, name);
    folderItem->setCheckState(0, Qt::Unchecked);
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
        folderItem->setIcon(0, this->mainWindow->style()->standardIcon(QStyle::SP_DirHomeIcon));
        folderItem->setFlags(folderItem->flags() & ~Qt::ItemIsDragEnabled);

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
