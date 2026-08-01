#ifndef DIALOGORESTAURARSESION_H
#define DIALOGORESTAURARSESION_H

#include <QDialog>

namespace Ui {
class DialogoRestaurarSesion;
}

class DialogoRestaurarSesion : public QDialog
{
    Q_OBJECT

public:
    explicit DialogoRestaurarSesion(QWidget *parent = nullptr);
    ~DialogoRestaurarSesion();

    void setDiferencias(const QString &texto);
private:
    Ui::DialogoRestaurarSesion *ui;
};

#endif // DIALOGORESTAURARSESION_H
