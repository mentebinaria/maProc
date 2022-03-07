#pragma once

#include <QDialog>

#include "include/ps.hpp"
#include "include/mainwindow.hpp"
#include <unordered_map>

namespace Ui
{
    class DirWindow;
}

class DirWindow : public QDialog
{
    Q_OBJECT

private:
    std::unordered_map<std::string, std::string> m_umap;
    Ui::DirWindow *m_ui;
    pid_t m_pid;
    Ps m_ps;

    void Conf_pidTable(void);
    void setPid(QString );
    void Set_pidTable(void);

public:
    explicit DirWindow(QWidget * = nullptr);
    virtual ~DirWindow();

    pid_t getPid();

private slots:
    void on_search_textEdited(const QString &);
    void on_pidTable_doubleClicked(const QModelIndex &);
};
