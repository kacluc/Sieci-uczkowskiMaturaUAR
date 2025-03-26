#ifndef ARXDIALOG_H
#define ARXDIALOG_H

#include <QDialog>

namespace Ui {
class ArxDialog;
}

class ArxDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArxDialog(QWidget *parent = nullptr);
    ~ArxDialog();

private:
    Ui::ArxDialog *ui;
};

#endif // ARXDIALOG_H
