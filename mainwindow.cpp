#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->tabWidget = ui->tabWidget;
    this->settings = new Settings(this);
    this->modsTab = new ModsTab(this, this->settings);
    this->optionsTab = new OptionsTab(this, this->settings);

    connect(this->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabWidgetCurrentChanged);

    this->setWindowTitle(QApplication::applicationName());
    this->tabWidget->setCurrentIndex(0);
    this->modsTab->loadAvailableMods();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::tabWidgetCurrentChanged(int index)
{
    if (index == 1) {
        optionsTab->tabChanged();
        return;
    }
}
