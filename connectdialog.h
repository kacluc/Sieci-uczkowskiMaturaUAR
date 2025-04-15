#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = nullptr);
    ~ConnectDialog();

    bool get_connection_type() {return server_chosen; }

    QString get_IP();

    int get_Port();

private slots:
    void on_radioButtonServer_clicked();

    void on_radioButtonClient_clicked();

private:

    void enable_controls();

    Ui::ConnectDialog *ui;
    bool server_chosen = false;
};

#endif // CONNECTDIALOG_H
