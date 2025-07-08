#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowIconText("shit");
    this->setWindowIcon(QIcon(":/shit.ico"));
}

MainWindow::~MainWindow() {}
