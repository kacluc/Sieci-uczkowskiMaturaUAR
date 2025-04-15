#ifndef DISCONNECTDIALOG_H
#define DISCONNECTDIALOG_H

#include <QDialog>

namespace Ui {
class DisconnectDialog;
}

class DisconnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DisconnectDialog(QWidget *parent = nullptr);
    ~DisconnectDialog();

private:
    Ui::DisconnectDialog *ui;
};

#endif // DISCONNECTDIALOG_H
