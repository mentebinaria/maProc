#include "mainwindow.hpp"
#include "dirwindow.hpp"
#include "include/data/win/structs.hpp"
#include "gui/ui_mainwindow.h"
#include "gui/ui_about.h"
#include "include/elf/relf.hpp"

#include <ctime>
#include <QFileDialog>
#include <sstream>
#include <QInputDialog>
#include <string>
#include <QMessageBox>
#include <QDesktopServices>
#include <QFile>
#include <QTextStream>

// =======================================================================
// Macros utilizade for mapper determinate memory
//
// =======================================================================
#define mapper_heap m_mapper.map_mem("[heap]")
#define mapper_stack m_mapper.map_mem("[stack]")

// =======================================================================
// Utils macros for dell row
// column, two macro for write log
//
// =======================================================================
#define column_delete(p_column)      \
  {                                  \
    m_ui->progressFind->setValue(0); \
    while (p_column->rowCount() > 0) \
      p_column->removeRow(0);        \
  }

#define write_log(p_string)                                                    \
  if (m_ui->checkLog->isChecked())                                             \
  {                                                                            \
    time_t now = time(0);                                                      \
    char *date_time = ctime(&now);                                             \
    m_ui->log_text->appendPlainText("DATE: " + QString(date_time) + p_string); \
  }

// =======================================================================
// Constructor and Destructor
//
// =======================================================================
MainWindow::MainWindow(QWidget *p_parent) : QMainWindow(p_parent),
                                            m_ui(new Ui::MainWindow),
                                            m_hex(new QHexView),
                                            m_layout(new QVBoxLayout),
                                            m_dialog(),
                                            m_countRow(0),
                                            m_pid(0)
{
  m_ui->setupUi(this);

  setWindowTitle(TITLE_WINDOW);
  setWindowIcon(QIcon(ICON_WINDOW));
  setMinimumWidth(850);
  setMinimumHeight(680);

  // buttons conf
  conf_button_all();

  // tables conf
  column_config_all();

  // get type initial
  m_type = m_ui->type->currentIndex();

  // set values for edit address and read
  set_types_edit_read();

  // default theme
  on_ButtonLight_triggered();
}

MainWindow::~MainWindow()
{
  delete m_ui;
  delete m_layout;
  delete m_hex;
}

// =======================================================================
// Config all qt, buttons and columns
//
// =======================================================================
void MainWindow::conf_button_all()
{
  // buttons for pass pid
  m_ui->pidButton->setIcon(QIcon(ICON_PASS_PID));
  m_ui->pidButton2->setIcon(QIcon(ICON_PASS_PID));
  m_ui->rpidButton->setIcon(QIcon(ICON_RPASS_PID));
  m_ui->pidButton->setShortcut(QKeySequence("Ctrl+O"));
  m_ui->rpidButton->setShortcut(QKeySequence("Ctrl+R"));

  // buttons for clean
  m_ui->cleanButton->setIcon(QIcon(ICON_CLEAN));
  m_ui->cleanButtonLog->setIcon(QIcon(ICON_CLEAN));
  m_ui->cleanButton->setShortcut(QKeySequence("Ctrl+L"));

  // search button
  m_ui->searchButton->setIcon(QIcon(ICON_SEARCH));
  m_ui->searchButton->setAutoDefault(true);
  m_ui->find->setPlaceholderText("Value");

  // edit button
  m_ui->editButton->setIcon(QIcon(ICON_EDIT));
  m_ui->value->setPlaceholderText("Value");

  // about button
  m_ui->aboutButton->setIcon(QIcon(ICON_ABOUT));

  // new button
  m_ui->newButton->setIcon(QIcon(ICON_NEW));
  m_ui->newButton->setShortcut(QKeySequence("Ctrl+N"));

  // kill button
  m_ui->killButton->setIcon(QIcon(ICON_KILL));
  m_ui->killButton->setShortcut(QKeySequence("Ctrl+K"));

  // stop / play button
  m_ui->stopButton->setIcon(QIcon(ICON_STOP));
  m_ui->stopButton->setShortcut(QKeySequence("Ctrl+S"));

  // search address
  m_ui->search_address->setPlaceholderText("Address");
  // search value maps
  m_ui->search_maps->setPlaceholderText("Value");

  // hex button
  m_ui->gotooffsetButton->setIcon(QIcon(ICON_HEX));
  m_ui->gotooffsetButton->setShortcut(QKeySequence("Ctrl+G"));

  // close
  m_ui->closeButton->setIcon(QIcon(ICON_CLOSE));
  m_ui->closeButton->setShortcut(QKeySequence("Ctrl+Q"));

  // help
  m_ui->quickHelpButton->setIcon(QIcon(ICON_HELP));
  m_ui->quickHelpButton->setShortcut(QKeySequence("Ctrl+H"));

  // full screen
  m_ui->FullScreenButton->setIcon(QIcon(ICON_SCREEN));
  m_ui->FullScreenButton->setShortcut(QKeySequence("F11"));

  // save log
  m_ui->SaveLogButton->setIcon(QIcon(ICON_SAVE));
}

