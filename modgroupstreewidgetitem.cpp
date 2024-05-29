#include "modgroupstreewidgetitem.h"

ModGroupsTreeWidgetItem::ModGroupsTreeWidgetItem(QString name, QVariant data, bool isFolder)
    : QTreeWidgetItem{}
{
    this->folder = isFolder;
    this->isMissing = false;
    this->originalColor = this->foreground(0).color();

    this->setText(0, name);
    this->setToolTip(0, name);
    this->setData(0, Qt::UserRole, data);
    this->setCheckState(Qt::Unchecked);

    if (this->folder) {
        this->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirHomeIcon));
        this->setFlags(this->flags() & ~Qt::ItemIsDragEnabled);
    }
}

bool ModGroupsTreeWidgetItem::isFolder()
{
    return this->folder;
}

ModGroupsTreeWidgetItem *ModGroupsTreeWidgetItem::addModToFolder(QString path)
{
    if (!this->isFolder()) {
        return nullptr;
    }

    QString cleanPath = Util::cleanPath(path);
    QString name = Util::getFilename(cleanPath);
    QVariant data(cleanPath);
    if (Util::hasItemInTreeWidgetItem(this, name, data, 0)) {
        return nullptr;
    }

    ModGroupsTreeWidgetItem *newItem = new ModGroupsTreeWidgetItem(name, data, false);
    newItem->setToolTip(0, path);
    this->addChild(newItem);

    return newItem;
}

void ModGroupsTreeWidgetItem::setMissing(bool isMissing)
{
    this->isMissing = isMissing;
    if (this->isMissing) {
        this->setForeground(0, Qt::red);
        return;
    }

    this->setForeground(0, this->originalColor);
}

ModGroupsTreeWidgetItem *ModGroupsTreeWidgetItem::castTreeWidgetItem(QTreeWidgetItem *item)
{
    if (item == nullptr) {
        return nullptr;
    }

    return dynamic_cast<ModGroupsTreeWidgetItem *>(item);
}

void ModGroupsTreeWidgetItem::setCheckState(Qt::CheckState state)
{
    QTreeWidgetItem::setCheckState(0, state);
    updateSavedCheckState();
}

bool ModGroupsTreeWidgetItem::hasCheckStateChanged()
{
    return this->savedCheckState != this->checkState(0);
}

void ModGroupsTreeWidgetItem::updateSavedCheckState()
{
    this->savedCheckState = this->checkState(0);
}

int ModGroupsTreeWidgetItem::getChildrenCheckedCount()
{
    int count = 0;
    for (int i = 0; i < this->childCount(); i += 1) {
        if (this->child(i)->checkState(0) != Qt::Checked) {
            continue;
        }

        count += 1;
    }

    return count;
}

bool ModGroupsTreeWidgetItem::isAllChildrenChecked()
{
    return getChildrenCheckedCount() == this->childCount();
}
