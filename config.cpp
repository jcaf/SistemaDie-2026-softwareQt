#include "config.h"
#include "ui_config.h"
#include <QMessageBox>
#include <QFile>
#include <QString>
#include "mainwindow.h"

bool Config::readFile(void)
{
    bool codret=false;

    int encodernpulses = 0;
    float longitud_arco= 0;

    QString filename="config.txt";
    QFile file(filename);


    if(!file.exists())
    {
        qDebug() << "NO existe el archivo "<<filename;
    }else{
        qDebug() << "Archivo "<<filename<<" encontrado...";
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "error", file.errorString());
    }
    else
    {
        QTextStream in(&file);

        QString encoderPPR = in.readLine();
        qDebug()<< "encoderPPR" << encoderPPR;
        encodernpulses = encoderPPR.toInt();
        ui->encoder_PPR->setValue(encodernpulses);
        //
        QString LongitudArco = in.readLine();
        longitud_arco = LongitudArco.toFloat();
        ui->longitudArcoPorResolucion->setValue(longitud_arco);
        qDebug()<< "LongitudArco" << LongitudArco;

        codret = true;
    }


    file.close();
    return codret;
}

Config::Config(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Config)
{
    ui->setupUi(this);

    readFile();
}

Config::~Config()
{
    delete ui;
}

void Config::on_buttonBox_accepted()
{
    QString filename="config.txt";
    QFile file(filename);

    int encodernpulses = ui->encoder_PPR->value();
    QString encoder_line= QString::number(encodernpulses);

    float longitud_arco= ui->longitudArcoPorResolucion->value();
    QString longitudarco_line = QString::number(longitud_arco);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << encoder_line <<Qt::endl;
        out<<longitudarco_line;

        file.close();
    }
    //
    emit update(USB_DATACODE_SET_ENCODER_PPR, encodernpulses);
    emit update_float(USB_DATACODE_SET_LONGITUD_ARCO, longitud_arco);

}

