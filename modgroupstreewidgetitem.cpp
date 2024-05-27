#include "modgroupstreewidgetitem.h"

ModGroupsTreeWidgetItem::ModGroupsTreeWidgetItem(QString name, QVariant data, bool isFolder)
    : QTreeWidgetItem{}
{
    init(name, data, isFolder);
}

bool ModGroupsTreeWidgetItem::isFolder()
{
    return this->folder;
}

void ModGroupsTreeWidgetItem::init(QString name, QVariant data, bool isFolder)
{
    this->folder = isFolder;
    if (this->folder) {
        this->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirHomeIcon));
        this->setFlags(this->flags() & ~Qt::ItemIsDragEnabled);
    }

    this->setText(0, name);
    this->setToolTip(0, name);
    this->setData(0, Qt::UserRole, data);
    this->setCheckState(0, Qt::Unchecked);
}