/**
 * @brief clean all columns and delete rows
 *
 */
void MainWindow::column_clean_all()
{
  setWindowTitle(TITLE_WINDOW);
  column_delete(m_ui->view_address);
  column_delete(m_ui->maps_table);
  m_ui->address_edit->setText(NULL_STR);
  m_hex->clear();
  m_ui->foundAddr_label->setText("Found : 0");
  m_ui->Process_label->setText("No Process Selected");
  m_pid_name.clear();
  m_pid_wchan.clear();
  m_pid_exedir.clear();
  m_pid_loginuid.clear();
  m_pid_cmdline.clear();
  m_ui->Process_List->clear();
  m_ui->progressFind->reset();
  m_ui->fileSize_label->setText("Size 0 Bytes");

  for (int i = 10; i >= 0; i--)
  {
    m_ui->infos_addr->setItem(i, Address_on, new QTableWidgetItem(NULL_STR));
    m_ui->infos_addr->setItem(i, Address_off, new QTableWidgetItem(NULL_STR));
    m_ui->infos_addr->setItem(i, Size_map, new QTableWidgetItem("0"));
    m_ui->infos_addr->setItem(i, Flags, new QTableWidgetItem("----"));
  }

  for (int i = 0; i <= 16; i++)
    m_ui->elfheader_table->setItem(0, i, new QTableWidgetItem(CLEAN_ROW));
}

void MainWindow::column_config_all()
{
  // view address
  // m_ui->view_address->horizontalHeader()->setSectionResizeMode ( QHeaderView::Stretch );
  m_ui->view_address->setShowGrid(false);
  m_ui->view_address->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_ui->view_address->verticalHeader()->setVisible(false);

  // infos addr heap and stack
  m_ui->infos_addr->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  m_ui->infos_addr->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_ui->infos_addr->setShowGrid(false);

  // log infos
  m_ui->log_text->setReadOnly(true);

  // maps table
  // m_ui->maps_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  m_ui->maps_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_ui->maps_table->verticalHeader()->setVisible(false);

  // hex dump
  m_layout->addWidget(m_hex);
  m_ui->HexViewTab->setLayout(m_layout);

  // config infos type OS
  m_sys_hostname = QString::fromStdString(m_mapper.get_utilsPid(HOSTNAME));
  m_sys_osrealese = QString::fromStdString(m_mapper.get_utilsPid(OSREALESE));
  m_sys_version = QString::fromStdString(m_mapper.get_utilsPid(VERSION));
  m_sys_type = QString::fromStdString(m_mapper.get_utilsPid(TYPE));

  // elf header
  m_ui->elfheader_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  m_ui->elfheader_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

  m_ui->Process_List->setReadOnly(true);
}

// =======================================================================
// Set Values in Columns
//
// =======================================================================
void MainWindow::set_values_column_heap()
{
  m_ui->view_address->setShowGrid(false);
  m_ui->view_address->setEditTriggers(QAbstractItemView::NoEditTriggers);

  // infos_addr
  QString on = QString::number(m_mapper.get_addrOn(), 16);
  QString off = QString::number(m_mapper.get_addrOff(), 16);
  QString size = QString::number(m_mapper.get_sizeAddress());
  QString flags = QString::fromStdString(m_mapper.get_Flags());

  // set itens
  m_ui->infos_addr->setItem(0, Address_on, new QTableWidgetItem(on));
  m_ui->infos_addr->setItem(0, Address_off, new QTableWidgetItem(off));
  m_ui->infos_addr->setItem(0, Size_map, new QTableWidgetItem(size));
  m_ui->infos_addr->setItem(0, Flags, new QTableWidgetItem(flags));
}

void MainWindow::set_values_column_stack()
{
  // infos_addr
  QString on = QString::number(m_mapper.get_addrOn(), 16);
  QString off = QString::number(m_mapper.get_addrOff(), 16);
  QString size = QString::number(m_mapper.get_sizeAddress());
  QString flags = QString::fromStdString(m_mapper.get_Flags());

  // set itens
  m_ui->infos_addr->setItem(1, Address_on, new QTableWidgetItem(on));
  m_ui->infos_addr->setItem(1, Address_off, new QTableWidgetItem(off));
  m_ui->infos_addr->setItem(1, Size_map, new QTableWidgetItem(size));
  m_ui->infos_addr->setItem(1, Flags, new QTableWidgetItem(flags));
}

