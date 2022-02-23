#include "include/dirwindow.hpp"
#include "include/datastructs/dir_utils.hpp"
#include "gui/ui_dirwindow.h"

#include <vector>
#include <iostream>
#include <QMessageBox>
#include <unordered_map>

DirWindow::DirWindow(QWidget *parent) : QDialog(parent),
                                        ui(new Ui::DirWindow),
                                        pid(0)
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
    int dir_read = ps.Reading_DirProcess(umap);

    if (dir_read == OPEN_SUCCESS)
    {
        for (auto &x : umap)
        {
            ui->pidTable->insertRow(ui->pidTable->rowCount());
            int rowCount = ui->pidTable->rowCount() - 1;

            ui->pidTable->setItem(rowCount, Pid, new QTableWidgetItem(QString(QString::fromStdString(x.second))));
            ui->pidTable->setItem(rowCount, Name, new QTableWidgetItem(QString(QString::fromStdString(x.first))));
        }
    }
}

/**
 * @brief
 *
 * @param index
 */
void DirWindow::on_pidTable_doubleClicked(const QModelIndex &index)
{
    setPid(index.model()->data(index).toString());
}

/**
 * @brief get pid clicked in pid table
 *
 * @return pid_t pid
 */
pid_t DirWindow::getPid()
{
    return pid;
}

void DirWindow::setPid(QString __pid)
{
    try
    {
        pid = std::stoi(umap[__pid.toStdString()]);
    }
    catch (std::exception &e)
    {
        pid = std::stoi(__pid.toStdString());
    }

    close();
}