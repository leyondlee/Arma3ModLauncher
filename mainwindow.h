#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QApplication>

#include "settings.h"
#include "modstab.h"
#include "optionstab.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Settings *settings;
    QTabWidget *tabWidget;
    ModsTab *modsTab;
    OptionsTab *optionsTab;

    void tabWidgetCurrentChangedHandler(int index);
    void applicationAboutToQuitHandler();
};
#endif // MAINWINDOW_H
