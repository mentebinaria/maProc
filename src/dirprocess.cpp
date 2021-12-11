#include "include/dirprocess.hpp"
#include "ui_dirprocess.h"

DirProcess::DirProcess(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DirProcess)
{
    ui->setupUi(this);
}

DirProcess::~DirProcess()
{
    delete ui;
}
