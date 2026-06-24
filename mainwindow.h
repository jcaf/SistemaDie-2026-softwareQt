#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/qserialportinfo.h>
#include <QDebug>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStringList>
#include "ledindicator.h"
#include "config.h"
#include <QTimer>

//////////////////////////////////////////////////////////
#define USB_DATACODE_TOKEN_BEGIN '@'
#define USB_DATACODE_TOKEN_END '\r'

#define USB_DATACODE_SET_RECORRIDO_TOTAL 'A'
#define USB_DATACODE_SET_INTERVALO 'B'

#define USB_DATACODE_SET_SELECTOR 'C'
#define SELECTOR_SP 0
#define SELECTOR_NC 1
#define SELECTOR_NL 2
#define SELECTOR_L  3

#define USB_DATACODE_SET_EXECUTION 'D'
#define INICIO 0
#define PAUSA 1
#define PARAR 2
#define RESET 3

#define USB_DATACODE_SET_MOTOR 'E'
#define MOTOR_ON 1
#define MOTOR_OFF 0

#define USB_DATACODE_SET_LED_ENLACE 'F'
#define LED_ENLACE_ON 1
#define LED_ENLACE_OFF 0


#define USB_DATACODE_SET_ENCODER_PPR 'G'
#define USB_DATACODE_SET_LONGITUD_ARCO 'H'
//
#define USB_DATACODE_GET_SELECTOR 'I'
#define USB_DATACODE_GET_EXECUTION 'J'
#define USB_DATACODE_GET_MOTOR 'K'
#define USB_DATACODE_GET_LED_ENLACE 'L'
//
#define USB_DATACODE_SET_RECORRIDO_ACTUAL 'M'
//
#define USB_DATACODE_INTERVALO_COMPLETO 'N'

#define USB_DATACODE_CONTROL_ACTIVATED 'O'

#define USB_DATACODE_RESET_BOARD 'P'

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTableWidget *tableWidget;


    bool usbport(void);

    QSerialPort *usbCDC;

    void USB_send_data_float(char datacode, float payload0);//al enviar

    Config *objconfig;

    QTimer *timer;
    bool usbport_status;
    bool usbport_status_last;
private slots:
    void readSerial();

    void on_actionConstantes_triggered();


    void on_pushButton_Aceptar_clicked();

    void USB_send_data_integer(char datacode, int payload0);//al enviar

    void on_radioButton_SP_clicked();

    void on_radioButton_NC_clicked();

    void on_radioButton_NL_clicked();

    void on_radioButton_L_clicked();

    void on_pushButton_Parar_clicked();

    void on_pushButton_Inicio_clicked();

    void on_pushButton_Pause_clicked();

    void on_pushButton_Reset_clicked();

    void on_pushButton_Motor_clicked();

    void on_pushButton_Motor_toggled(bool checked);

    void tabla_update_cell_posicion(int tabla_numfila_actual);


    void on_pushButton_Exportar_clicked();

    bool usbport_available();

    void onCellChanged(int row, int column);
private:
    Ui::MainWindow *ui;

    /*
     * 1a86:7523 QinHeng Electronics CH340 serial converter
     */
    static const quint16 usbCDC_vendor_id = 6790;   //
    static const quint16 usbCDC_product_id = 29987; //
    QString usbCDC_port_name;
    bool usbCDC_is_available;
    void USB_commands(char USB_DATACODE, char *USB_payload_char );//al recibir

    void buttons_disable();
    void buttons_enable();

    void tableWidget_columna_setEnabled(int col, bool state);
    void tableWidget_enable_for_SP();
    void tableWidget_enable_for_NC();
    void tableWidget_enable_for_NL();
    void tableWidget_enable_for_L();

    LedIndicator *led_motor;
    LedIndicator *led_enlace;
    int tabla_numfila;
    int TABLA_NUM_FILAS_TOTALES;
    float RECORRIDOTOTAL;
};
#endif // MAINWINDOW_H
