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
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(TITLE_WINDOW);
    setWindowIcon(QIcon(ICON_WINDOW));

    // status bar conf default
    ui->statusBar->showMessage("No process is being mapped at this time");
    // get value type combo button
    type = ui->type->currentIndex();

    // buttons conf
    conf_button_pass_pid();
    conf_button_clean();
    conf_button_search();
    conf_button_close();
    conf_button_edit();
    conf_button_new();
    conf_button_about();

    // tables conf
    column_config_all();
    

    // Initialize var sizes

   
    typeSizes.insert(std::make_pair<std::string, <size_t, size_t>("char", sizeof(char), INT8_MAX));
    // typeSizes.insert(std::make_pair<std::string, size_t>("int", sizeof(int)));
    // typeSizes.insert(std::make_pair<std::string, size_t>("int8", sizeof(int8_t)));
    // typeSizes.insert(std::make_pair<std::string, size_t>("int16", sizeof(int16_t)));
    // typeSizes.insert(std::make_pair<std::string, size_t>("uint32", sizeof(uint32_t)));
    // typeSizes.insert(std::make_pair<std::string, size_t>("uint64", sizeof(uint64_t)));
    // typeSizes.insert(std::make_pair<std::string, size_t>("float", sizeof(float)));
    // typeSizes.insert(std::make_pair<std::string, size_t>("string", sizeof(uint8_t)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::conf_button_pass_pid()
{
    ui->pidButton->setIcon(QIcon(ICON_PASS_PID));
    ui->pidButton_2->setIcon(QIcon(ICON_PASS_PID));
}

void MainWindow::conf_button_about()
{
    ui->aboutButton->setIcon(QIcon(ICON_ABOUT));
}

void MainWindow::conf_button_clean()
{
    ui->cleanButton->setIcon(QIcon(ICON_CLEAN));
    ui->cleanButton2->setIcon(QIcon(ICON_CLEAN));
}

void MainWindow::conf_button_search()
{
    ui->searchButton->setIcon(QIcon(ICON_SEARCH));
}

void MainWindow::conf_button_close()
{
    ui->closeButton->setIcon(QIcon(ICON_CLOSE));
}

void MainWindow::conf_button_edit()
{
    ui->editButton->setIcon(QIcon(ICON_EDIT));
}

void MainWindow::conf_button_new()
{
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
            return;
        else
            Button_clicked();
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
    ui->infos_file->setItem(0, 1, new QTableWidgetItem(pid_sizebin));

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

bool MainWindow::mapper_heap()
{
    return mapper.map_mem("[heap]");
}

bool MainWindow::mapper_stack()
{
    return mapper.map_mem("[stack]");
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
    std::string varType = ui->type->currentText().toStdString();
    
    auto it = typeSizes.find(varType);

    if (it != typeSizes.end()) {
        mapper.map_find(it->second, targetValue);
    } else {
         throw std::runtime_error("Type not found");
    }

}

void MainWindow::on_editButton_clicked()
{
    std::string varType = ui->type->currentText().toStdString();
    auto it = typeSizes.find(varType);

    if (it != typeSizes.end()) {
        mapper.map_write(0, it->second);
    } else {
         throw std::runtime_error("Type not found");
    }
}

/**
 * @brief set value in label for edit value in address
 *
 * @param row
 * @param column
 */
void MainWindow::on_view_address_cellDoubleClicked(int row, int column)
{
}

/**
 * @brief about button for infos copyright and project information
 *
 */
void MainWindow::on_aboutButton_triggered()
{
}