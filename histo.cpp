#include "histo.h"
#include "ui_histo.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

Histo::Histo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Histo)
{
    ui->setupUi(this);
}

Histo::~Histo()
{
    delete ui;
}





