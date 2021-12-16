#include "include/dirprocess.hpp"
#include "ui_dirprocess.h"


DirProcess::DirProcess(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DirProcess)
{
    ui->setupUi(this);

    config_Qfile();
    config_TreeDir();

}

DirProcess::~DirProcess()
{
    delete ui;
    delete dirmodel;
}

void DirProcess::config_Qfile()
{
    QString path = "/proc"; 
    dirmodel = new QFileSystemModel(this); 
    dirmodel->setRootPath(path);
}

void DirProcess::config_TreeDir()
{
    ui->tree_Dir->setModel(dirmodel);
}