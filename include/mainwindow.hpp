#pragma once

#include <QMainWindow>
#include <QDialog>
#include <unordered_map>

#include "include/ps.hpp"
#include "include/pmap.hpp"

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
    void verify_pid();
    bool mapper_heap();
    bool mapper_stack();

    void conf_button_all();

    off_t valid_address_edit();
    void column_clean_all();
    void column_config_all();
    void set_values_column_utils();
    void set_values_column_heap();
    void set_values_column_stack();
    void set_types_edit_read();

    void on_pidButton_clicked();
    void on_searchButton_clicked();
    void on_cleanButton_triggered();
    void on_closeButton_2_triggered();
    void on_closeButton_clicked();
    void on_cleanButton2_clicked();
    void on_pidButton_2_triggered();
    void on_newButton_triggered();
    void on_editButton_clicked();
    void on_view_address_cellDoubleClicked(int row, int column);
    void on_aboutButton_triggered();

private:
    int type;
    pid_t pid;
    QString pid_name;
    QString pid_cmdline;
    QString pid_loginuid;
    QString pid_sizebin;
    QString pid_wchan;
    QString pid_blksize;

    std::unordered_map<std::string, size_t> typeSizes;
    Ui::MainWindow *ui;
    Ps ps;
    Pmap mapper;
};
