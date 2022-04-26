#include "include/mainwindow.hpp"
#include "include/datastructs/win_utils.hpp"
#include "include/dirwindow.hpp"
#include "gui/ui_mainwindow.h"
#include "gui/ui_about.h"

#include <QInputDialog>
#include <iostream>
#include <string>
#include <QMessageBox>
#include <QDesktopServices>
#include <QFile>

#define mapper_heap m_mapper.map_mem("[heap]")
#define mapper_stack m_mapper.map_mem("[stack]")

#define column_delete(p_column)          \
    {                                    \
      m_ui->progressFind->setValue(0);       \
      while (p_column->rowCount() > 0)   \
            p_column->removeRow(0);      \
    }

#define write_log(p_string)            \
    if (m_ui->checkLog->isChecked()) \
        m_ui->log_text->appendPlainText(p_string);

MainWindow::MainWindow ( QWidget *p_parent ) : QMainWindow ( p_parent ),
  m_ui ( new Ui::MainWindow ),
  m_hex ( new QHexView ),
  m_layout ( new QVBoxLayout ),
  m_dialog(),
  m_pid ( 0 )
{
  m_ui->setupUi ( this );

  setWindowTitle ( TITLE_WINDOW );
  setWindowIcon ( QIcon ( ICON_WINDOW ) );
  setMinimumWidth ( 660 );
  setMinimumHeight ( 650 );

  // buttons conf
  conf_button_all();

  // tables conf
  column_config_all();

  // get type initial
  m_type = m_ui->type->currentIndex();

  // set values for edit address and read
  set_types_edit_read();
}

MainWindow::~MainWindow()
{
  delete m_ui;
  delete m_layout;
  delete m_hex;
}

/**
 * @brief conf all buttons
 *
 */
void MainWindow::conf_button_all()
{
  // buttons for pass pid
  m_ui->pidButton->setIcon ( QIcon ( ICON_PASS_PID ) );
  m_ui->rpidButton->setIcon ( QIcon ( ICON_RPASS_PID ) );
  m_ui->pidButton->setShortcut ( QKeySequence ( "Ctrl+O" ) );
  m_ui->rpidButton->setShortcut ( QKeySequence ( "Ctrl+R" ) );

  // buttons for clean
  m_ui->cleanButton->setIcon ( QIcon ( ICON_CLEAN ) );
  m_ui->cleanButtonLog->setIcon ( QIcon ( ICON_CLEAN ) );
  m_ui->cleanButton->setShortcut ( QKeySequence ( "Ctrl+L" ) );

  // search button
  m_ui->searchButton->setIcon ( QIcon ( ICON_SEARCH ) );
  m_ui->find->setPlaceholderText ( "Value" );

  // edit button
  m_ui->editButton->setIcon ( QIcon ( ICON_EDIT ) );
  m_ui->value->setPlaceholderText ( "Value" );

  // about button
  m_ui->aboutButton->setIcon ( QIcon ( ICON_ABOUT ) );

  // new button
  m_ui->newButton->setIcon ( QIcon ( ICON_NEW ) );
  m_ui->newButton->setShortcut ( QKeySequence ( "Ctrl+N" ) );

  // kill button
  m_ui->killButton->setIcon ( QIcon ( ICON_KILL ) );
  m_ui->killButton->setShortcut ( QKeySequence ( "Ctrl+K" ) );

  // stop / play button
  m_ui->stopButton->setIcon ( QIcon ( ICON_STOP ) );
  m_ui->stopButton->setShortcut ( QKeySequence ( "Ctrl+S" ) );

  // search address
  m_ui->search_address->setPlaceholderText ( "Address" );

  // hex button
  m_ui->gotooffsetButton->setIcon ( QIcon ( ICON_HEX ) );
  m_ui->gotooffsetButton->setShortcut ( QKeySequence ( "Ctrl+G" ) );

  // close
  m_ui->closeButton->setIcon ( QIcon ( ICON_CLOSE ) );
  m_ui->closeButton->setShortcut ( QKeySequence ( "Ctrl+Q" ) );

  // help
  m_ui->quickHelpButton->setIcon ( QIcon ( ICON_HELP ) );
  m_ui->quickHelpButton->setShortcut ( QKeySequence ( "Ctrl+H" ) );

  // full screen
  m_ui->FullScreenButton->setIcon ( QIcon ( ICON_SCREEN ) );
  m_ui->FullScreenButton->setShortcut ( QKeySequence ( "F11" ) );

  // save log
  m_ui->SaveLogButton->setIcon ( QIcon ( ICON_SAVE ) );
}

