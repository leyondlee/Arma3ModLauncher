#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QApplication>
#include <QCheckBox>

#include "settings.h"
#include "modstab.h"
#include "optionstab.h"
#include "launcher.h"

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
    QTabWidget *tabWidget;
    QCheckBox *battleyeCheckBox;
    Settings *settings;
    ModsTab *modsTab;
    OptionsTab *optionsTab;
    Launcher *launcher;

    void init();
    void loadBattleyeFromSettings();
    void tabWidgetCurrentChangedHandler(int index);
    void applicationAboutToQuitHandler();
    void battleyeCheckBoxStateChangedHandler(int state);
};
#endif // MAINWINDOW_H
