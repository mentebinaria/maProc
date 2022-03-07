#include "include/dirwindow.hpp"
#include "include/datastructs/dir_utils.hpp"
#include "gui/ui_dirwindow.h"

#include <vector>
#include <iostream>
#include <QMessageBox>
#include <unordered_map>

DirWindow::DirWindow(QWidget *parent) : QDialog(parent),
                                        m_ui(new Ui::DirWindow),
                                        m_pid(0)
{
    m_ui->setupUi(this);
    Conf_pidTable();
    Set_pidTable();
}

DirWindow::~DirWindow()
{
    delete m_ui;
}

/**
 * @brief will configure the pid table
 */
void DirWindow::Conf_pidTable(void)
{
    m_ui->pidTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ui->pidTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->pidTable->verticalHeader()->setVisible(false);
    m_ui->search->setPlaceholderText("Find");
}

/**
 * @brief will add all pid to table
 */
void DirWindow::Set_pidTable(void)
{
    int dir_read = m_ps.Reading_DirProcess(m_umap);

    if (dir_read == OPEN_SUCCESS)
    {

        for (auto &x : m_umap)
        {
            m_ui->pidTable->insertRow(m_ui->pidTable->rowCount());
            int rowCount = m_ui->pidTable->rowCount() - 1;

            m_ui->pidTable->setItem(rowCount, Pid, new QTableWidgetItem(QString(QString::fromStdString(x.second))));
            m_ui->pidTable->setItem(rowCount, Name, new QTableWidgetItem(QString(QString::fromStdString(x.first))));
        }

        m_ui->foundLabel->setText("Found " + QString::number(m_ui->pidTable->rowCount()));
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
    return m_pid;
}

/**
 * @brief set pid in table
 *
 * @param __pid
 */
void DirWindow::setPid(QString __pid)
{
    try
    {
        m_pid = std::stoi(m_umap[__pid.toStdString()]);
    }
    catch (std::exception &e)
    {
        m_pid = std::stoi(__pid.toStdString());
    }

    close();
}

void DirWindow::on_search_textEdited(const QString &arg1)
{
    for (int i = 0; i < m_ui->pidTable->rowCount(); i++)
        m_ui->pidTable->hideRow(i);

    QList<QTableWidgetItem *> search = m_ui->pidTable->findItems(arg1, Qt::MatchContains);
    foreach (auto &Ptr, search)
    {
        m_ui->pidTable->showRow(Ptr->row());
        m_ui->foundLabel->setText("Found " + QString::number(search.size()));
    }

    if (arg1.size() == 0)
        m_ui->foundLabel->setText("Found " + QString::number(m_ui->pidTable->rowCount()));

    search.clear();
}
