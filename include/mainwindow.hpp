#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QTableWidget>
#include <QMainWindow>

#include "mapped.hpp"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_mapButton_clicked();
    bool verify_pid();

    void column_config_all();
    void set_values_column_heap();
    void set_values_column_stack();

    void column_clean(QTableWidget *__column, bool __delete);
    
    bool mapper_heap();
    bool mapper_stack();

    void on_cleanButton_clicked();

private:
    enum Heap_Stack
    {
        Address,
        Value
    };

    enum Infos
    {
        Address_on,
        Address_off,
        Size_map
    };

    Ui::MainWindow *ui;
    std::string pid;

    mapper_memory mapper;

};

#endif // MAINWINDOW_HPP