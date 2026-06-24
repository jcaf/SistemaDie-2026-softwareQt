#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>
#include <QFile>
#include <QString>

namespace Ui {
class Config;
}

class Config : public QDialog
{
    Q_OBJECT

public:
    explicit Config(QWidget *parent = nullptr);
    ~Config();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Config *ui;
    bool readFile(void);

signals:
    void update(char datacode, int payload0);

    void update_float(char datacode, float payload0);

};

#endif // CONFIG_H
