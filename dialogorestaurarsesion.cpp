#include "dialogorestaurarsesion.h"
#include "ui_dialogorestaurarsesion.h"
#include <QPushButton>

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

    //added: para cambiar el texto de los botones:
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Restaurar"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancelar"));
}

DialogoRestaurarSesion::~DialogoRestaurarSesion()
{
    delete ui;
}

void DialogoRestaurarSesion::setDiferencias(const QString &texto)
{
    ui->plainTextEdit->setPlainText(texto);
}
