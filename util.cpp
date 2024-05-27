#include "util.h"

QString Util::joinPaths(QStringList paths)
{
    return QDir::toNativeSeparators(QDir::cleanPath(paths.join(QDir::separator())));
}

QString Util::getFilename(QString path)
{
    return QFileInfo(path).fileName();
}

bool Util::hasItemInTreeWidgetItem(QTreeWidgetItem *item, QString text, QVariant data, int column)
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

void Util::removeTreeWidgetItem(QTreeWidgetItem *item)
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