/**
 * @brief main if pid sucess is execute mapper heap and stack
 *
 */
void MainWindow::mainMapper()
{
  column_delete ( m_ui->view_address );
  column_clean_all();

  try
  {
    QFile file ( QString::fromStdString ( "/proc/" + std::to_string ( m_pid ) + "/exe" ) );

    if ( file.open ( QIODevice::ReadOnly ) )
    {
      QByteArray bytes = file.readAll();
      m_hex->setData ( new QHexView::DataStorageArray ( bytes ) );
    }

    verify_pid();
    set_values_column_utils();

    if ( mapper_stack )
      set_values_column_stack();

    if ( mapper_heap )
      set_values_column_heap();
  }
  catch ( std::exception &error )
  {
    QMessageBox::about ( nullptr, "Warning", error.what() );
    return;
  }
}

/**
 * @brief set pid for verify and mapper
 *
 */
void MainWindow::on_pidButton_triggered()
{
  column_clean_all();
  DirWindow dir;
  dir.exec();

  m_pid = 0;

  while ( m_pid == 0 )
  {
    m_pid = dir.getPid();

    if ( m_pid == 0 )
      break;
    else
    {
      column_clean_all();
      mainMapper();
    }
  }
}

/**
 * @brief verify pid for mapper
 *
 */
void MainWindow::verify_pid()
{
  if ( m_mapper.map_pid ( m_pid ) == PID_SUCCESS )
  {
    m_pid_name = QString::fromStdString ( m_mapper.get_utilsPid ( NAME ) );
    m_pid_cmdline = QString::fromStdString ( m_mapper.get_utilsPid ( CMDLINE ) );
    m_pid_loginuid = QString::fromStdString ( m_mapper.get_utilsPid ( LOGINUID ) );
    m_pid_sizebin = QString::fromStdString ( m_mapper.get_utilsPid ( SIZEBIN ) );
    m_pid_wchan = QString::fromStdString ( m_mapper.get_utilsPid ( WCHAN ) );
    m_pid_blksize = QString::fromStdString ( m_mapper.get_utilsPid ( BLOCKSIZEBIN ) );
    m_pid_exedir = QString::fromStdString ( m_mapper.get_utilsPid ( EXEDIR ) );

    // tell the status bar which pid is being mapped
    setWindowTitle ( "maProc - PID: " + QString::fromStdString ( std::to_string ( m_pid ) ) + " Name: " + m_pid_name );
  }
  else
    m_pid = 0;
}

/**
 * @brief conf all tables overview, search
 *
 */