void MainWindow::set_values_column_elf()
{
  PElf(m_pid_exedir.toStdString().data());
  ElfStruct(pelf);

  int classElf = pelf.classElf;

  m_ui->elfheader_table->setItem(0, e_magic, new QTableWidgetItem(QString("7f 45 4c 46")));
  m_ui->elfheader_table->setItem(0, e_version, new QTableWidgetItem(QString::number((classElf == ELFCLASS64) ? pelf.elf64.Header->e_version : pelf.elf32.Header->e_version)));
  m_ui->elfheader_table->setItem(0, e_entry, new QTableWidgetItem(QString::number((classElf == ELFCLASS64) ? pelf.elf64.Header->e_entry : pelf.elf32.Header->e_entry, 16)));
  m_ui->elfheader_table->setItem(0, e_phoff, new QTableWidgetItem(QString::number((classElf == ELFCLASS64) ? pelf.elf64.Header->e_phoff : pelf.elf32.Header->e_phoff)));
  m_ui->elfheader_table->setItem(0, e_shoff, new QTableWidgetItem(QString::number((classElf == ELFCLASS64) ? pelf.elf64.Header->e_shoff : pelf.elf32.Header->e_shoff)));
  m_ui->elfheader_table->setItem(0, e_abiversion, new QTableWidgetItem(QString::number(ElfMagic()[EI_ABIVERSION])));
  m_ui->elfheader_table->setItem(0, e_flags, new QTableWidgetItem(QString::number((classElf == ELFCLASS64) ? pelf.elf64.Header->e_flags : pelf.elf32.Header->e_flags)));
  m_ui->elfheader_table->setItem(0, e_shentsize, new QTableWidgetItem(QString::number((classElf == ELFCLASS64) ? pelf.elf64.Header->e_shentsize : pelf.elf32.Header->e_shentsize)));
  m_ui->elfheader_table->setItem(0, e_shnum, new QTableWidgetItem(QString::number((classElf == ELFCLASS64) ? pelf.elf64.Header->e_shnum : pelf.elf32.Header->e_shnum)));
  m_ui->elfheader_table->setItem(0, e_shstrndx, new QTableWidgetItem(QString::number((classElf == ELFCLASS64) ? pelf.elf64.Header->e_shstrndx : pelf.elf32.Header->e_shstrndx)));
  m_ui->elfheader_table->setItem(0, e_class, new QTableWidgetItem(QString((classElf == ELFCLASS64) ? "ELF64" : "ELF32")));
  m_ui->elfheader_table->setItem(0, e_abi, new QTableWidgetItem(QString((ElfMagic()[EI_OSABI] == 0) ? "System V" : "Not identified")));
  m_ui->elfheader_table->setItem(0, e_type, new QTableWidgetItem(QString((ElfMagic()[0x10] == ET_EXEC) ? "Executable File" : (ElfMagic()[0x10] == ET_REL) ? "Realocable File"
                                                                                                                                                          : "Shared Object")));
  m_ui->elfheader_table->setItem(0, e_machine, new QTableWidgetItem(QString((ElfMagic()[0x12] == EM_X86_64) ? "x86-64 Architecture" : (ElfMagic()[0x12] == EM_386) ? "x84_32 Architecture"
                                                                                                                                                                   : "Not identified")));
}

/**
 * @brief set types possibles for search mem
 *
 */
void MainWindow::set_types_edit_read()
{
  m_typeSizes.insert(std::make_pair<std::string, size_t>("char", sizeof(char)));
  m_typeSizes.insert(std::make_pair<std::string, size_t>("int", sizeof(int)));
  m_typeSizes.insert(std::make_pair<std::string, size_t>("int16", sizeof(int16_t)));
  m_typeSizes.insert(std::make_pair<std::string, size_t>("uint64", sizeof(uint64_t)));
  m_typeSizes.insert(std::make_pair<std::string, size_t>("string", sizeof(std::string)));
}

