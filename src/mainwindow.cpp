#include "include/mainwindow.hpp"
#include "include/datastructs/win_utils.hpp"
#include "include/dirwindow.hpp"
#include "gui/ui_mainwindow.h"

#include <iostream>
#include <string>
#include <QMessageBox>
#include <limits>

#define column_delete(__column)          \
    {                                    \
        while (__column->rowCount() > 0) \
            __column->removeRow(0);      \
    }

void MainWindow::column_clean_all()
{
    for (int i = 5; i >= 0; i--)
    {
        ui->infos_addr->setItem(i, Address_on, new QTableWidgetItem("null"));
        ui->infos_addr->setItem(i, Address_off, new QTableWidgetItem("null"));
        ui->infos_addr->setItem(i, Size_map, new QTableWidgetItem("0"));

        ui->infos_pid->setItem(i, 0, new QTableWidgetItem(CLEAN_ROW));
        ui->infos_file->setItem(i, 0, new QTableWidgetItem(CLEAN_ROW));
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow),
                                          pid(0)
{
    ui->setupUi(this);
    setWindowTitle(TITLE_WINDOW);
    setWindowIcon(QIcon(ICON_WINDOW));
    setMinimumWidth(500);
    setMinimumHeight(500);

    // status bar conf default
    ui->statusBar->showMessage("No process is being mapped at this time");

    // get value type combo button
    type = ui->type->currentIndex();

    // buttons conf
    conf_button_all();

    // tables conf
    column_config_all();

    // set values for edit address and read
    set_types_edit_read();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::conf_button_all()
{
    // buttons for pass pid
    ui->pidButton->setIcon(QIcon(ICON_PASS_PID));
    ui->pidButton_2->setIcon(QIcon(ICON_PASS_PID));

    // buttons for clean
    ui->cleanButton->setIcon(QIcon(ICON_CLEAN));
    ui->cleanButton2->setIcon(QIcon(ICON_CLEAN));

    // buttons for close
    ui->closeButton->setIcon(QIcon(ICON_CLOSE));
    ui->closeButton_2->setIcon(QIcon(ICON_CLOSE));

    // search button
    ui->searchButton->setIcon(QIcon(ICON_SEARCH));

    // edit button
    ui->editButton->setIcon(QIcon(ICON_EDIT));

    // about button
    ui->aboutButton->setIcon(QIcon(ICON_ABOUT));

    // new button
    ui->newButton->setIcon(QIcon(ICON_NEW));
}

void MainWindow::Button_clicked()
{
    column_delete(ui->view_address);
    column_clean_all();

    try
    {
        verify_pid();
        set_values_column_utils();

        if (mapper_stack())
            set_values_column_stack();

        if (mapper_heap())
            set_values_column_heap();
    }
    catch (std::exception &error)
    {
        QMessageBox::about(nullptr, "Warning", error.what());
        return;
    }
}

void MainWindow::on_pidButton_clicked()
{
    DirWindow dir;
    dir.exec();

    pid = 0;
    while (pid == 0)
    {
        pid = dir.getPid();
        if (pid == 0)
            break;
        else
        {
            column_clean_all();
            Button_clicked();
        }
    }
}

void MainWindow::verify_pid()
{
    int status_pid = mapper.map_pid(pid);

    if (status_pid == 0)
    {
        pid_name = QString::fromStdString(mapper.get_utilsPid(NAME));
        pid_cmdline = QString::fromStdString(mapper.get_utilsPid(CMDLINE));
        pid_loginuid = QString::fromStdString(mapper.get_utilsPid(LOGINUID));
        pid_sizebin = QString::fromStdString(mapper.get_utilsPid(SIZEBIN));
        pid_wchan = QString::fromStdString(mapper.get_utilsPid(WCHAN));
        pid_blksize = QString::fromStdString(mapper.get_utilsPid(BLOCKSIZEBIN));

        // tell the status bar which pid is being mapped
        ui->statusBar->showMessage("PID: " + QString::fromStdString(std::to_string(pid)) + " Name: " + pid_name);
    }
}

void MainWindow::column_config_all()
{
    QStringList view_address;
    view_address << "Address"
                 << "Value"
                 << "Memory";

    QStringList infos_addr;
    infos_addr << "Address_on"
               << "Address_off"
               << "Size_map";

    QStringList infos_pid;
    infos_pid << "InfosPid";

    // ui->infosView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // view address
    ui->view_address->setColumnCount(3);
    ui->view_address->setHorizontalHeaderLabels(view_address);
    ui->view_address->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->view_address->setShowGrid(false);
    ui->view_address->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // infos addr heap and stack
    ui->infos_addr->setColumnCount(3);
    ui->infos_addr->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->infos_addr->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->infos_addr->setHorizontalHeaderLabels(infos_addr);
    ui->infos_addr->setShowGrid(false);

    // infos pid config
    ui->infos_pid->setColumnCount(1);
    ui->infos_pid->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->infos_pid->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->infos_pid->setShowGrid(false);
    ui->infos_pid->setHorizontalHeaderLabels(infos_pid);

    // infos file config
    ui->infos_file->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->infos_file->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::set_values_column_utils()
{
    // table files infos
    ui->infos_file->setItem(0, 0, new QTableWidgetItem("/proc/" + QString::fromStdString(std::to_string(pid)) + "/exe"));
    ui->infos_file->setItem(0, 1, new QTableWidgetItem(pid_sizebin + "B"));
    ui->infos_file->setItem(0, 2, new QTableWidgetItem(pid_blksize + "B"));

    // table pid infos
    ui->infos_pid->setItem(0, 0, new QTableWidgetItem(pid_name));
    ui->infos_pid->setItem(1, 0, new QTableWidgetItem(QString::fromStdString(std::to_string(pid))));
    ui->infos_pid->setItem(2, 0, new QTableWidgetItem(pid_loginuid));
    ui->infos_pid->setItem(3, 0, new QTableWidgetItem(pid_wchan));
    ui->infos_pid->setItem(4, 0, new QTableWidgetItem(pid_cmdline));
}

void MainWindow::set_values_column_heap()
{
    ui->view_address->setShowGrid(false);
    ui->view_address->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // infos_addr
    QString on = QString::number(mapper.get_addrOn(), 16);
    QString off = QString::number(mapper.get_addrOff(), 16);

    ui->infos_addr->setShowGrid(false);
    ui->infos_addr->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // set itens
    ui->infos_addr->setItem(0, Address_on, new QTableWidgetItem(on));
    ui->infos_addr->setItem(0, Address_off, new QTableWidgetItem(off));
    ui->infos_addr->setItem(0, Size_map, new QTableWidgetItem(QString::number(mapper.get_sizeAddress())));
}

void MainWindow::set_values_column_stack()
{
    // table addr infos
    ui->infos_addr->setItem(1, Address_on, new QTableWidgetItem(QString::number(mapper.get_addrOn(), 16)));
    ui->infos_addr->setItem(1, Address_off, new QTableWidgetItem(QString::number(mapper.get_addrOff(), 16)));
    ui->infos_addr->setItem(1, Size_map, new QTableWidgetItem(QString::number(mapper.get_sizeAddress())));
}

void MainWindow::set_types_edit_read()
{
    typeSizes.insert(std::make_pair<std::string, size_t>("char", sizeof(char)));
    typeSizes.insert(std::make_pair<std::string, size_t>("int", sizeof(int)));
    typeSizes.insert(std::make_pair<std::string, size_t>("int8", sizeof(int8_t)));
    typeSizes.insert(std::make_pair<std::string, size_t>("int16", sizeof(int16_t)));
    typeSizes.insert(std::make_pair<std::string, size_t>("uint32", sizeof(uint32_t)));
    typeSizes.insert(std::make_pair<std::string, size_t>("uint64", sizeof(uint64_t)));
    typeSizes.insert(std::make_pair<std::string, size_t>("float", sizeof(float)));
    typeSizes.insert(std::make_pair<std::string, size_t>("string", sizeof(uint8_t)));
}

bool MainWindow::mapper_heap()
{
    return mapper.map_mem("[heap]");
}

bool MainWindow::mapper_stack()
{
    return mapper.map_mem("[stack]");
}

void MainWindow::on_closeButton_2_triggered()
{
    on_closeButton_clicked();
}

void MainWindow::on_closeButton_clicked()
{
    QMessageBox::StandardButton close = QMessageBox::question(this, "Close", "Close this Program ?", QMessageBox::Yes | QMessageBox::No);
    if (close == QMessageBox::Yes)
        this->close();
}

void MainWindow::on_cleanButton_triggered()
{
    column_clean_all();
    column_delete(ui->view_address);
}

void MainWindow::on_cleanButton2_clicked()
{
    on_cleanButton_triggered();
}

void MainWindow::on_pidButton_2_triggered()
{
    on_pidButton_clicked();
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
 * @brief search value in  fetch value from an address
 * on the stack or heap, will get the type using the combo box
 *
 */
void MainWindow::on_searchButton_clicked()
{
    QTableWidgetItem *addr;
    std::string varType = ui->type->currentText().toStdString();
    std::string find = ui->find->text().toStdString();
    std::size_t mem = ui->mem->currentIndex();
    auto it = typeSizes.find(varType);

    if (find.size() != 0 && pid != 0 && it != typeSizes.end())
    {
        switch (mem)
        {
        case 0: // stack
            addr = ui->infos_addr->item(1, 0);
            if (mapper.map_find(static_cast<off_t>(std::stoul(addr->text().toStdString(), nullptr, 16)), find, it->second) == READ_FAIL)
                QMessageBox::critical(nullptr, "ERROR", "Not possible read memory check offset");
            break;
        case 1: // heap
            addr = ui->infos_addr->item(0, 0);
            if (mapper.map_find(static_cast<off_t>(std::stoul(addr->text().toStdString(), nullptr, 16)), find, it->second) == READ_FAIL)
                QMessageBox::critical(nullptr, "ERROR", "Not possible read memory check offset");
            break;
        case 2: // all
            break;

        default:
            return;
        }
    }
    else if (it == typeSizes.end())
        throw std::runtime_error("[ERROR] Error in code not possible find type\n verify unordered_map[TypeSizes]");
}

/**
 * @brief edit address value
 *
 */
void MainWindow::on_editButton_clicked()
{
    off_t address = valid_address_edit();
    std::string varType = ui->type->currentText().toStdString();
    auto it = typeSizes.find(varType);

    if (it != typeSizes.end() && address != 0 && pid != 0)
        mapper.map_write(address, it->second);
    else if (it == typeSizes.end())
        throw std::runtime_error("[ERROR] Error in code not possible find type\n verify unordered_map[TypeSizes]");
}
/**
 * @brief about button for infos copyright and project information
 *
 */
void MainWindow::on_aboutButton_triggered()
{
}

/**
 * @brief
 *
 * @return off_t if address valid return offset, else address not valid return 0
 */
off_t MainWindow::valid_address_edit()
{
    std::string address_edit = ui->address_edit->text().toStdString();
    off_t address = 0;
    if (address_edit == "null")
        QMessageBox::critical(nullptr, "ERROR", "Address NULL not valid, set address valid for search/edit value");
    else
        address = static_cast<off_t>(std::stoul(address_edit, nullptr, 16));

    return address;
}

void MainWindow::on_view_address_cellDoubleClicked(int row, int column)
{

}