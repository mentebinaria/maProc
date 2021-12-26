#include "include/dirwindow.hpp"
#include "include/structs/dir_utils.hpp"
#include "ui_dirwindow.h"

#include <vector>
#include <iostream>
#include <QMessageBox>

DirWindow::DirWindow(QWidget *parent) : QDialog(parent),
                                        ui(new Ui::DirWindow)
{
    ui->setupUi(this);
    Conf_pidTable();
    Set_pidTable();
}

DirWindow::~DirWindow()
{
    delete ui;
}

/**
 * @brief will configure the pid table
 */
void DirWindow::Conf_pidTable(void)
{
    QStringList column;
    column << "Pid"
           << "Name";

    ui->pidTable->setColumnCount(2);
    ui->pidTable->setHorizontalHeaderLabels(column);
    ui->pidTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->pidTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

/**
 * @brief will add all pid to table
 */
void DirWindow::Set_pidTable(void)
{
    std::vector<std::string> NameProcess;
    std::vector<std::string> PidProcess;

    ps.Reading_DirProcess(NameProcess, PidProcess);

    for (auto it : NameProcess)
    {
        int rowCount = ui->pidTable->rowCount();
        ui->pidTable->insertRow(rowCount);

        ui->pidTable->setItem(rowCount - 1, Pid, new QTableWidgetItem(QString(QString::fromStdString(PidProcess.back()))));
        ui->pidTable->setItem(rowCount - 1, Name, new QTableWidgetItem(QString(QString::fromStdString(NameProcess.back()))));

        PidProcess.pop_back();
        NameProcess.pop_back();
    }
}

/**
 * @brief
 *
 * @param index 
 */
void DirWindow::on_pidTable_doubleClicked(const QModelIndex &index)
{
    pid = std::stoi(index.model()->data(index).toString().toStdString());
    if (pid != 0)
        this->close();
}
/**
 * @brief get pid clicked in pid table
 *
 * @return pid_t pid
 */
unsigned int DirWindow::getPid()
{
    return pid;
}