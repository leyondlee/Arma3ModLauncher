#include "modstab.h"
#include "qheaderview.h"

ModsTab::ModsTab(QWidget *modsTab, Settings *settings)
    : QObject{modsTab}
{
    this->settings = settings;
    this->availableModsTreeWidget = modsTab->findChild<QTreeWidget *>("availableModsTreeWidget");
    this->modGroupsTreeWidget = modsTab->findChild<ModGroupsTreeWidget *>("modGroupsTreeWidget");

    init();
}

void ModsTab::init()
{
    this->availableModsTreeWidget->setHeaderHidden(true);
    this->availableModsTreeWidget->header()->setStretchLastSection(false);
    this->availableModsTreeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    this->availableModsTreeWidget->setAcceptDrops(false);
    this->availableModsTreeWidget->setDragEnabled(true);

    this->modGroupsTreeWidget->setAcceptDrops(true);
    this->modGroupsTreeWidget->setDragEnabled(true);
    this->modGroupsTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this->modGroupsTreeWidget, &ModGroupsTreeWidget::dragEnterSignal, this, &ModsTab::modGroupsDragEnterSignalHandler);
    connect(this->modGroupsTreeWidget, &ModGroupsTreeWidget::dragLeaveSignal, this, &ModsTab::modGroupsDragLeaveSignalHandler);
    connect(this->modGroupsTreeWidget, &ModGroupsTreeWidget::dragMoveSignal, this, &ModsTab::modGroupsDragMoveSignalHandler);
    connect(this->modGroupsTreeWidget, &ModGroupsTreeWidget::dropSignal, this, &ModsTab::modGroupsDropSignalHandler);
    connect(this->modGroupsTreeWidget, &ModGroupsTreeWidget::customContextMenuRequested, this, &ModsTab::modGroupsCustomContextMenuRequestedHandler);
}

bool ModsTab::hasItemInTreeWidget(QTreeWidget *treeWidget, QString text, QVariant data, int column)
{
    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        if (QString::compare((*it)->text(column), text) == 0 && (*it)->data(column, Qt::UserRole) == data) {
            return true;
        }

        ++it;
    }

    return false;
}

bool ModsTab::hasItemInTreeWidgetItem(QTreeWidgetItem *item, QString text, QVariant data, int column)
{
    for (int i = 0; i < item->childCount(); i += 1) {
        QTreeWidgetItem *childItem = item->child(i);
        if (childItem == nullptr) {
            continue;
        }

        if (QString::compare(childItem->text(0), text) != 0) {
            continue;
        }

        if (childItem->data(0, Qt::UserRole) == data) {
            return true;
        }
    }

    return false;
}

void ModsTab::removeTreeWidgetItem(QTreeWidgetItem *item)
{
    QTreeWidgetItem *parent = item->parent();
    if (parent == nullptr) {
        QTreeWidget *treeWidget = item->treeWidget();
        int index = treeWidget->indexOfTopLevelItem(item);
        if (index == -1) {
            return;
        }

        QTreeWidgetItem *topLevelItem = treeWidget->takeTopLevelItem(index);
        if (topLevelItem == nullptr) {
            return;
        }
        Q_ASSERT(topLevelItem == item);

        delete topLevelItem;
        return;
    }

    Q_ASSERT(parent->indexOfChild(item) >= 0);
    parent->removeChild(item);
    delete item;

    return;
}

void ModsTab::modGroupsDragEnterSignalHandler(QDragEnterEvent *event)
{
    QTreeWidget *sourceTreeWidget = qobject_cast<QTreeWidget *>(event->source());
    if (sourceTreeWidget == nullptr || (sourceTreeWidget != this->availableModsTreeWidget && sourceTreeWidget != this->modGroupsTreeWidget)) {
        return;
    }

    event->acceptProposedAction();
}

void ModsTab::modGroupsDragLeaveSignalHandler(QDragLeaveEvent *event)
{
    event->ignore();
}

void ModsTab::modGroupsDragMoveSignalHandler(QDragMoveEvent *event)
{
    this->modGroupsTreeWidget->clearSelection();

    QTreeWidgetItem *item = this->modGroupsTreeWidget->itemAt(event->position().toPoint());
    if (item == nullptr) {
        event->ignore();
        return;
    }

    while (item->parent() != nullptr) {
        item = item->parent();
    }

    if (!item->data(0, Qt::UserRole).isNull()) {
        event->ignore();
        return;
    }

    item->setSelected(true);
    event->acceptProposedAction();
}

void ModsTab::modGroupsDropSignalHandler(QDropEvent *event)
{
    QTreeWidget *sourceTreeWidget = qobject_cast<QTreeWidget *>(event->source());
    if (sourceTreeWidget == nullptr || (sourceTreeWidget != this->availableModsTreeWidget && sourceTreeWidget != this->modGroupsTreeWidget)) {
        return;
    }

    QTreeWidgetItem *parentItem = this->modGroupsTreeWidget->itemAt(event->position().toPoint());
    if (parentItem == nullptr) {
        return;
    }

    while (parentItem->parent() != nullptr) {
        parentItem = parentItem->parent();
    }

    ModGroupsTreeWidgetItem *parentItemCasted = dynamic_cast<ModGroupsTreeWidgetItem *>(parentItem);
    if (parentItemCasted == nullptr) {
        return;
    }

    if (!parentItemCasted->isFolder()) {
        return;
    }

    QTreeWidgetItem *currentItem = sourceTreeWidget->currentItem();
    if (currentItem->parent() == parentItem) {
        return;
    }

    QVariant data = currentItem->data(0, Qt::UserRole);
    if (data.isNull() || !data.canConvert<QString>()) {
        return;
    }

    QString path = data.toString();
    QString name = Util::getFilename(path);

    parentItem->setExpanded(true);

    if (hasItemInTreeWidgetItem(parentItem, name, data, 0)) {
        return;
    }

    ModGroupsTreeWidgetItem *newItem = new ModGroupsTreeWidgetItem(name, data, false);
    parentItem->addChild(newItem);

    if (sourceTreeWidget == this->modGroupsTreeWidget) {
        removeTreeWidgetItem(currentItem);
    }

    this->modGroupsTreeWidget->sortItems(0, Qt::AscendingOrder);
}

void ModsTab::modGroupsCustomContextMenuRequestedHandler(QPoint pos)
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

    if (hasItemInTreeWidget(this->modGroupsTreeWidget, name, QVariant(), 0)) {
        QMessageBox messageBox(QMessageBox::Warning, "Warning", "Folder already exists.", QMessageBox::NoButton, this->modGroupsTreeWidget);
        messageBox.exec();
        return;
    }

    ModGroupsTreeWidgetItem *folderItem = new ModGroupsTreeWidgetItem(name, QVariant(), true);
    this->modGroupsTreeWidget->addTopLevelItem(folderItem);
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
