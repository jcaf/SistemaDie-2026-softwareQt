#ifndef PARAMETROS_H
#define PARAMETROS_H

#include <QDialog>

namespace Ui {
class Parametros;
}

class Parametros : public QDialog
{
    Q_OBJECT

public:
    explicit Parametros(QWidget *parent = nullptr);
    ~Parametros();

    void setMedidas(double nc, double nl);
    double getMedidaNC() const;
    double getMedidaNL() const;
private:
    Ui::Parametros *ui;
};

#endif // PARAMETROS_H
