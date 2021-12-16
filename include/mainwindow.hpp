#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QTableWidget>
#include <QMainWindow>

#include "mapped.hpp"
#include "dirprocess.hpp"

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

    void conf_button_clean();
    void conf_button_pass_pid();
    void conf_button_search();

    void on_mapButton_clicked();
    void on_pidButton_clicked();
    void on_cleanButton_clicked();

    bool verify_pid();

    void column_config_all();
    void set_values_column_heap();
    void set_values_column_stack();

    void column_clean(QTableWidget *__column, bool __delete);
    
    bool mapper_heap();
    bool mapper_stack();


private:
    std::string pid;

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

    Address_info addr;
    mapper_memory mapper;
    DirProcess dir;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_HPP