#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->tabWidget = ui->tabWidget;
    this->battleyeCheckBox = ui->battleyeCheckBox;
    this->settings = new Settings(this);
    this->modsTab = new ModsTab(ui->modsTab, this->settings);
    this->optionsTab = new OptionsTab(ui->optionsTab, this->modsTab, this->settings);
    this->launcher = new Launcher(this);

    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    this->setWindowTitle(QApplication::applicationName());
    this->tabWidget->setCurrentIndex(0);

    connect(this->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabWidgetCurrentChangedHandler);
    connect(QApplication::instance(), &QApplication::aboutToQuit, this, &MainWindow::applicationAboutToQuitHandler);
    connect(this->battleyeCheckBox, &QCheckBox::checkStateChanged, this, &MainWindow::battleyeCheckBoxStateChangedHandler);

    loadBattleyeFromSettings();
}

void MainWindow::loadBattleyeFromSettings()
{
    QJsonValue battleyeSettings = this->settings->get(BATTLEYE_KEY);
    if (battleyeSettings.isNull() || !battleyeSettings.isBool()) {
        return;
    }

    this->battleyeCheckBox->blockSignals(true);
    if (battleyeSettings.toBool()) {
        this->battleyeCheckBox->setCheckState(Qt::Checked);
    } else {
        this->battleyeCheckBox->setCheckState(Qt::Unchecked);
    }
    this->battleyeCheckBox->blockSignals(false);
}

void MainWindow::tabWidgetCurrentChangedHandler(int index)
{
    if (index == 0) {
        modsTab->refreshTab();
        return;
    }

    if (index == 1) {
        optionsTab->tabChanged();
        return;
    }
}

void MainWindow::applicationAboutToQuitHandler()
{
    this->settings->save();
}

void MainWindow::battleyeCheckBoxStateChangedHandler(Qt::CheckState state)
{
    this->settings->save();
}