// Big O(n²)
void MainWindow::set_values_column_maps()
{
  for (auto &x : m_unmap)
  {
    m_ui->maps_table->insertRow(m_ui->maps_table->rowCount());
    int rowCount = m_ui->maps_table->rowCount() - 1;

    m_ui->maps_table->setItem(rowCount, Address_start, new QTableWidgetItem(QString::number(x.second.addr_on, 16)));
    m_ui->maps_table->setItem(rowCount, Address_stop, new QTableWidgetItem(QString::number(x.second.addr_off, 16)));
    m_ui->maps_table->setItem(rowCount, Perms, new QTableWidgetItem(QString::fromStdString(x.second.flags)));
    m_ui->maps_table->setItem(rowCount, Offset, new QTableWidgetItem(QString::number(x.second.offset)));
    m_ui->maps_table->setItem(rowCount, Pathname, new QTableWidgetItem(QString::fromStdString(x.second.pathname)));
  }
}

void MainWindow::set_values_process()
{
  m_ui->Process_label->setText("Process");
  m_ui->Process_List->setText("\t\tName : " + m_pid_name.trimmed() + "\t\tPid: " + QString::fromStdString(std::to_string(m_pid)));
}

/**
 * Big O(n²)
 *
 * @brief will set all addresses and values ​​found in the search
 *
 * @param p_offsets vector containing the address
 * @param value value find
 * @param memory memory in which the value was found
 */
void MainWindow::set_values_column_address(std::vector<off_t> &p_offsets, std::string p_value, std::string p_memory)
{
  int progress = 0;
  int valueMax = p_offsets.size();
  m_ui->progressFind->reset();
  m_ui->progressFind->setMaximum(valueMax);

  if (m_ui->CheckHex->isChecked())
    p_value = "0x" + p_value;

  if (m_all_mapper)
    m_ui->view_address->setRowCount(m_countRow);
  else if (m_ui->view_address->rowCount() > 0)
    column_delete(m_ui->view_address);

  for (auto &x : p_offsets)
  {
    m_ui->view_address->insertRow(m_ui->view_address->rowCount());
    int rowCount = m_ui->view_address->rowCount() - 1;

    QString addr = QString::number(x, 16);

    m_ui->view_address->setItem(rowCount, Address, new QTableWidgetItem(addr));
    m_ui->view_address->setItem(rowCount, Value, new QTableWidgetItem(QString(QString::fromStdString(p_value))));
    m_ui->view_address->setItem(rowCount, Memory, new QTableWidgetItem(QString(QString::fromStdString(p_memory))));

    m_ui->progressFind->setValue(progress);
    progress++;
  }

  if (m_all_mapper)
    m_countRow = m_ui->view_address->rowCount();

  m_ui->foundAddr_label->setText("Found : " + QString::number(m_ui->view_address->rowCount()));

  if (m_ui->progressFind->value() < valueMax)
    m_ui->progressFind->setValue(valueMax);
}

// =======================================================================
// Buttons clicked and actions
//
// =======================================================================

// style sheets
void MainWindow::on_ButtonDark_triggered()
{
  QFile f("../src/gui/style/dark.css");

  if (!f.exists())
  {
    std::string errorMessage("Loading Error: ");
    errorMessage.append("Error in load style dark, verify this file exist...");

    QMessageBox msgBox;
    msgBox.setText(errorMessage.c_str());
    msgBox.exec();
    return;
  }
  else
  {
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    setStyleSheet(ts.readAll());

    m_hex->setColorCharacters(Qt::white);
    m_hex->setColorAddress(QColor(30, 30, 30, 0xff));
  }
}

void MainWindow::on_ButtonLight_triggered()
{
  QFile f("../src/gui/style/light.css");

  if (!f.exists())
  {
    std::string errorMessage("Loading Error: ");
    errorMessage.append("Error in load style light, verify this file exist...");

    QMessageBox msgBox;
    msgBox.setText(errorMessage.c_str());
    msgBox.exec();
    return;
  }
  else
  {
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    setStyleSheet(ts.readAll());

    m_hex->setColorAddress(Qt::white);
    m_hex->setColorCharacters(Qt::black);
  }
}

void MainWindow::on_PidInfoButton_triggered()
{
  if (m_pid_name.size() == 0)
    return;

  QString infos = "<h5>Name : </h5>" + m_pid_name +
                  "\n<h5>Command Line : </h5>" + m_pid_cmdline +
                  "\n<h5>Login uid : </h5>" + m_pid_loginuid +
                  "\n<h5>Binary Size : </h5>" + m_pid_sizebin +
                  "\n<h5>Wchan : </h5>" + m_pid_wchan;
  QMessageBox::about(nullptr, "Pid Info", infos);
}

/**
 * @brief clean log button
 *
 */
void MainWindow::on_cleanButtonLog_triggered()
{
  m_ui->log_text->clear();
}

/**
 * @brief stop process button
 *
 */
