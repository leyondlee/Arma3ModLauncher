#include "modgroupstreewidgetitem.h"

ModGroupsTreeWidgetItem::ModGroupsTreeWidgetItem(QString name, QVariant data, bool isFolder)
    : QTreeWidgetItem{}
{
    this->folder = isFolder;
    this->isMissing = false;

    this->setText(0, name);
    this->setToolTip(0, name);
    this->setData(0, Qt::UserRole, data);
    this->setCheckState(0, Qt::Unchecked);

    if (this->folder) {
        this->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirHomeIcon));
        this->setFlags(this->flags() & ~Qt::ItemIsDragEnabled);
    }
}

bool ModGroupsTreeWidgetItem::isFolder()
{
    return this->folder;
}

ModGroupsTreeWidgetItem *ModGroupsTreeWidgetItem::addChildModItem(QString path)
{
    QString name = Util::getFilename(path);
    QVariant data(path);
    if (Util::hasItemInTreeWidgetItem(this, name, data, 0)) {
        return nullptr;
    }

    ModGroupsTreeWidgetItem *newItem = new ModGroupsTreeWidgetItem(name, data, false);
    this->addChild(newItem);

    return newItem;
}

void ModGroupsTreeWidgetItem::setMissing(bool isMissing)
{
    this->isMissing = isMissing;
    if (this->isMissing) {
        // TODO: color
        return;
    }
}
