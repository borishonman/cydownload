#include "dialogrepo.h"
#include "ui_dialogrepo.h"

dialogrepo::dialogrepo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogrepo)
{
    ui->setupUi(this);
}

dialogrepo::~dialogrepo()
{
    delete ui;
}

QString dialogrepo::getText()
{
    return ui->lineEdit->text();
}
