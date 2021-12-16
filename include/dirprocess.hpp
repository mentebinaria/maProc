#ifndef DIRPROCESS_HPP
#define DIRPROCESS_HPP

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>

namespace Ui {
    class DirProcess;
}

class DirProcess : public QDialog
{
    Q_OBJECT

public:
    explicit DirProcess(QWidget *parent = nullptr);
    ~DirProcess();

    void config_Qfile();
    void config_TreeDir();

private:
    Ui::DirProcess *ui;
    QFileSystemModel *dirmodel;

};

#endif // DIRPROCESS_HPP
