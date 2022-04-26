#pragma once

#include <QMainWindow>
#include <QDialog>
#include <unordered_map>
#include <QTableWidgetItem>
#include <vector>
#include <memory>
#include <QVBoxLayout>
#include <QWidget>

#include "src/include/qhexview.hpp"
#include "src/include/ps.hpp"
#include "src/include/pmap.hpp"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

 public:
  explicit MainWindow ( QWidget * = nullptr );
  ~MainWindow();

  void mainMapper();
  pid_t m_pid;

 private slots:
  void verify_pid();
  void mapper_find ( off_t, off_t, std::string,
                     uint8_t, std::vector<off_t> & );

  off_t valid_address_edit();
  void column_clean_all();
  void conf_button_all();
  void column_config_all();
  void set_values_column_utils();
  void set_values_column_address ( std::vector<off_t> &, std::string, std::string );
  void view_address_table ( QTableWidgetItem * );
  void set_values_column_heap();
  void set_values_column_stack();
  void set_types_edit_read();

  void on_search_address_textEdited ( const QString & );
  void on_pidButton_triggered();
  void on_searchButton_clicked();
  void on_closeButton_triggered();
  void on_newButton_triggered();
  void on_cleanButton_triggered();
  void on_editButton_clicked();
  void on_aboutButton_triggered();
  void on_gotooffsetButton_triggered();
  void on_rpidButton_triggered();
  void on_killButton_triggered();
  void on_stopButton_triggered();
  void on_cleanButtonLog_triggered();
  void on_quickHelpButton_triggered();
  void on_FullScreenButton_triggered();
  void on_SaveLogButton_triggered();

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
  QVBoxLayout *m_layout;
  std::shared_ptr<QDialog> m_dialog;
};