void MainWindow::column_config_all()
{
  // view address
  m_ui->view_address->horizontalHeader()->setSectionResizeMode ( QHeaderView::Stretch );
  m_ui->view_address->setShowGrid ( false );
  m_ui->view_address->setEditTriggers ( QAbstractItemView::NoEditTriggers );
  m_ui->view_address->verticalHeader()->setVisible ( false );

  // infos addr heap and stack
  m_ui->infos_addr->horizontalHeader()->setSectionResizeMode ( QHeaderView::Stretch );
  m_ui->infos_addr->setEditTriggers ( QAbstractItemView::NoEditTriggers );
  m_ui->infos_addr->setShowGrid ( false );

  // infos pid config
  m_ui->infos_pid->setEditTriggers ( QAbstractItemView::NoEditTriggers );
  m_ui->infos_pid->horizontalHeader()->setSectionResizeMode ( QHeaderView::Stretch );
  m_ui->infos_pid->setShowGrid ( false );

  // infos file config
  m_ui->infos_file->setEditTriggers ( QAbstractItemView::NoEditTriggers );
  m_ui->infos_file->horizontalHeader()->setSectionResizeMode ( QHeaderView::Stretch );
  m_ui->infos_file->setShowGrid ( false );

  // log infos
  m_ui->log_text->setReadOnly ( true );

  // maps table
  m_ui->maps_table->horizontalHeader()->setSectionResizeMode ( QHeaderView::Stretch );
  m_ui->maps_table->setEditTriggers ( QAbstractItemView::NoEditTriggers );

  // hex dump
  m_layout->addWidget ( m_hex );
  m_ui->HexViewTab->setLayout ( m_layout );

  // config infos type OS
  m_sys_hostname = QString::fromStdString ( m_mapper.get_utilsPid ( HOSTNAME ) );
  m_sys_osrealese = QString::fromStdString ( m_mapper.get_utilsPid ( OSREALESE ) );
  m_sys_version = QString::fromStdString ( m_mapper.get_utilsPid ( VERSION ) );
  m_sys_type = QString::fromStdString ( m_mapper.get_utilsPid ( TYPE ) );

  m_ui->host_name_label->setText ( "Host-Name :  " + m_sys_hostname );
  m_ui->os_realese_label->setText ( "OS-Realese :  " + m_sys_osrealese );
  m_ui->version_label->setText ( "Version :  " + m_sys_version );
  m_ui->os_type_label->setText ( "OS-Type :  " +  m_sys_type );
  
  // elf header
  m_ui->elfheader_table->horizontalHeader()->setSectionResizeMode ( QHeaderView::Stretch );
  m_ui->elfheader_table->setEditTriggers ( QAbstractItemView::NoEditTriggers );

}

/**
 * @brief ser values in infos overview
 *
 */
void MainWindow::set_values_column_utils()
{
  // table files infos
  m_ui->infos_file->setItem ( 0, 0, new QTableWidgetItem ( m_pid_exedir ) );
  m_ui->infos_file->setItem ( 0, 1, new QTableWidgetItem ( m_pid_sizebin + "B" ) );
  m_ui->infos_file->setItem ( 0, 2, new QTableWidgetItem ( m_pid_blksize + "B" ) );

  // table pid infos
  m_ui->infos_pid->setItem ( 0, 0, new QTableWidgetItem ( m_pid_name ) );
  m_ui->infos_pid->setItem ( 1, 0, new QTableWidgetItem ( QString::fromStdString ( std::to_string ( m_pid ) ) ) );
  m_ui->infos_pid->setItem ( 2, 0, new QTableWidgetItem ( m_pid_loginuid ) );
  m_ui->infos_pid->setItem ( 3, 0, new QTableWidgetItem ( m_pid_wchan ) );
  m_ui->infos_pid->setItem ( 4, 0, new QTableWidgetItem ( m_pid_cmdline ) );
}

/**
 * @brief set p_offsetss in heap column
 *
 */
void MainWindow::set_values_column_heap()
{
  m_ui->view_address->setShowGrid ( false );
  m_ui->view_address->setEditTriggers ( QAbstractItemView::NoEditTriggers );

  // infos_addr
  QString on = QString::number ( m_mapper.get_addrOn(), 16 );
  QString off = QString::number ( m_mapper.get_addrOff(), 16 );
  QString size = QString::number ( m_mapper.get_sizeAddress() );
  QString flags = QString::fromStdString ( m_mapper.get_Flags() );

  // set itens
  m_ui->infos_addr->setItem ( 0, Address_on, new QTableWidgetItem ( "0x" + on ) );
  m_ui->infos_addr->setItem ( 0, Address_off, new QTableWidgetItem ( "0x" + off ) );
  m_ui->infos_addr->setItem ( 0, Size_map, new QTableWidgetItem ( size ) );
  m_ui->infos_addr->setItem ( 0, Flags, new QTableWidgetItem ( flags ) );
}

/**
 * @brief set p_offsetss in stack column
 *
 */
