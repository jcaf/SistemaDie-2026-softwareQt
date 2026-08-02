#include "dialogorestaurarsesion.h"
#include "ui_dialogorestaurarsesion.h"

DialogoRestaurarSesion::DialogoRestaurarSesion(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogoRestaurarSesion)
{
    ui->setupUi(this);

    connect(ui->buttonBox,
            &QDialogButtonBox::accepted,
            this,
            &QDialog::accept);

    connect(ui->buttonBox,
            &QDialogButtonBox::rejected,
            this,
            &QDialog::reject);
}

DialogoRestaurarSesion::~DialogoRestaurarSesion()
{
    delete ui;
}

void DialogoRestaurarSesion::setDiferencias(
    const QString &texto)
{
    ui->plainTextEdit->setPlainText(texto);
}
