#include "disconnectdialog.h"
#include "ui_disconnectdialog.h"

DisconnectDialog::DisconnectDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DisconnectDialog)
{
    ui->setupUi(this);
}

DisconnectDialog::~DisconnectDialog()
{
    delete ui;
}