void MainWindow::on_stopButton_triggered()
{
  if (m_pid_name.size() == 0)
    return;

  try
  {
    if (m_ui->stopButton->text() == "Stop Pid")
    {
      m_mapper.map_stop();
      m_ui->stopButton->setText("Play Pid");
      m_ui->stopButton->setIcon(QIcon(ICON_PLAY));
      write_log("[SIGNAL STOP] you send a signal to the PID=" + QString::fromStdString(std::to_string(m_pid)) + ", sending it on a stop in the process")
    }
    else if (m_ui->stopButton->text() == "Play Pid")
    {
      m_mapper.map_stop(false);
      m_ui->stopButton->setText("Stop Pid");
      m_ui->stopButton->setIcon(QIcon(ICON_STOP));
      write_log("[SIGNAL RUNNING] you send a signal to the PID=" + QString::fromStdString(std::to_string(m_pid)) + ", sending it on a running in the process")
    }
  }
  catch (std::exception &error)
  {
    QMessageBox::critical(nullptr, "Error", error.what());
    column_clean_all();
  }
}

/**
 * @brief kill process button
 *
 */
void MainWindow::on_killButton_triggered()
{
  if (m_pid_name.size() == 0)
    return;

  QMessageBox::StandardButton kill = QMessageBox::question(this, "SIGNAL KILL", "Kill Process " + m_pid_name + "? ", QMessageBox::Yes | QMessageBox::No);

  if (kill == QMessageBox::Yes)
  {
    m_pid = 0;
    m_mapper.map_kill();
    column_clean_all();
    write_log("[SIGNAL KILL] you send a signal to the PID=" + QString::fromStdString(std::to_string(m_pid)) + ", sending it on a kill in the process")
  }
}

void MainWindow::on_search_maps_textEdited(const QString &p_arg1)
{
  for (int i = 0; i < m_ui->maps_table->rowCount(); i++)
    m_ui->maps_table->hideRow(i);

  QList<QTableWidgetItem *> search = m_ui->maps_table->findItems(p_arg1, Qt::MatchContains);

  foreach (auto &Ptr, search)
  {
    m_ui->maps_table->showRow(Ptr->row());
    m_ui->foundAddr_label->setText("Found " + QString::number(search.size()));
  }

  if (p_arg1.size() == 0)
    m_ui->foundAddr_label->setText("Found " + QString::number(m_ui->maps_table->rowCount()));

  search.clear();
}

/**
 * @brief search table view address
 *
 * @param arg1 search address table view_address
 */
void MainWindow::on_search_address_textEdited(const QString &p_arg1)
{
  for (int i = 0; i < m_ui->view_address->rowCount(); i++)
    m_ui->view_address->hideRow(i);

  QList<QTableWidgetItem *> search = m_ui->view_address->findItems(p_arg1, Qt::MatchContains);

  foreach (auto &Ptr, search)
    m_ui->view_address->showRow(Ptr->row());

  search.clear();
}

/**
 * @brief set offset for jumṕ
 * */
void MainWindow::on_gotooffsetButton_triggered()
{
  if (m_pid_name.size() == 0)
    return;

  bool done;
  QString offset = QInputDialog::getText(this, tr("Goto..."),
                                         tr("Offset (0x for hexadecimal):"), QLineEdit::Normal,
                                         nullptr, &done);

  if (done && offset[0] == '0' && offset[1] == 'x')
    m_hex->showFromOffset(offset.toInt(nullptr, 16));
  else
    m_hex->showFromOffset(offset.toInt(nullptr));
}

/**
 * @brief open browser for help maProc
 * */
void MainWindow::on_quickHelpButton_triggered()
{
  try
  {
    QDesktopServices::openUrl(QUrl("https://github.com/mentebinaria/maProc/wiki/Help"));
  }
  catch (std::exception &e)
  {
    QMessageBox::warning(nullptr, "Warning", e.what());
  }
}

/**
 * @brief full screen window
 * */
void MainWindow::on_FullScreenButton_triggered()
{
  if (isFullScreen())
    showNormal();
  else
    showFullScreen();
}

/**
 * @brief Save log
 * */
void MainWindow::on_SaveLogButton_triggered()
{
  QString textLog = m_ui->log_text->toPlainText();

  if (textLog.size() != 0)
  {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Log maProc"), "",
                                                    tr("maProc Logs (*.log);;All Files (*)"));

    if (fileName.isEmpty())
      return;
    else
    {
      QFile file(fileName + ".log");

      if (!file.open(QIODevice::WriteOnly))
      {
        QMessageBox::information(this, tr("Unable to open file"),
                                 file.errorString());
        return;
      }

      file.write(textLog.toStdString().data(), textLog.size());
    }
  }
  else
    QMessageBox::critical(nullptr, "Error", "Logging is empty, active checkbox []Log");
}

