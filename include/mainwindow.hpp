#pragma once

#include <QMainWindow>
#include <QDialog>
#include <unordered_map>
#include <QTableWidgetItem>
#include <vector>
#include <memory>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

#include "include/qhexview.hpp"
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
    explicit MainWindow(QWidget * = nullptr);
    ~MainWindow();

    void mainMapper();
    pid_t m_pid;

private slots:
    void verify_pid();
    void mapper_find(off_t, off_t, std::string,
                     uint8_t, std::vector<off_t> &);

	off_t valid_address_edit();
    void column_clean_all();
    void conf_button_all();
    void column_config_all();
    void set_values_column_utils();
    void set_values_column_address(std::vector<off_t> &, std::string, std::string);
    void view_address_table(QTableWidgetItem *);
    void set_values_column_heap();
    void set_values_column_stack();
    void set_types_edit_read();

    void on_search_address_textEdited(const QString &arg1);
    void on_pidButton_clicked();
    void on_searchButton_clicked();
    void on_cleanButton_triggered();
    void on_closeButton_2_triggered();
    void on_closeButton_clicked();
    void on_cleanButton2_clicked();
    void on_pidButton_2_triggered();
    void on_newButton_triggered();
    void on_editButton_clicked();
    void on_aboutButton_triggered();
	void on_GotooffsetButton_triggered();
    void on_rpidButton_clicked();
    void on_killButton_clicked();
    void on_stopButton_clicked();
    void on_cleanButtonLog_clicked();

private:
    int m_type;
    QString m_pid_name;
    QString m_pid_cmdline;
    QString m_pid_loginuid;
    QString m_pid_sizebin;
    QString m_pid_wchan;
    QString m_pid_blksize;
    QString m_pid_exedir;
    QString m_sys_hostname;
    QString m_sys_osrealese;
    QString m_sys_version;
    QString m_sys_type;

    std::unordered_map<std::string, size_t> m_typeSizes;
    Ui::MainWindow *m_ui;
    Ps m_ps;
    Pmap m_mapper;
    QHexView *m_hex;
	QSplitter *m_splitter;
	QVBoxLayout *m_layout;
    std::shared_ptr<QDialog> m_dialog;
};
