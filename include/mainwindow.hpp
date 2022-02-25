#pragma once

#include <QMainWindow>
#include <QDialog>
#include <unordered_map>
#include <QTableWidgetItem>
#include <vector>

#include "include/hexview.hpp"
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

    void mainMapper();

private slots:
    void verify_pid();
    bool mapper_heap();
    bool mapper_stack();
    void mapper_find(off_t __addr, off_t __length, std::__cxx11::string __find,
                     uint8_t __type, std::vector<off_t> &__offsets);

    off_t valid_address_edit();
    void column_clean_all();
    void conf_button_all();
    void column_config_all();
    void set_values_column_utils();
    void set_values_column_address(std::vector<off_t> &offset, std::string value, std::string memory);
    void view_address_table(QTableWidgetItem *p_first);
    void set_values_column_heap();
    void set_values_column_stack();
    void set_types_edit_read();

    void on_pidButton_clicked();
    void on_searchButton_clicked();
    void on_cleanButton_triggered();
    void on_closeButton_2_triggered();
    void on_closeButton_clicked();
    void on_cleanButton2_clicked();
    void on_hexButton_clicked();
    void on_pidButton_2_triggered();
    void on_newButton_triggered();
    void on_editButton_clicked();
    void on_aboutButton_triggered();
    void on_rpidButton_clicked();

private:
    int m_type;
    pid_t m_pid;
    QString m_pid_name;
    QString m_pid_cmdline;
    QString m_pid_loginuid;
    QString m_pid_sizebin;
    QString m_pid_wchan;
    QString m_pid_blksize;
    QString m_sys_hostname;
    QString m_sys_osrealese;
    QString m_sys_version;
    QString m_sys_type; 

    std::unordered_map<std::string, size_t> m_typeSizes;
    Ui::MainWindow *m_ui;
    Ps m_ps;
    Pmap m_mapper;
    HexView m_hex;
};