void MainWindow::on_SystemInfoButton_triggered()
{

  QString infos = "<h5>Host Name :</h5>" + m_sys_hostname +
                  "\n<h5>Os realese :</h5>" + m_sys_osrealese +
                  "\n<h5>Os Version :</h5>" + m_sys_version +
                  "\n<h5>Os type :</h5>" + m_sys_type;
  QMessageBox::about(nullptr, "System Info", infos);
}

void MainWindow::on_pidButton_triggered()
{
  column_clean_all();
  DirWindow dir;
  dir.exec();

  m_pid = dir.getPid();
  if (m_pid == 0)
    return;
  else
    mainMapper();
}

void MainWindow::on_pidButton2_clicked()
{
  on_pidButton_triggered();
}

/**
 * @brief search value in  fetch value from an address
 * on the stack or heap, will get the type using the combo box
 */
void MainWindow::on_searchButton_clicked()
{
  m_all_mapper = false;
  std::string varType = m_ui->type->currentText().toStdString();
  std::string find = m_ui->find->text().toStdString();
  std::size_t mem = m_ui->mem->currentIndex();
  auto it = m_typeSizes.find(varType);

  try
  {
    if (m_ui->CheckHex->isChecked() && it->second != sizeof(std::string))
      find = std::to_string(stoul(find, nullptr, 16));

    if (find.size() != 0 && m_pid != 0 && it != m_typeSizes.end())
    {
      QTableWidgetItem *addr;
      off_t address_start;
      QTableWidgetItem *size;
      uint64_t lenght;
      std::vector<off_t> offsets;

      switch (mem)
      {
      case 0: // stack
      stack:
        addr = m_ui->infos_addr->item(1, 0);
        size = m_ui->infos_addr->item(1, 2);

        if (addr->text().toStdString() != NULL_STR)
        {
          address_start = static_cast<off_t>(std::stoul(addr->text().toStdString(), nullptr, 16));
          lenght = std::stoul(size->text().toStdString(), nullptr);

          mapper_find(address_start, lenght, find, it->second, offsets);

          if (offsets.size() != 0)
            set_values_column_address(offsets, find, "[stack]");
        }

        if (mem == 3)
          goto heap;

        break;

      case 1: // heap
      heap:
        addr = m_ui->infos_addr->item(0, 0);
        size = m_ui->infos_addr->item(0, 2);

        if (addr->text().toStdString() != NULL_STR)
        {
          address_start = static_cast<off_t>(std::stoul(addr->text().toStdString(), nullptr, 16));
          lenght = std::stoul(size->text().toStdString(), nullptr);

          mapper_find(address_start, lenght, find, it->second, offsets);

          if (offsets.size() != 0)
            set_values_column_address(offsets, find, "[heap]");
        }

        if (mem == 3)
          goto personalization;

        break;

      case 2: // personalization
      {
      personalization:

        if (m_ui->StartAddress->text().size() != 0 && m_ui->StopAddress->text().size() != 0)
        {
          address_start = static_cast<off_t>(std::stoul(m_ui->StartAddress->text().toStdString(), nullptr, 16));
          off_t address_stop = static_cast<off_t>(std::stoul(m_ui->StopAddress->text().toStdString(), nullptr, 16));

          if (address_start < address_stop)
          {
            lenght = address_stop - address_start;
            mapper_find(address_start, lenght, find, it->second, offsets);
          }
          else
            QMessageBox::critical(nullptr, "Error", "Offsets invalids, consult tab maps");

          if (offsets.size() != 0)
            set_values_column_address(offsets, find, "[personalization]");
        }

        break;
      }

      case 3: // all
        m_all_mapper = true;
        goto stack;

      default:
        return;
      }
    }
    else if (it == m_typeSizes.end()) // critical error, will exit the program
    {
      QMessageBox::critical(nullptr, "Critical Error", "Error in code not possible find type\n verify unordered_map[TypeSizes]");
      exit(EXIT_FAILURE);
    }
  }
  catch (std::exception &error)
  {
    QMessageBox::warning(nullptr, "Warning", "Verify offsets error unexpected, consult maps tab");
  }
}

void MainWindow::on_closeButton_triggered()
{
  QMessageBox::StandardButton close = QMessageBox::question(this, "Close", "Close this Program ?", QMessageBox::Yes | QMessageBox::No);

  if (close == QMessageBox::Yes)
    this->close();
}

