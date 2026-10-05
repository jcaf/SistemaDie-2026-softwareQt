#include "parametros.h"
#include "ui_parametros.h"

Parametros::Parametros(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Parametros)
{
    ui->setupUi(this);
}

Parametros::~Parametros()
{
    delete ui;
}
void Parametros::setMedidas(double nc, double nl)
{
    ui->medidaNC->setValue(nc);
    ui->medidaNL->setValue(nl);
}
double Parametros::getMedidaNC() const
{
    return ui->medidaNC->value();
}
double Parametros::getMedidaNL() const
{
    return ui->medidaNL->value();
}
