#ifndef DIRPROCESS_HPP
#define DIRPROCESS_HPP

#include <QDialog>

namespace Ui {
class DirProcess;
}

class DirProcess : public QDialog
{
    Q_OBJECT

public:
    explicit DirProcess(QWidget *parent = nullptr);
    ~DirProcess();

private:
    Ui::DirProcess *ui;
};

#endif // DIRPROCESS_HPP
