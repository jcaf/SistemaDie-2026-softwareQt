#include "config.h"
#include "ui_config.h"
#include <QMessageBox>
#include <QFile>
#include <QString>
#include "mainwindow.h"

void Config::setConfiguracion(const ConfiguracionSistema &cfg)
{
    ui->encoder_PPR->setValue(cfg.encoderPPR);

    ui->longitudArcoPorResolucion->setValue(cfg.longitudArco);
}

ConfiguracionSistema Config::configuracion() const
{
    ConfiguracionSistema cfg;

    cfg.encoderPPR = ui->encoder_PPR->value();

    cfg.longitudArco = ui->longitudArcoPorResolucion->value();

    return cfg;
}

Config::Config(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Config)
{
    ui->setupUi(this);

}

Config::~Config()
{
    delete ui;
}

//Incluso podríamos eliminar estos signals más adelante, pero por ahora los podemos mantener para no cambiar demasiadas cosas a la vez.
void Config::on_buttonBox_accepted()
{

}

