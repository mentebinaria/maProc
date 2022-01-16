#pragma once

#include <QMainWindow>
#include <QDialog>

#include "include/ps.hpp"
#include "pmap.hpp"

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

    void Button_clicked();

private slots:
    void column_clean_all();

    void conf_button_clean();
    void conf_button_pass_pid();
    void conf_button_search();
    void conf_button_close();
    void conf_button_edit();
    void conf_button_new();

    void on_pidButton_clicked();

    void verify_pid();

    void column_config_all();
    void set_values_column_heap();
    void set_values_column_stack();

    bool mapper_heap();
    bool mapper_stack();

    void on_closeButton_triggered();
    void on_cleanButton_triggered();

    void on_pidButton_2_triggered();

    void on_newButton_triggered();

private:
    pid_t pid;

    Ps ps;
    Pmap mapper;
    Ui::MainWindow *ui;
};
