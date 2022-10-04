#include "dirwindow.hpp"
#include "include/data/win/structs.hpp"
#include "gui/ui_dirwindow.h"

#include <QMessageBox>
#include <unordered_map>

DirWindow::DirWindow(QWidget *p_parent) : QDialog(p_parent),
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
  // m_ui->pidTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  m_ui->pidTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_ui->pidTable->verticalHeader()->setVisible(false);
  m_ui->search->setPlaceholderText("Find");
}

/**
 * @brief will add all pid to table
 */
void DirWindow::Set_pidTable()
{
  m_ps.Reading_DirProcess(m_umap);

  for (auto &x : m_umap)
  {
    m_ui->pidTable->insertRow(m_ui->pidTable->rowCount());
    int rowCount = m_ui->pidTable->rowCount() - 1;

    m_ui->pidTable->setItem(rowCount, Pid, new QTableWidgetItem(QString(QString::fromStdString(x.second.pid))));
    m_ui->pidTable->setItem(rowCount, Name, new QTableWidgetItem(QString(QString::fromStdString(x.first))));
    m_ui->pidTable->setItem(rowCount, User, new QTableWidgetItem(QString(QString::fromStdString(x.second.user))));
  }

  m_ui->foundLabel->setText("Found " + QString::number(m_ui->pidTable->rowCount()));
}

/**
 * @brief
 *
 * @param index
 */
void DirWindow::on_pidTable_doubleClicked(const QModelIndex &p_index)
{
  if (p_index.column() == 2)
    return;

  try
  {
    setPid(p_index.model()->data(p_index).toString());
  }
  catch (std::exception &error)
  {
    QMessageBox::critical(nullptr, "Error", "Verify pid " + QString::fromStdString(std::to_string(m_pid)+" error unexpected"));
  }
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
 * @param p_pid
 */
void DirWindow::setPid(QString p_pid)
{
  try
  {
    m_pid = std::stoi(m_umap[p_pid.toStdString()].pid);
  }
  catch (std::exception &error)
  {
    m_pid = std::stoi(p_pid.toStdString());
  }

  close();
}

void DirWindow::on_search_textEdited(const QString &p_arg1)
{
  for (int i = 0; i < m_ui->pidTable->rowCount(); i++)
    m_ui->pidTable->hideRow(i);

  QList<QTableWidgetItem *> search = m_ui->pidTable->findItems(p_arg1, Qt::MatchContains);

  foreach (auto &Ptr, search)
  {
    m_ui->pidTable->showRow(Ptr->row());
    m_ui->foundLabel->setText("Found " + QString::number(search.size()));
  }

  if (p_arg1.size() == 0)
    m_ui->foundLabel->setText("Found " + QString::number(m_ui->pidTable->rowCount()));

  search.clear();
}