void MainWindow::on_cleanButton_triggered()
{
  column_clean_all();
}

/**
 * @brief new window
 *
 */
void MainWindow::on_newButton_triggered()
{
  auto mm = new MainWindow();
  mm->setAttribute(Qt::WA_DeleteOnClose);
  mm->show();
}

/**
 * @brief edit address value
 *
 */
void MainWindow::on_editButton_clicked()
{
  off_t address = valid_address_edit();
  std::string varType = m_ui->type->currentText().toStdString();
  auto it = m_typeSizes.find(varType);
  std::string value = m_ui->value->text().toStdString();

  if (it != m_typeSizes.end() && address != 0 && m_pid != 0 && value.size() != 0)
  {
    std::size_t SizeT = (varType == "string") ? value.size() : it->second;

    if (m_ui->FillNullCheck->isChecked() && varType == "string")
    {
      if (SizeT < m_countFillNull)
      {
        for (int i = 0; i < m_countFillNull; i++)
          value += '\0';

        SizeT = value.size();
      }
    }

    if (m_mapper.map_write(address, (void *)&value[0], SizeT) == true)
    {
      write_log("[EDITED] Memory edited address [" + QString::number(address, 16) + "]  [" + m_ui->find->text() + "] - > [" + QString::fromStdString(value) + "]");
    }
    else
    {
      write_log("[ERROR] Fail edit memory [" + QString::number(address, 16) + "]  [" + m_ui->find->text() + "] - > [" + QString::fromStdString(value) + "]");
    }
  }
  else if (it == m_typeSizes.end())
  { // critical error, will exit the program

    QMessageBox::critical(nullptr, "Critical Error", "Error in code not possible find type\n verify unordered_map[TypeSizes]");
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief repasser pid if cleaned infos
 *
 */
void MainWindow::on_rpidButton_triggered()
{
  if (m_pid != 0)
    mainMapper();
}

/**
 * @brief about button for infos copyright and project information
 *
 */
void MainWindow::on_aboutButton_triggered()
{
  m_dialog.reset(new QDialog(this));
  Ui_m_about aboutUi;
  aboutUi.setupUi(m_dialog.get());

  m_dialog->showNormal();
}

// =======================================================================
// Utils, clean columns and view columns,
// verify pid and main mapper process and validatios
//
// =======================================================================

void MainWindow::mainMapper()
{
  column_delete(m_ui->view_address);
  column_clean_all();

  try
  {
    verify_pid();

    if (m_pid != 0)
    {
      m_unmap = m_mapper.get_Maps();

      set_values_column_elf();
      set_values_process();
      set_values_column_maps();
      m_hex->loadFile(m_pid_exedir);
      m_ui->fileSize_label->setText("Size " + QString::number(m_hex->getFileSize()) + "Bytes");

      if (mapper_stack)
        set_values_column_stack();

      if (mapper_heap)
        set_values_column_heap();
    }
  }
  catch (std::exception &error)
  {
    QMessageBox::warning(nullptr, "Warning", (std::string(error.what()) == "stoul") ? "Verify pid error unexpected" : error.what());
  }
}

/**
 * @brief verify pid for mapper
 *
 */
void MainWindow::verify_pid()
{
  if (m_mapper.map_pid(m_pid) == PID_SUCCESS)
  {
    m_pid_name = QString::fromStdString(m_mapper.get_utilsPid(NAME));
    m_pid_cmdline = QString::fromStdString(m_mapper.get_utilsPid(CMDLINE));
    m_pid_loginuid = QString::fromStdString(m_mapper.get_utilsPid(LOGINUID));
    m_pid_sizebin = QString::fromStdString(m_mapper.get_utilsPid(SIZEBIN));
    m_pid_wchan = QString::fromStdString(m_mapper.get_utilsPid(WCHAN));
    m_pid_exedir = QString::fromStdString(m_mapper.get_utilsPid(EXEDIR));
    m_pid_blocksize = QString::fromStdString(m_mapper.get_utilsPid(BLOCKSIZEBIN));

    // tell the status bar which pid is being mapped
    setWindowTitle("maProc - PID: " + QString::fromStdString(std::to_string(m_pid)) + " Name: " + m_pid_name);
  }
  else
    m_pid = 0;
}

/**
 * @brief will try to fetch the information in memory
 *
 * @param p_addr start address for search
 * @param p_length size for read
 * @param p_find what to look for
 * @param p_type type for search
 * @param p_offsets will store the addresses found in a vector
 */
void MainWindow::mapper_find(off_t p_addr, off_t p_length, std::string p_find,
                             uint8_t p_type, std::vector<off_t> &p_offsets)
{
  try
  {
    int valueMax = p_offsets.size();
    m_countFillNull = p_find.size();

    // search for locations that have
    // already been found in the table
    if (m_ui->checkCache->isChecked())
    {

      // progress bar
      m_ui->progressFind->setValue(0);

      std::string find = m_ui->find->text().toStdString();
      int rowCont = m_ui->view_address->rowCount();
      std::size_t size = find.size();

      Data data(size);

      for (int it = 0; it < rowCont; it++)
      {
        std::string str;
        off_t addr = std::stoul(m_ui->view_address->item(it, Address)->text().toStdString(), nullptr, 16);
        m_mapper.map_read(addr, size, data);

        for (int i = 0; i < data.m_size; i++)
          str += data.m_buff[i];

        if (str == find)
        {
          m_ui->view_address->showRow(it);
          m_ui->view_address->setItem(it, Value, new QTableWidgetItem(QString(QString::fromStdString(find))));
        }
        else
          m_ui->view_address->hideRow(it);

        m_ui->progressFind->setValue(it);
      }

      if (m_ui->progressFind->value() < valueMax)
        m_ui->progressFind->setValue(valueMax);

      data.clear();
    }
    else
    {
      if (m_mapper.map_find(p_addr, p_length, p_find, p_type, p_offsets) == READ_FAIL)
        QMessageBox::critical(nullptr, "Error", "Not read memory, error in start  " + QString::number(p_addr, 16));
    }

    QString sizeFound = QString::fromStdString(std::to_string(valueMax));
    write_log("[SEARCH] PID [" + QString::fromStdString(std::to_string(m_pid)) + "] searched in memory start [" + QString::number(p_addr, 16) + "] [" + QString::fromStdString(p_find) + "] found these addresses with such values [" + sizeFound + "]");

    m_ui->foundAddr_label->setText("Found : " + sizeFound);
  }
  catch (std::exception &error)
  {
    m_ui->foundAddr_label->setText("Found : 0");
    QMessageBox::critical(nullptr, "Error", error.what());
    write_log("[ERROR] not read memory " + QString::fromStdString(error.what()));
  }
}

/**
 * @brief
 *
 * @return off_t if address valid return p_offsets, else address not valid return 0
 */
off_t MainWindow::valid_address_edit()
{
  std::string address_edit = m_ui->address_edit->text().toStdString();
  off_t address = 0;

  if (address_edit == NULL_STR)
    QMessageBox::critical(nullptr, "Error", "Address NULL not valid, set address valid for edit value");
  else
    address = static_cast<off_t>(std::stoul(address_edit, nullptr, 16));

  return address;
}

void MainWindow::on_maps_table_itemDoubleClicked(QTableWidgetItem *p_first)
{
  if (m_pid_name.size() == 0)
    return;

  QString AddressStart = m_ui->maps_table->item(p_first->row(), Address_start)->text();
  QString AddressStop = m_ui->maps_table->item(p_first->row(), Address_stop)->text();
  QString PathnameMemory = m_ui->maps_table->item(p_first->row(), Pathname)->text();

  m_ui->StartAddress->setText(AddressStart);
  m_ui->StopAddress->setText(AddressStop);

  write_log("[CLICKED] Address Start [" + AddressStart + "] Address Stop [" + AddressStop + "]" + " Memory [" + PathnameMemory + "]");
}

/**
 * @brief set address for edit value
 *
 * @param p_first address double clicked
 */
void MainWindow::view_address_table(QTableWidgetItem *p_first)
{
  if (m_pid_name.size() == 0)
    return;

  QString selectedAddress = m_ui->view_address->item(p_first->row(), Address)->text();
  QString selectedValue = m_ui->view_address->item(p_first->row(), Value)->text();
  QString selectedMemory = m_ui->view_address->item(p_first->row(), Memory)->text();
  m_ui->address_edit->setText(selectedAddress);

  write_log("[CLICKED] Address [" + selectedAddress + "] Value [" + selectedValue + "]" + " Memory [" + selectedMemory + "]");
}

// =======================================================================
//  Thread and tasks window
// =======================================================================

void MainWindow::paintEvent(QPaintEvent *event)
{
  if (m_hex->getFileSize() == 0)
    return;

  auto offset = m_hex->getOffset();
  m_ui->offset_label->setText("0x" + QString::number(offset, 16));

  m_ui->selecteds_label->setText("(Bytes " + QString::number(m_hex->getBytesSelecteds()) + " selected)");
}