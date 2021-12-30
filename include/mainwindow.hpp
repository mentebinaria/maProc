#pragma once

#include <QMainWindow>
#include <QDialog>

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
    void conf_button_clean();
    void conf_button_pass_pid();
    void conf_button_search();
    void conf_button_close();

    void on_pidButton_clicked();
    void on_cleanButton_clicked();
    void on_closeButton_clicked();

    void verify_pid();

    void column_config_all();
    void set_values_column_heap();
    void set_values_column_stack();

    bool mapper_heap();
    bool mapper_stack();

private:
    pid_t pid;

    Pmap mapper;
    Ui::MainWindow *ui;
};