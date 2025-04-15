#include "connectdialog.h"
#include "ui_connectdialog.h"

ConnectDialog::ConnectDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

void ConnectDialog::on_radioButtonServer_clicked()
{
    this->server_chosen = true;
    enable_controls();
}


void ConnectDialog::on_radioButtonClient_clicked()
{
    this->server_chosen = false;
    enable_controls();
}

void ConnectDialog::enable_controls()
{
    ui->IP1->setDisabled(server_chosen);
    ui->IP2->setDisabled(server_chosen);
    ui->IP3->setDisabled(server_chosen);
    ui->IP4->setDisabled(server_chosen);
}

QString ConnectDialog::get_IP()
{
    return ui->IP1->text() + "." + ui->IP2->text() + "." + ui->IP3->text() + "." + ui->IP4->text();
}

int ConnectDialog::get_Port()
{
    return this->ui->Port->text().toInt();
}

