#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QDialog>

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

    void Button_clicked(unsigned int __pid);

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
    unsigned int pid;

    mapper_memory mapper;
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_HPP