void MainWindow::set_values_column_stack()
{
  // infos_addr
  QString on = QString::number ( m_mapper.get_addrOn(), 16 );
  QString off = QString::number ( m_mapper.get_addrOff(), 16 );
  QString size = QString::number ( m_mapper.get_sizeAddress() );
  QString flags = QString::fromStdString ( m_mapper.get_Flags() );

  // set itens
  m_ui->infos_addr->setItem ( 1, Address_on, new QTableWidgetItem ( "0x" + on ) );
  m_ui->infos_addr->setItem ( 1, Address_off, new QTableWidgetItem ( "0x" + off ) );
  m_ui->infos_addr->setItem ( 1, Size_map, new QTableWidgetItem ( size ) );
  m_ui->infos_addr->setItem ( 1, Flags, new QTableWidgetItem ( flags ) );
}

/**
 * @brief set types possibles for search mem
 *
 */
void MainWindow::set_types_edit_read()
{
  m_typeSizes.insert ( std::make_pair<std::string, size_t> ( "char", sizeof ( char ) ) );
  m_typeSizes.insert ( std::make_pair<std::string, size_t> ( "int", sizeof ( int ) ) );
  m_typeSizes.insert ( std::make_pair<std::string, size_t> ( "int16", sizeof ( int16_t ) ) );
  m_typeSizes.insert ( std::make_pair<std::string, size_t> ( "uint64", sizeof ( uint64_t ) ) );
  m_typeSizes.insert ( std::make_pair<std::string, size_t> ( "string", sizeof ( std::string ) ) );
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
void MainWindow::mapper_find ( off_t p_addr, off_t p_length, std::string p_find,
                               uint8_t p_type, std::vector<off_t> &p_offsets )
{
  try
  {
    int valueMax = p_offsets.size();
    // search for locations that have
    // already been found in the table
    if ( m_ui->checkCache->isChecked() )
    {

      // progress bar
      m_ui->progressFind->setValue(0);

      std::string find = m_ui->find->text().toStdString();
      int rowCont = m_ui->view_address->rowCount();
      std::size_t size = find.size();

      Data data ( size );

      for ( int it = 0; it < rowCont; it++ )
      {
        std::string str;
        off_t addr = std::stoul ( m_ui->view_address->item ( it, Address )->text().toStdString(), nullptr, 16 );
        m_mapper.map_read ( addr, size, data );

        for ( int i = 0; i < data.m_size; i++ )
          str += data.m_buff[i];

        if ( str == find )
        {
          m_ui->view_address->showRow ( it );
          m_ui->view_address->setItem ( it, Value, new QTableWidgetItem ( QString ( QString::fromStdString ( find ) ) ) );
        }
        else
          m_ui->view_address->hideRow ( it );

        m_ui->progressFind->setValue(it);
      }

      if(m_ui->progressFind->value() < valueMax)
          m_ui->progressFind->setValue(valueMax);

      data.clear();
    }
    else
    {
      //  will start mapping
      // from 0 all the memory
      column_delete ( m_ui->view_address );

      if ( m_mapper.map_find ( p_addr, p_length, p_find, p_type, p_offsets ) == READ_FAIL )
        QMessageBox::critical ( nullptr, "Fail Read", "Not read memory, error in start  0x" + QString::number ( p_addr, 16 ) );
    }

    QString sizeFound = QString::fromStdString ( std::to_string (valueMax ) );
    write_log ( "[SEARCH] PID [" + QString::fromStdString ( std::to_string ( m_pid ) ) + "] searched in memory start [0x" + QString::number ( p_addr, 16 ) + "] [" + QString::fromStdString ( p_find ) + "] found these addresses with such values [" + sizeFound + "]" );

    m_ui->foundAddr_label->setText ( "Found : " + sizeFound );

  }
  catch ( std::exception &error )
  {
    m_ui->foundAddr_label->setText ( "Found : 0" );
    QMessageBox::critical ( nullptr, "Not Read", error.what() );
    write_log ( "[ERROR] not read memory " + QString::fromStdString ( error.what() ) );
  }
}

void MainWindow::on_closeButton_triggered()
{
  QMessageBox::StandardButton close = QMessageBox::question ( this, "Close", "Close this Program ?", QMessageBox::Yes | QMessageBox::No );

  if ( close == QMessageBox::Yes )
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
  mm->setAttribute ( Qt::WA_DeleteOnClose );
  mm->show();
}

/**
 * @brief search value in  fetch value from an address
 * on the stack or heap, will get the type using the combo box
 */
void MainWindow::on_searchButton_clicked()
{
  std::string varType = m_ui->type->currentText().toStdString();
  std::string find = m_ui->find->text().toStdString();
  std::size_t mem = m_ui->mem->currentIndex();
  auto it = m_typeSizes.find ( varType );

  if ( find.size() != 0 && m_pid != 0 && it != m_typeSizes.end() )
  {
    QTableWidgetItem *addr;
    QTableWidgetItem *size;
    uint64_t lenght;
    std::vector<off_t> p_offsetss;
    off_t address_start;

    switch ( mem )
    {
      case 0: // stack
        addr = m_ui->infos_addr->item ( 1, 0 );
        size = m_ui->infos_addr->item ( 1, 2 );

        if ( addr->text().toStdString() != NULL_STR )
        {
          address_start = static_cast<off_t> ( std::stoul ( addr->text().toStdString(), nullptr, 16 ) );
          lenght = std::stoul ( size->text().toStdString(), nullptr );

          mapper_find ( address_start, lenght, find, it->second, p_offsetss );

          if ( p_offsetss.size() != 0 )
            set_values_column_address ( p_offsetss, find, "stack" );
        }

        break;

      case 1: // heap
        addr = m_ui->infos_addr->item ( 0, 0 );
        size = m_ui->infos_addr->item ( 0, 2 );

        if ( addr->text().toStdString() != NULL_STR )
        {
          address_start = static_cast<off_t> ( std::stoul ( addr->text().toStdString(), nullptr, 16 ) );
          lenght = std::stoul ( size->text().toStdString(), nullptr );

          mapper_find ( address_start, lenght, find, it->second, p_offsetss );

          if ( p_offsetss.size() != 0 )
            set_values_column_address ( p_offsetss, find, "heap" );
        }

        break;

      default:
        return;
    }
  }
  else if ( it == m_typeSizes.end() ) // critical error, will exit the program
  {
    QMessageBox::critical ( nullptr, "CRITICAL ERROR", "Error in code not possible find type\n verify unordered_map[TypeSizes]" );
    exit ( EXIT_FAILURE );
  }
}

/**
 * @brief edit address value
 *
 */
void MainWindow::on_editButton_clicked()
{
  off_t address = valid_address_edit();
  std::string varType = m_ui->type->currentText().toStdString();
  auto it = m_typeSizes.find ( varType );
  std::string value = m_ui->value->text().toStdString();

  if ( it != m_typeSizes.end() && address != 0 && m_pid != 0 && value.size() != 0 )
  {
    std::size_t SizeT = ( varType == "string" ) ? value.size() : it->second;

    if ( m_mapper.map_write ( address, ( void * ) &value[0], SizeT ) == true ){
      write_log ( "[EDITED] Memory edited address [0x" + QString::number ( address, 16 ) + "]  [" + m_ui->find->text() + "] - > [" + QString::fromStdString ( value ) + "]" );
    }
    else
      write_log ( "[ERROR] Fail edit memory [0x" + QString::number ( address, 16 ) + "]  [" + m_ui->find->text() + "] - > [" + QString::fromStdString ( value ) + "]" );
  }
  else if ( it == m_typeSizes.end() ) // critical error, will exit the program
  {
    QMessageBox::critical ( nullptr, "CRITICAL ERROR", "Error in code not possible find type\n verify unordered_map[TypeSizes]" );
    exit ( EXIT_FAILURE );
  }
}

/**
 * @brief repasser pid if cleaned infos
 *
 */
void MainWindow::on_rpidButton_triggered()
{
  if ( m_pid != 0 )
    mainMapper();
}

/**
 * @brief about button for infos copyright and project information
 *
 */
void MainWindow::on_aboutButton_triggered()
{
  m_dialog.reset ( new QDialog ( this ) );
  Ui_m_about aboutUi;
  aboutUi.setupUi ( m_dialog.get() );

  m_dialog->showNormal();
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

  if ( address_edit == NULL_STR )
    QMessageBox::critical ( nullptr, "ERROR", "Address NULL not valid, set address valid for edit value" );
  else
    address = static_cast<off_t> ( std::stoul ( address_edit, nullptr, 16 ) );

  return address;
}

/**
 * @brief will set all addresses and values ​​found in the search
 *
 * @param p_offsets vector containing the address
 * @param value value find
 * @param memory memory in which the value was found
 */
void MainWindow::set_values_column_address ( std::vector<off_t> &p_offsets, std::string p_value, std::string p_memory )
{
  int i = 0;
  int valueMax = p_offsets.size();
  m_ui->progressFind->reset();
  m_ui->progressFind->setMaximum(valueMax);

  column_delete ( m_ui->view_address )

  for ( auto &x : p_offsets )
  {
    m_ui->view_address->insertRow ( m_ui->view_address->rowCount() );

    int rowCount = m_ui->view_address->rowCount() - 1;
    QString addr = QString::number ( x, 16 );

    m_ui->view_address->setItem ( rowCount, Address, new QTableWidgetItem ( "0x" + addr ) );
    m_ui->view_address->setItem ( rowCount, Value, new QTableWidgetItem ( QString ( QString::fromStdString ( p_value ) ) ) );
    m_ui->view_address->setItem ( rowCount, Memory, new QTableWidgetItem ( QString ( QString::fromStdString ( p_memory ) ) ) );

    p_offsets.pop_back();
    m_ui->progressFind->setValue(i);
    i++;
  }

  if(m_ui->progressFind->value() < valueMax)
      m_ui->progressFind->setValue(valueMax);
}

/**
 * @brief set address for edit value
 *
 * @param p_first address double clicked
 */
void MainWindow::view_address_table ( QTableWidgetItem *p_first )
{
  QString selectedAddress = m_ui->view_address->item ( p_first->row(), Address )->text();
  QString selectedValue = m_ui->view_address->item ( p_first->row(), Value )->text();
  QString selectedMemory = m_ui->view_address->item ( p_first->row(), Memory )->text();
  m_ui->address_edit->setText ( selectedAddress );

  write_log ( "[CLICKED] Address [" + selectedAddress + "] Value [" + selectedValue + "]" + " Memory [" + selectedMemory + "]" );
}

/**
 * @brief clean all columns
 *
 */
void MainWindow::column_clean_all()
{
  setWindowTitle ( TITLE_WINDOW );
  column_delete ( m_ui->view_address );
  m_ui->address_edit->setText ( NULL_STR );
  m_hex->clear();
  m_ui->foundAddr_label->setText ( "Found : 0" );
  m_pid_name.clear();
  m_pid_wchan.clear();
  m_pid_exedir.clear();
  m_pid_loginuid.clear();
  m_pid_cmdline.clear();
  m_ui->progressFind->reset();

  for ( int i = 5; i >= 0; i-- )
  {
    m_ui->infos_addr->setItem ( i, Address_on, new QTableWidgetItem ( NULL_STR ) );
    m_ui->infos_addr->setItem ( i, Address_off, new QTableWidgetItem ( NULL_STR ) );
    m_ui->infos_addr->setItem ( i, Size_map, new QTableWidgetItem ( "0" ) );
    m_ui->infos_addr->setItem ( i, Flags, new QTableWidgetItem ( "----" ) );

    m_ui->infos_pid->setItem ( i, 0, new QTableWidgetItem ( CLEAN_ROW ) );
    m_ui->infos_file->setItem ( i, 0, new QTableWidgetItem ( CLEAN_ROW ) );
  }
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
  if(m_pid_name.size() == 0)
    return;

  try
  {
    if ( m_ui->stopButton->text() == "Stop Pid" )
    {
      m_mapper.map_stop();
      m_ui->stopButton->setText ( "Play Pid" );
      m_ui->stopButton->setIcon ( QIcon ( ICON_PLAY ) );
      m_pid_wchan = QString::fromStdString ( m_mapper.get_utilsPid ( WCHAN ) );
      m_ui->infos_pid->setItem ( 3, 0, new QTableWidgetItem ( m_pid_wchan ) );
      write_log ( "[SIGNAL STOP] you send a signal to the PID=" + QString::fromStdString ( std::to_string ( m_pid ) ) + ", sending it on a stop in the process" )
    }
    else if ( m_ui->stopButton->text() == "Play Pid" )
    {
      m_mapper.map_stop ( false );
      m_ui->stopButton->setText ( "Stop Pid" );
      m_ui->stopButton->setIcon ( QIcon ( ICON_STOP ) );
      m_pid_wchan = QString::fromStdString ( m_mapper.get_utilsPid ( WCHAN ) );
      m_ui->infos_pid->setItem ( 3, 0, new QTableWidgetItem ( m_pid_wchan ) );
      write_log ( "[SIGNAL RUNNING] you send a signal to the PID=" + QString::fromStdString ( std::to_string ( m_pid ) ) + ", sending it on a running in the process" )
    }
  }
  catch ( std::exception &error )
  {
    QMessageBox::critical ( nullptr, "ERROR", error.what() );
    column_clean_all();
  }
}

/**
 * @brief kill process button
 *
 */
void MainWindow::on_killButton_triggered()
{
  if(m_pid_name.size() == 0)
    return;

  QMessageBox::StandardButton kill = QMessageBox::question ( this, "SIGNAL KILL", "Kill Process " + m_pid_name + "? ", QMessageBox::Yes | QMessageBox::No );

  if ( kill == QMessageBox::Yes )
  {
    m_pid = 0;
    m_mapper.map_kill();
    column_clean_all();
    write_log ( "[SIGNAL KILL] you send a signal to the PID=" + QString::fromStdString ( std::to_string ( m_pid ) ) + ", sending it on a kill in the process" )
  }
}

/**
 * @brief search table view address
 *
 * @param arg1 search address table view_address
 */
void MainWindow::on_search_address_textEdited ( const QString &p_arg1 )
{
  for ( int i = 0; i < m_ui->view_address->rowCount(); i++ )
    m_ui->view_address->hideRow ( i );

  QList<QTableWidgetItem *> search = m_ui->view_address->findItems ( p_arg1, Qt::MatchContains );

  foreach ( auto &Ptr, search )
  {
    m_ui->view_address->showRow ( Ptr->row() );
    m_ui->foundAddr_label->setText ( "Found " + QString::number ( search.size() ) );
  }

  if ( p_arg1.size() == 0 )
    m_ui->foundAddr_label->setText ( "Found " + QString::number ( m_ui->view_address->rowCount() ) );

  search.clear();
}


/**
 * @brief set offset for jumṕ
 * */
void MainWindow::on_gotooffsetButton_triggered()
{
  bool done;
  int offset = QInputDialog::getInt ( 0, "Offset", "Offset:", 0, 0, 2147483647, 1, &done );

  if ( done )
    m_hex->showFromOffset ( offset );
}


/**
 * @brief open browser for help maProc
 * */
void MainWindow::on_quickHelpButton_triggered()
{
  QDesktopServices::openUrl ( QUrl ( "https://github.com/mentebinaria/maProc/wiki/Help" ) );
}


/**
 * @brief full screen window
 * */
void MainWindow::on_FullScreenButton_triggered()
{
  if ( isFullScreen() )
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

  if ( textLog.size() != 0 )
  {
    bool ok;
    QString text = QInputDialog::getText ( this, tr ( "Save Logging" ),
                                           tr ( "File Name:" ), QLineEdit::Normal,
                                           m_pid_name+QString("Log"), &ok );

    if ( !text.isEmpty() )
    {
      QFile file(text+QString(".mPl"));
      if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
      else
      {
        file.write(textLog.toUtf8().data(), textLog.size());
        file.close();
        QMessageBox::information(nullptr, "SUCESSS", "Logging saved is sucessfull"); 
      }
    }
  
  }
  else
    QMessageBox::critical ( nullptr, "ERROR", "Logging is empty, active checkbox []Log" );
}
