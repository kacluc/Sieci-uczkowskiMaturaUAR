#include "arxdialog.h"
#include "ui_arxdialog.h"

ArxDialog::ArxDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ArxDialog)
{
    ui->setupUi(this);
}

ArxDialog::~ArxDialog()
{
    delete ui;
}
