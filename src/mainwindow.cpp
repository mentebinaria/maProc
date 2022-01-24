#include "include/mainwindow.hpp"
#include "include/datastructs/win_utils.hpp"
#include "include/dirwindow.hpp"
#include "gui/ui_mainwindow.h"

#include <string>
#include <QMessageBox>

#define column_delete(__column)          \
    {                                    \
        while (__column->rowCount() > 0) \
            __column->removeRow(0);      \
    }

void MainWindow::column_clean_all()
{
    for (int i = 5; i >= 0; i--)
    {
        ui->infos_addr->setItem(i, Address_on, new QTableWidgetItem(CLEAN_ROW));
        ui->infos_addr->setItem(i, Address_off, new QTableWidgetItem(CLEAN_ROW));
        ui->infos_addr->setItem(i, Size_map, new QTableWidgetItem(CLEAN_ROW));

        ui->infos_pid->setItem(i, 0, new QTableWidgetItem(CLEAN_ROW));
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(TITLE_WINDOW);
    setWindowIcon(QIcon(ICON_WINDOW));

    // status bar conf default
    ui->statusBar->showMessage("No process is being mapped at this time !!");

    // buttons conf
    conf_button_pass_pid();
    conf_button_clean();
    conf_button_search();
    conf_button_close();
    conf_button_edit();
    conf_button_new();

    // tables conf
    column_config_all();
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

void MainWindow::conf_button_clean()
{
    ui->cleanButton->setIcon(QIcon(ICON_CLEAN));
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

    if (dir.close())
    {
        pid = dir.getPid();
        Button_clicked();
    }
}

void MainWindow::verify_pid()
{
    int status_pid = mapper.map_pid(pid);

    if (status_pid == 0)
        ui->statusBar->showMessage("PID: " + QString::fromStdString(std::to_string(pid)) + " Name: " + QString::fromStdString(ps.Get_UtilsPid(pid, "comm"))); // tell the status bar which pid is being mapped
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

    // table pid infos
    ui->infos_pid->setItem(0, 0, new QTableWidgetItem(QString::fromStdString(ps.Get_UtilsPid(pid, "comm"))));
    ui->infos_pid->setItem(1, 0, new QTableWidgetItem(QString::fromStdString(std::to_string(pid))));
    ui->infos_pid->setItem(4, 0, new QTableWidgetItem(QString::fromStdString(ps.Get_UtilsPid(pid, "cmdline"))));
}

void MainWindow::set_values_column_heap()
{
    // heaView config
    // int rowCount_heap = ui->viewAddress->rowCount();

    ui->view_address->setShowGrid(false);
    ui->view_address->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // ui->viewAddress->insertRow(rowCount_heap);

    // infos_addr
    QString on = QString::number(mapper.get_addrOn(), 16);
    QString off = QString::number(mapper.get_addrOff(), 16);

    ui->infos_addr->setShowGrid(false);
    ui->infos_addr->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // QTableWidgetItem *Address_mapped = new QTableWidgetItem("done");
    // Address_mapped->setTextAlignment(100);

    // set itens
    ui->infos_addr->setItem(0, Address_on, new QTableWidgetItem(on));
    ui->infos_addr->setItem(0, Address_off, new QTableWidgetItem(off));
    ui->infos_addr->setItem(0, Size_map, new QTableWidgetItem(QString::number(mapper.get_sizeAddress())));

    // ui->viewAddress->setItem(rowCount_heap - 1, Address, Address_mapped);
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

void MainWindow::on_pidButton_2_triggered()
{
    on_pidButton_clicked();
}

void MainWindow::on_newButton_triggered()
{
    auto mm = new MainWindow();
    mm->setAttribute(Qt::WA_DeleteOnClose);
    mm->show();
}