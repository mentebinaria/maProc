#include "include/mainwindow.hpp"
#include "include/datastructs/win_utils.hpp"
#include "include/dirwindow.hpp"
#include "ui_mainwindow.h"

#include <string>
#include <QMessageBox>

#define column_clean(__column, __delete)                                            \
    {                                                                               \
        if (__delete)                                                               \
        {                                                                           \
            while (__column->rowCount() > 0)                                        \
                __column->removeRow(0);                                             \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            for (int i = 1; i >= 0; i--)                                            \
            {                                                                       \
                __column->setItem(i, Address_on, new QTableWidgetItem(CLEAN_ROW));  \
                __column->setItem(i, Address_off, new QTableWidgetItem(CLEAN_ROW)); \
                __column->setItem(i, Size_map, new QTableWidgetItem(CLEAN_ROW));    \
            }                                                                       \
        }                                                                           \
    }

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(TITLE_WINDOW);
    setWindowIcon(QIcon(ICON_WINDOW));

    // status bar conf default
    ui->statusBar->showMessage("No processes are currently being mapped");

    // buttons conf
    conf_button_pass_pid();
    conf_button_clean();
    conf_button_search();
    conf_button_close();

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
}

void MainWindow::conf_button_clean()
{
    ui->statusBar->addPermanentWidget(ui->cleanButton);
    ui->cleanButton->setIcon(QIcon(ICON_CLEAN));
}

void MainWindow::conf_button_search()
{
    ui->searchButton->setIcon(QIcon(ICON_SEARCH));
}

void MainWindow::conf_button_close()
{
    ui->statusBar->addPermanentWidget(ui->closeButton);
    ui->closeButton->setIcon(QIcon(ICON_CLOSE));
}

void MainWindow::Button_clicked()
{
    column_clean(ui->viewAddress, true);
    column_clean(ui->viewAddress, true);
    column_clean(ui->infos_addr, false);

    try
    {
        verify_pid();

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

void MainWindow::on_cleanButton_clicked()
{
    column_clean(ui->viewAddress, true);
    column_clean(ui->viewAddress, true);
    column_clean(ui->infos_addr, false);
    column_clean(ui->infos_addr, false);
}

void MainWindow::on_closeButton_clicked()
{
    this->close();
}

void MainWindow::verify_pid()
{
    int status_pid = mapper.map_pid(pid);

    if (status_pid == 0)
        ui->statusBar->showMessage("Mapping process PID " + QString::fromStdString(std::to_string(pid))); // tell the status bar which pid is being mapped
}

void MainWindow::column_config_all()
{
    QStringList column;
    column << "Address"
           << "Value"
           << "Memory";

    QStringList column_infos;
    column_infos << "Address_on"
                 << "Address_off"
                 << "Size_map";

    ui->infosView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // view address
    ui->viewAddress->setColumnCount(3);
    ui->viewAddress->setHorizontalHeaderLabels(column);
    ui->viewAddress->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->infos_addr->setColumnCount(3);
    ui->infos_addr->setHorizontalHeaderLabels(column_infos);
}

void MainWindow::set_values_column_heap()
{
    // heaView config
    // int rowCount_heap = ui->viewAddress->rowCount();

    ui->viewAddress->setShowGrid(false);
    ui->viewAddress->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
    // viewAddress config
    // int rowCount_stack = ui->viewAddress->rowCount() - 1;

    ui->viewAddress->setShowGrid(false);
    ui->viewAddress->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // ui->viewAddress->insertRow(rowCount_heap);

    // infos_addr
    QString on = QString::number(mapper.get_addrOn(), 16);
    QString off = QString::number(mapper.get_addrOff(), 16);

    // set itens
    ui->infos_addr->setItem(1, Address_on, new QTableWidgetItem(on));
    ui->infos_addr->setItem(1, Address_off, new QTableWidgetItem(off));
    ui->infos_addr->setItem(1, Size_map, new QTableWidgetItem(QString::number(mapper.get_sizeAddress())));
}

bool MainWindow::mapper_heap()
{
    bool status_exit = true;
    if (mapper.map_mem("[heap]") != true)
        status_exit = false;

    return status_exit;
}

bool MainWindow::mapper_stack()
{
    bool status_exit = true;
    if (mapper.map_mem("[stack]") != true)
        status_exit = false;

    return status_exit;
}