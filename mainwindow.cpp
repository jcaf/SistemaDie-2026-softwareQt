/* 23/06/2026 - cambios nuevos
 * 17/09/2025
 * C:\Qt\6.9.2\mingw_64\bin>windeployqt.exe Y:\Documents\PROYECTOS\ATMEL\C\Wilfredo\SistemaDiE-2025\software\SistemaDiE\build\Desktop_Qt_6_9_2_MinGW_64_bit-Release\output\SistemaDiE.exe
 *
 * C:\Qt\6.8.1\mingw_64\bin>windeployqt.exe C:\Users\NEFER\OneDrive\Documentos\untitled\build\Desktop_Qt_6_8_1_MinGW_64_bit-Release\output\untitled.exe
 * Qt Creator 15.0.0
 *
 * en la carpeta
 * C:\Users\NEFER\OneDrive\Documentos\untitled\build\Desktop_Qt_6_8_1_MinGW_64_bit-Release\output
 * se encuentra todos los archivos necesarios para la ejecucion independiente. Solo hay que crear o copiar el archivo config.txt
 * en esa carpeta de salida
 *
 * JCaf.
    Based on Qt 6.8.1 (MSVC 2022, x86_64)
    Built on Nov 27 2024 08:38:21

 *  serial port Terminal: realTerm
 *
 *En QT6, hay que agregar manualmente las siguientes lineas en su lugar correspondiente en el archivo CMakeLists.txt
 *En QT6, se ha migrado todo a CMake
 *
 *find_package(Qt6 REQUIRED COMPONENTS SerialPort)
 *target_link_libraries(untitled PRIVATE Qt6::SerialPort)
 */
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "config.h"
#include <QThread>
#include <QDateTime>
#include <QFileDialog>
#include <QString>
#include <OpenXLSX.hpp>
using namespace OpenXLSX;

#define TABLA_NUMBER_COLUMNAS 7//9 se elimina Vl y Rl

#define TABLA_NUMBER_FILAS_ADICIONALES 5//filas adicionales a las que se crean

void MainWindow::buttons_disable()
{
    // ui->radioButton_SP->setEnabled(false);
    // ui->radioButton_NC->setEnabled(false);
    // ui->radioButton_NL->setEnabled(false);
    // ui->radioButton_L->setEnabled(false);

    ui->pushButton_Reset->setEnabled(false);
    ui->pushButton_Motor->setEnabled(false);

    ui->pushButton_Inicio->setEnabled(false);
    ui->pushButton_Pause->setEnabled(false);
    ui->pushButton_Parar->setEnabled(false);
    ui->pushButton_Aceptar->setEnabled(false);

    ui->menuConfig->setEnabled(false);
    led_motor->setEnabled(false);
}
void MainWindow::buttons_enable()
{
    // ui->radioButton_SP->setEnabled(true);
    // ui->radioButton_NC->setEnabled(true);
    // ui->radioButton_NL->setEnabled(true);
    // ui->radioButton_L->setEnabled(true);

    ui->pushButton_Reset->setEnabled(true);
    ui->pushButton_Motor->setEnabled(true);

    ui->pushButton_Inicio->setEnabled(false);
    ui->pushButton_Pause->setEnabled(false);
    ui->pushButton_Parar->setEnabled(true);
    ui->pushButton_Aceptar->setEnabled(true);

    ui->menuConfig->setEnabled(true);
    led_motor->setEnabled(true);

}


bool MainWindow::usbport_available(void)
{
    usbCDC_is_available = false;
    usbCDC_port_name = "";
    //usbCDC = new QSerialPort;

    //+- Solo para depurar
    //qDebug()<< "number of available ports "<< QSerialPortInfo::availablePorts().length();
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        //qDebug()<< "Has vendor ID" << serialPortInfo.hasVendorIdentifier();
        if (serialPortInfo.hasVendorIdentifier()){
          //  qDebug()<< "Vendor id"<< serialPortInfo.vendorIdentifier();
        }

        //qDebug()<< "Has product ID" << serialPortInfo.hasProductIdentifier();
        if (serialPortInfo.hasProductIdentifier())
        {
          //  qDebug()<< "Product ID" << serialPortInfo.productIdentifier();
        }
    }
    //+-

    //changed 8/set/2025
    //

    bool usbport_status_temp = false;

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if (serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier())
        {
            if ( (serialPortInfo.productIdentifier() == usbCDC_product_id) && (serialPortInfo.vendorIdentifier() == usbCDC_vendor_id) )
            {
                usbCDC_is_available = true;
                usbCDC_port_name  =serialPortInfo.portName();

                // qDebug()<< usbCDC_port_name;
                // qDebug()<<"puerto encontrado";

                //usbport_status = true;
                usbport_status_temp = true;
            }
            else
            {
                //usbport_status = false;
                //usbCDC_port_name = "";

                //qDebug()<<"puerto NO encontrado";
            }
        }
    }
    usbport_status = usbport_status_temp;

    if (usbport_status_last != usbport_status )
    {
        usbport_status_last = usbport_status;

        if (usbport_status == true)
        {
            usbCDC = new QSerialPort;

            usbCDC->setPortName(usbCDC_port_name);
            //usbCDC->open(QSerialPort::WriteOnly);
            usbCDC->open(QSerialPort::ReadWrite);
            //usbCDC->open(QSerialPort::ReadOnly);//ok x lecturas
            //usbCDC->setBaudRate(QSerialPort::Baud38400);
            //usbCDC->setBaudRate(QSerialPort::Baud115200);
            //usbCDC->setBaudRate(230400);
            usbCDC->setBaudRate(250000);
            //usbCDC->setBaudRate(9600);

            usbCDC->setDataBits(QSerialPort::Data8);
            usbCDC->setParity(QSerialPort::NoParity);
            usbCDC->setStopBits(QSerialPort::OneStop);
            usbCDC->setFlowControl(QSerialPort::NoFlowControl);
            //

            QObject::connect(usbCDC, SIGNAL(readyRead()), this, SLOT(readSerial()));

            //ui->status->setText("Conexión OK: Tarjeta de control encontrado");
            qDebug() << "Conexión OK: Tarjeta de control encontrado";
            //
            led_enlace->setState(true);

            USB_send_data_integer(USB_DATACODE_SET_SELECTOR, SELECTOR_SP);
            tableWidget_enable_for_SP();
            //

            buttons_enable();
        }
        else
        {
            // if (usbCDC->isOpen())
            // {
            //     usbCDC->close();
            // }
            /*
             * En usbport_available() creas un nuevo QSerialPort cada vez que detectas conexión:
             * De lo contrario puedes dejar un descriptor abierto en el SO o incluso un dangling pointer si se intenta acceder después.
             */

            if (usbCDC)
            {
                if (usbCDC->isOpen())
                {
                    usbCDC->close();
                }
                delete(usbCDC);
                usbCDC = nullptr;
            }

            //delete(usbCDC);//bug fixed 1

            //QMessageBox::warning(this, "Port error", "Tarjeta de control no encontrado");
            //ui->status->setText("Port error, Tarjeta de control no encontrado");
            qDebug() << "Port error, Tarjeta de control no encontrado";

            led_enlace->setState(false);

            buttons_disable();
        }
    }

    return usbCDC_is_available;
}

/*
bool MainWindow::usbport(void)
{
    usbCDC_is_available = false;
    usbCDC_port_name = "";
    usbCDC = new QSerialPort;

    //+- Solo para depurar
    qDebug()<< "number of available ports "<< QSerialPortInfo::availablePorts().length();
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        qDebug()<< "Has vendor ID" << serialPortInfo.hasVendorIdentifier();
        if (serialPortInfo.hasVendorIdentifier()){
            qDebug()<< "Vendor id"<< serialPortInfo.vendorIdentifier();
        }

        qDebug()<< "Has product ID" << serialPortInfo.hasProductIdentifier();
        if (serialPortInfo.hasProductIdentifier())
        {
            qDebug()<< "Product ID" << serialPortInfo.productIdentifier();
        }
    }
    //+-
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if (serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier())
        {
            if ( (serialPortInfo.productIdentifier() == usbCDC_product_id) && (serialPortInfo.vendorIdentifier() == usbCDC_vendor_id) )
            {
                usbCDC_is_available = true;
                usbCDC_port_name  =serialPortInfo.portName();
                qDebug()<< usbCDC_port_name;
            }
        }
    }
    //+-
    if (usbCDC_is_available)
    {
        usbCDC->setPortName(usbCDC_port_name);
        //usbCDC->open(QSerialPort::WriteOnly);
        usbCDC->open(QSerialPort::ReadWrite);
        //usbCDC->open(QSerialPort::ReadOnly);//ok x lecturas
        //usbCDC->setBaudRate(QSerialPort::Baud38400);
        //usbCDC->setBaudRate(QSerialPort::Baud115200);
        //usbCDC->setBaudRate(230400);
        usbCDC->setBaudRate(250000);
        //usbCDC->setBaudRate(9600);

        usbCDC->setDataBits(QSerialPort::Data8);
        usbCDC->setParity(QSerialPort::NoParity);
        usbCDC->setStopBits(QSerialPort::OneStop);
        usbCDC->setFlowControl(QSerialPort::NoFlowControl);
        //
        QObject::connect(usbCDC, SIGNAL(readyRead()), this, SLOT(readSerial()));

        //ui->status->setText("Conexión OK: Tarjeta de control encontrado");
        qDebug() << "Conexión OK: Tarjeta de control encontrado";
        return true;
    }
    else
    {
        QMessageBox::warning(this, "Port error", "Tarjeta de control no encontrado");
        //ui->status->setText("Port error, Tarjeta de control no encontrado");
        qDebug() << "Port error, Tarjeta de control no encontrado";
        return false;
    }
}
*/
void MainWindow::tabla_update_cell_posicion(int tabla_numfila_actual)
{
    QString str_recorrido_actual = QString::number(ui->recorridoActual->value(),'f',2);
    QTableWidgetItem* text = new QTableWidgetItem();
    text->setText(str_recorrido_actual);
    tableWidget->setItem(tabla_numfila_actual,0, text);
    tableWidget_columna_setEnabled(0,false);
}
void MainWindow::USB_commands(char USB_DATACODE, char *USB_payload_char )
{
    float payload_f;//x floats
    int payload_i;//x integers

    switch (USB_DATACODE)
    {
        case USB_DATACODE_SET_RECORRIDO_TOTAL:
            payload_f = atof(USB_payload_char);
            qDebug()<<"USB_DATACODE_SET_RECORRIDO_TOTAL: "<< payload_f<< Qt::endl;
        break;
        case USB_DATACODE_SET_INTERVALO:
            payload_f = atof(USB_payload_char);
            qDebug()<<"USB_DATACODE_SET_INTERVALO: "<< payload_f<< Qt::endl;
        break;
        case USB_DATACODE_SET_ENCODER_PPR:
            payload_i = atoi(USB_payload_char);
            qDebug()<<"USB_DATACODE_SET_ENCODER_PPR: "<< payload_i<< Qt::endl;
        break;
        case USB_DATACODE_SET_LONGITUD_ARCO:
            payload_f = atof(USB_payload_char);
            qDebug()<<"USB_DATACODE_SET_LONGITUD_ARCO: "<< payload_f<< Qt::endl;
        break;
        case USB_DATACODE_SET_RECORRIDO_ACTUAL:
            payload_f = atof(USB_payload_char);
            ui->recorridoActual->setValue(payload_f);
            qDebug()<<"USB_DATACODE_SET_RECORRIDO_ACTUAL: "<< payload_f<< Qt::endl;
        break;

        case USB_DATACODE_SET_EXECUTION://confirmacion desde el uC
            payload_i = atoi(USB_payload_char);
            if (payload_i == INICIO)
            {
                qDebug()<<"**USB_DATACODE_SET_EXECUTION: INICIO "<< payload_i<< Qt::endl;
            }
            else if (payload_i == PAUSA)
            {
                qDebug()<<"**USB_DATACODE_SET_EXECUTION: PAUSA "<< payload_i<< Qt::endl;

            }
            else if (payload_i == PARAR)
            {
                qDebug()<<"**USB_DATACODE_SET_EXECUTION: PARAR "<< payload_i<< Qt::endl;

            }
            else if (payload_i == RESET)
            {
                qDebug()<<"**USB_DATACODE_SET_EXECUTION: RESET "<< payload_i<< Qt::endl;
            }

        break;

        case USB_DATACODE_INTERVALO_COMPLETO:

            if (tabla_numfila < TABLA_NUM_FILAS_TOTALES)
            {
                tabla_update_cell_posicion(tabla_numfila);
                tabla_numfila++;
            }
            //
            if (tabla_numfila < TABLA_NUM_FILAS_TOTALES)
            {
                ui->pushButton_Inicio->setEnabled(true);
            }
            else
            {
                ui->pushButton_Inicio->setEnabled(false);
            }


            ui->pushButton_Pause->setEnabled(false);
            ui->pushButton_Parar->setEnabled(true);
            led_motor->setState(false);

            qDebug()<<"USB_DATACODE_INTERVALO_COMPLETO: "<<Qt::endl;

        break;
        case USB_DATACODE_SET_SELECTOR:
            payload_i = atoi(USB_payload_char);

            if (payload_i == SELECTOR_SP)
            {
                ui->radioButton_SP->setChecked(true);
            }
            else if (payload_i == SELECTOR_NC)
            {
                ui->radioButton_NC->setChecked(true);

            }
            else if (payload_i == SELECTOR_NL)
            {
                ui->radioButton_NL->setChecked(true);

            }
            else if (payload_i == SELECTOR_L)
            {
                ui->radioButton_L->setChecked(true);

            }

            break;

        case USB_DATACODE_RESET_BOARD:
            payload_i = atoi(USB_payload_char);
            qDebug()<<"+++++++++++++ RESET BOARD +++++++++++++++"<< payload_i<< Qt::endl;

            break;
        case USB_DATACODE_CONTROL_ACTIVATED:
            payload_i = atoi(USB_payload_char);
            qDebug()<<"USB_DATACODE_CONTROL_ACTIVATED"<< payload_i<< Qt::endl;

            break;

    default: break;
    }
}

static QString str_acc = "";
int yy;
/*
void MainWindow::readSerial()
{
    static char USB_DATACODE;
    static char USB_payload_char[300];
    static int8_t USB_payload_idx = 0;
    static int8_t sm0 = 0;
    char c;

    QByteArray serialBuff = usbCDC->readAll();
    QString str_payload = QString::fromStdString(serialBuff.toStdString());
    str_acc += str_payload;   //acumula el buffer recibido

    qDebug() << "str_acc:" << ++yy << " - " << str_acc << Qt::endl;

    // Procesar sobre str_acc completo
    for (int i = 0; i < str_acc.length(); i++)
    {
        c = str_acc[i].toLatin1(); // tomamos char ASCII

        if (sm0 == 0)
        {
            if (c == USB_DATACODE_TOKEN_BEGIN)
            {
                USB_payload_idx = 0;
                sm0++;
            }
        }
        else if (sm0 == 1)
        {
            if ((c >= USB_DATACODE_SET_RECORRIDO_TOTAL) && (c <= USB_DATACODE_RESET_BOARD))
            {
                USB_DATACODE = c;
                sm0++;
            }
            else
            {
                sm0 = 0;
            }
        }
        else if (sm0 == 2) // storage payload
        {
            if (c == USB_DATACODE_TOKEN_END)
            {
                USB_payload_char[USB_payload_idx] = '\0';

                //Ejecuta comando con trama completa
                USB_commands(USB_DATACODE, USB_payload_char);

                //Recorta el acumulador solo hasta lo procesado
                str_acc.remove(0, i + 1);
                i = -1; // reinicia el for desde el nuevo buffer
                sm0 = 0;
            }
            else
            {
                USB_payload_char[USB_payload_idx] = c;
                USB_payload_idx++;
            }
        }

        //Prevención de overflow en el buffer de payload
        if (USB_payload_idx >= sizeof(USB_payload_char) - 1)
        {
            sm0 = 0; // reset state machine
            USB_payload_idx = 0;
            qDebug() << "Overflow prevenido en USB_payload_char";
            continue;
        }
    }
}

*/
void MainWindow::readSerial()
{
    static char USB_DATACODE;
    static char USB_payload_char[300];
    static int8_t USB_payload_idx = 0;
    static int8_t sm0 = 0;
    char c;

    QByteArray serialBuff = usbCDC->readAll();
    QString str_payload = QString::fromStdString(serialBuff.toStdString());
//    str_acc += str_payload;         //va acumulando y formando el string
    str_acc = str_payload;         //va acumulando y formando el string

    qDebug()<< "str_acc:"<<++yy<<" - "<<str_acc<<Qt::endl;


    std::string Cstr = str_acc.toStdString();

    int length = Cstr.length();

    //sm0 = 0;
    for (int i=0; i< length; i++)
    {
        c =  Cstr[i];
        if (sm0 == 0)
        {
            if ( c == USB_DATACODE_TOKEN_BEGIN)
            {
                USB_payload_idx = 0;
                sm0++;
            }
        }
        else if (sm0 == 1)
        {
            if ((c >=USB_DATACODE_SET_RECORRIDO_TOTAL) && (c <=USB_DATACODE_RESET_BOARD))
            {
                USB_DATACODE = c;
                sm0++;
            }
            else
            {
                sm0 = 0;
            }

        }
        else if (sm0 == 2)//storage payload
        {
            if (c == USB_DATACODE_TOKEN_END)
            {
                USB_payload_char[USB_payload_idx] = '\0';
                //
                USB_commands(USB_DATACODE, USB_payload_char);

                //str_acc = "";
                sm0 = 0;
            }
            else
            {
                USB_payload_char[USB_payload_idx] = c;
                USB_payload_idx++;
            }
        }

        //Bug fixed 4
        //
        //En readSerial() llenas USB_payload_char[300] pero nunca chequeas si USB_payload_idx supera 299.
        //Esto puede generar un buffer overflow y comportamiento indefinido.


        if (USB_payload_idx >= sizeof(USB_payload_char) - 1)
        {
            sm0 = 0; // reset
            USB_payload_idx = 0;
            qDebug() << "Overflow prevenido en USB_payload_char";
            continue;
        }
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->radioButton_L->hide();

    TABLA_NUM_FILAS_TOTALES = 0;

    tableWidget = new QTableWidget(this);
    //tableWidget->setGeometry(390,145,330,600);
    //tableWidget->setGeometry(390,145,330,515);
    //tableWidget->setGeometry(390,130,990,515);
    tableWidget->setGeometry(380,130,750,520);

    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    tableWidget->setRowCount(0);
    tableWidget->setColumnCount(TABLA_NUMBER_COLUMNAS);



    QStringList headers;

    //headers << "POSICION" << "I\nmA" << "SP\nmV" << "Vnc\nmV" << "Vnl\nmV" << "Vl\nmV" << "Rnc\nΩ-m" << "Rnl\nΩ-m" << "Rl\nΩ-m";
    headers << "POSICION" << "I\nmA" << "SP\nmV" << "Vnc\nmV" << "Vnl\nmV" << "Rnc\nΩ-m" << "Rnl\nΩ-m";

    tableWidget->setHorizontalHeaderLabels(headers);


    led_motor = new LedIndicator();
    led_enlace = new LedIndicator();

    ui->gridLayoutLedMotor->addWidget(led_motor,1,0);
    ui->gridLayoutLedEnlace->addWidget(led_enlace,1,0);

    led_motor->setOnColor(QColorConstants::Cyan);
    led_enlace->setOnColor(QColorConstants::Green);

    ui->radioButton_SP->setChecked(true);

//x test01
    buttons_disable();
    //////////////////////////////////
//x test02
    usbport_status = false;
    usbport_status_last = usbport_status;
    ///
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, usbport_available );
    timer->start(500);
    ////////////////////////////////

    //new: 8/set/2025
    connect(tableWidget, &QTableWidget::cellChanged,
            this, &MainWindow::onCellChanged,
            Qt::UniqueConnection);

}

MainWindow::~MainWindow()
{
    delete ui;

    //Bug fixed 5
    /*Pero nunca liberas usbCDC, timer, tableWidget, ni los LedIndicator. Esto es una fuga de memoria
     * */
    if (usbCDC)
    {
        if (usbCDC->isOpen())
        {
            usbCDC->close();
        }
        delete usbCDC;
    }
    delete timer;
    delete tableWidget;
    delete led_motor;
    delete led_enlace;
}

void MainWindow::on_actionConstantes_triggered()
{
    // Config mDialog;
    // mDialog.setModal(true);
    // mDialog.exec();
    Config* objconfig = new Config;
    connect(objconfig, &Config::update, this, &MainWindow::USB_send_data_integer);
    connect(objconfig, &Config::update_float, this, &MainWindow::USB_send_data_float);

    objconfig->setModal(true);
    objconfig->exec();
}


//usar overloading la sgte. vez para tener 2 funciones diferentes enviando datos diferentes

void MainWindow::USB_send_data_float(char datacode, float payload0)
{
    if (usbport_status == true) //added 8/set/2025
    {

        char str[30];
        char buff[30];

        str[0] = USB_DATACODE_TOKEN_BEGIN;
        str[1] = datacode;
        str[2] = '\0';
        sprintf(buff,"%.2f",payload0);
        strcat(str,buff);
        strcat(str,"\r");
        //

        //Bug fixed 2
        /*
         * QByteArray data = QByteArray::fromRawData(str, strlen(str));
            Esto no copia el buffer, solo lo referencia. Si usbCDC->write(data) se ejecuta y str deja de existir, se puede enviar basura.
         */

        //QByteArray data = QByteArray::fromRawData(str, strlen(str)   );
        QByteArray data(str, strlen(str));

        usbCDC->write(data);

        qDebug()<<"USB_send_data_float"<< data<< Qt::endl;
    }
}

void MainWindow::USB_send_data_integer(char datacode, int payload0)
{
    if (usbport_status == true) //added 8/set/2025
    {

        char str[30];
        char buff[30];

        str[0] = USB_DATACODE_TOKEN_BEGIN;
        str[1] = datacode;
        str[2] = '\0';
        itoa(payload0, buff, 10);
        strcat(str,buff);
        strcat(str,"\r");

        //Bug fixed 2
        /*
         * QByteArray data = QByteArray::fromRawData(str, strlen(str));
            Esto no copia el buffer, solo lo referencia. Si usbCDC->write(data) se ejecuta y str deja de existir, se puede enviar basura.
         */

        //QByteArray data = QByteArray::fromRawData(str, strlen(str)   );
        QByteArray data(str, strlen(str));

        usbCDC->write(data);
        //
        qDebug()<<"USB_send_data_integer"<< data << Qt::endl;
    }
}

void MainWindow::tableWidget_columna_setEnabled(int col, bool state)
{
    for (int r=0; r < TABLA_NUM_FILAS_TOTALES; r++)
    {
        //tableWidget->item(r, 1)->setFlags(tableWidget->item(r, 1)->flags() & ~Qt::ItemIsEditable);

        QTableWidgetItem *dataItem = tableWidget->item(r, col);

        if (state == false)
        {
            dataItem->setFlags(dataItem->flags() & ~Qt::ItemIsEditable);

        }
        else
        {
            dataItem->setFlags(dataItem->flags() | Qt::ItemIsEditable);
        }
    }

}
/*
enum _HEADER_LABEL
{
    TABLEPOSC_POSICION  = 0,
    TABLEPOSC_I_ma,
    TABLEPOSC_SP_mV,
    TABLEPOSC_Vnc_mV,
    TABLEPOSC_Vnl_mV,
    TABLEPOSC_Vl_mV,
    TABLEPOSC_Rnc_ohm_m,
    TABLEPOSC_Rnl_ohm_m,
    TABLEPOSC_Rl_ohm_m,
};
*/
//cambiado ahora a solo 7 headers
enum _HEADER_LABEL
{
    TABLEPOSC_POSICION  = 0,
    TABLEPOSC_I_ma,
    TABLEPOSC_SP_mV,
    TABLEPOSC_Vnc_mV,
    TABLEPOSC_Vnl_mV,
    TABLEPOSC_Rnc_ohm_m,
    TABLEPOSC_Rnl_ohm_m,
};


void MainWindow::onCellChanged(int row, int column)
{

    if (ui->radioButton_SP->isChecked())
    {
    }
    else if (ui->radioButton_NC->isChecked())
    {
        // Solo recalculamos si cambio las columnas en cuestion
        if (column == TABLEPOSC_I_ma || column == TABLEPOSC_Vnc_mV)
        {
            QTableWidgetItem *itemA = tableWidget->item(row, TABLEPOSC_I_ma);
            QTableWidgetItem *itemB = tableWidget->item(row, TABLEPOSC_Vnc_mV);

            if (!itemA || !itemB) return;

            bool okA, okB;
            double a = itemA->text().toDouble(&okA);
            double b = itemB->text().toDouble(&okB);

            //Buf fixed 6
            /*
                 * Aunque en on_pushButton_Aceptar_clicked() inicializas todas las celdas, si por algún bug un QTableWidgetItem es nullptr, haces
                 *                 Esto crashea si es nullptr.
                    Mejor asegurar:
                 */
            auto *target = tableWidget->item(row, TABLEPOSC_Rnc_ohm_m);

            if (okA && okB && a!= 0.0 )
            {
                //=4*PI()*0.125*(E4/B4)
                double Rnc = 4 * M_PI * 0.125 * (b/a);

                //Buf fixed 6
                /*
                 * Aunque en on_pushButton_Aceptar_clicked() inicializas todas las celdas, si por algún bug un QTableWidgetItem es nullptr, haces
                 *                 Esto crashea si es nullptr.
                    Mejor asegurar:
                 */
                //tableWidget->item(row, TABLEPOSC_Rnc_ohm_m)->setText(QString::number(Rnc));

                //auto *target = tableWidget->item(row, TABLEPOSC_Rnc_ohm_m);
                if (target)
                {
                    target->setText(QString::number(Rnc));
                }

            }
            else
            {
                // Si no son números, reseteamos
                //tableWidget->item(row, TABLEPOSC_Rnc_ohm_m)->setText("0");
                if (target)
                {
                    target->setText("0");
                }
            }
        }
    }
    else if (ui->radioButton_NL->isChecked())
    {
        // Solo recalculamos si cambio las columnas en cuestion
        if (column == TABLEPOSC_I_ma || column == TABLEPOSC_Vnl_mV)
        {
            QTableWidgetItem *itemA = tableWidget->item(row, TABLEPOSC_I_ma);
            QTableWidgetItem *itemB = tableWidget->item(row, TABLEPOSC_Vnl_mV);

            if (!itemA || !itemB) return;

            bool okA, okB;
            double a = itemA->text().toDouble(&okA);
            double b = itemB->text().toDouble(&okB);

            //Buf fixed 6
            /*
                 * Aunque en on_pushButton_Aceptar_clicked() inicializas todas las celdas, si por algún bug un QTableWidgetItem es nullptr, haces
                 *                 Esto crashea si es nullptr.
                    Mejor asegurar:
                 */
            auto *target = tableWidget->item(row, TABLEPOSC_Rnl_ohm_m);

            if (okA && okB && a!= 0.0)
            {
                //=4*PI()*0.765*(E4/B4) //corregido a 0.63
                double Rnl = 4 * M_PI * 0.63 * (b/a);
                //tableWidget->item(row, TABLEPOSC_Rnl_ohm_m)->setText(QString::number(Rnl));
                if (target)
                {
                    target->setText(QString::number(Rnl));
                }
            }
            else
            {
                // Si no son números, reseteamos
                //tableWidget->item(row, TABLEPOSC_Rnl_ohm_m)->setText("0");
                if (target)
                {
                    target->setText("0");
                }
            }
        }
    }
    //L es retirado
    /*else if (ui->radioButton_L->isChecked())
    {
        // Solo recalculamos si cambio las columnas en cuestion
        if (column == TABLEPOSC_I_ma || column == TABLEPOSC_Vl_mV)
        {
            QTableWidgetItem *itemA = tableWidget->item(row, TABLEPOSC_I_ma);
            QTableWidgetItem *itemB = tableWidget->item(row, TABLEPOSC_Vl_mV);

            if (!itemA || !itemB) return;

            bool okA, okB;
            double a = itemA->text().toDouble(&okA);
            double b = itemB->text().toDouble(&okB);

            if (okA && okB && a!= 0.0)
            {
                //=4*PI()*( (2.39*0.125) /0.64)* (F4/B4)
                double Rl = 4 * M_PI * ( (2.39*0.125) /0.64)  * (b/a);
                tableWidget->item(row, TABLEPOSC_Rl_ohm_m)->setText(QString::number(Rl));
            }
            else
            {
                // Si no son números, reseteamos
                tableWidget->item(row, TABLEPOSC_Rl_ohm_m)->setText("0");
            }
        }
    }*/


///////////////////////////////////////////////////////////////////////////////////////

}

void MainWindow::on_pushButton_Aceptar_clicked()
{
    //float rt = ui->recorridoTotal->value();
    RECORRIDOTOTAL = ui->recorridoTotal->value();
    float itv = ui->intervalo->value();
    //TABLA_NUM_FILAS_TOTALES =(int) (rt /itv) + TABLA_NUMBER_FILAS_ADICIONALES;
    TABLA_NUM_FILAS_TOTALES =(int) (RECORRIDOTOTAL /itv) + TABLA_NUMBER_FILAS_ADICIONALES;

    tableWidget->blockSignals(true);  //desactivar señales
    tableWidget->setRowCount(TABLA_NUM_FILAS_TOTALES);
    tableWidget->setColumnCount(TABLA_NUMBER_COLUMNAS);

    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    //excusa para poder tener un puntero despues con tableWidget->item(r,0)
    //En realidad, para QTableWidget es necesario inicicializar todos los items como "texto vacio" para que cuando se exporte
    //al leer el valor de celdas vacias, no de error. Es como si cada celda necesita previamente tener un valor aun asi no se use
    for (int r=0; r < TABLA_NUM_FILAS_TOTALES; r++)
    {
        for (int col=0; col<TABLA_NUMBER_COLUMNAS; col++)
        {
            QTableWidgetItem* text = new QTableWidgetItem();
            text->setText("");
            tableWidget->setItem(r,col,text);
        }
    }

    tableWidget->blockSignals(false);
    //Posicion disable
    tableWidget_columna_setEnabled(0,false);
    // for (int r=0; r < TABLA_NUM_FILAS_TOTALES; r++)
    // {
    //     //tableWidget->item(r, 1)->setFlags(tableWidget->item(r, 1)->flags() & ~Qt::ItemIsEditable);
    //     QTableWidgetItem *dataItem = tableWidget->item(r, 0);
    //     dataItem->setFlags(dataItem->flags() & ~Qt::ItemIsEditable);
    // }
    //deshabilitar los controles
    ui->recorridoTotal->setEnabled(false);
    ui->intervalo->setEnabled(false);
    ui->pushButton_Reset->setEnabled(false);
    ui->pushButton_Motor->setEnabled(false);
    //
    // ui->radioButton_SP->setEnabled(false);
    // ui->radioButton_NC->setEnabled(false);
    // ui->radioButton_NL->setEnabled(false);
    // ui->radioButton_L->setEnabled(false);
    //
    ui->pushButton_Aceptar->setEnabled(false);
    ui->pushButton_Inicio->setEnabled(true);
    ui->pushButton_Parar->setEnabled(true);
    //
    ui->pushButton_Reset->setEnabled(false);
    ui->pushButton_Motor->setChecked(false);

    if (ui->pushButton_Motor->isChecked())
    {
        ui->pushButton_Motor->setChecked(false);
        led_motor->setState(false);
    }
    tabla_numfila = 0; //reset

//xDebug3

    //qDebug()<<"enviar datos al microcontrolador..."<<Qt::endl;
    USB_send_data_float(USB_DATACODE_SET_RECORRIDO_TOTAL, RECORRIDOTOTAL );//rt);
    //QThread::msleep(10);
    USB_send_data_float(USB_DATACODE_SET_INTERVALO, itv);
    //QThread::msleep(10);
    USB_send_data_integer(USB_DATACODE_CONTROL_ACTIVATED,0);
    //qDebug()<<"fin enviar datos al microcontrolador..."<<Qt::endl;

    //ADD:
    if (ui->radioButton_SP->isChecked())
    {
        tableWidget_enable_for_SP();
    }
    else if (ui->radioButton_NC->isChecked())
    {
        tableWidget_enable_for_NC();
    }
    else if (ui->radioButton_NL->isChecked())
    {
        tableWidget_enable_for_NL();
    }
    else if (ui->radioButton_L->isChecked())
    {
        tableWidget_enable_for_L();
    }

    //ahora pasa al constructor
    /*
    //new: 8/set/2025
    connect(tableWidget, &QTableWidget::cellChanged,
            this, &MainWindow::onCellChanged,
               Qt::UniqueConnection);
*/

    //Bug fixed 7:Posible doble conexión de señales
    /*
     * Bien hecho , pero ojo: en on_pushButton_Aceptar_clicked() tienes comentada otra conexión similar. Si la descomentas y olvidas el UniqueConnection, tendrás ejecuciones dobles.
     * */
}

void MainWindow::tableWidget_enable_for_SP()
{
    tableWidget_columna_setEnabled(0,false);
    tableWidget_columna_setEnabled(1,true);
    tableWidget_columna_setEnabled(2,true);
    tableWidget_columna_setEnabled(3,false);
    tableWidget_columna_setEnabled(4,false);

    tableWidget_columna_setEnabled(5,false);
    tableWidget_columna_setEnabled(6,false);
    //tableWidget_columna_setEnabled(7,false);
    //tableWidget_columna_setEnabled(8,false);
}
void MainWindow::tableWidget_enable_for_NC()
{
    //add:
    tableWidget_columna_setEnabled(0,false);
    tableWidget_columna_setEnabled(1,true);
    tableWidget_columna_setEnabled(2,false);
    tableWidget_columna_setEnabled(3,true);
    tableWidget_columna_setEnabled(4,false);

    tableWidget_columna_setEnabled(5,false);
    tableWidget_columna_setEnabled(6,false);
    //tableWidget_columna_setEnabled(7,false);
    //tableWidget_columna_setEnabled(8,false);

    //=4*PI()*0.125*(D4/B4)

}
void MainWindow::tableWidget_enable_for_NL()
{
    //add:
    tableWidget_columna_setEnabled(0,false);
    tableWidget_columna_setEnabled(1,true);
    tableWidget_columna_setEnabled(2,false);
    tableWidget_columna_setEnabled(3,false);
    tableWidget_columna_setEnabled(4,true);

    tableWidget_columna_setEnabled(5,false);
    tableWidget_columna_setEnabled(6,false);
    //tableWidget_columna_setEnabled(7,false);
    //tableWidget_columna_setEnabled(8,false);

    //=4*PI()*0.765*(E4/B4)
}
void MainWindow::tableWidget_enable_for_L()
{
/*
    //add:
    tableWidget_columna_setEnabled(0,false);
    tableWidget_columna_setEnabled(1,true);
    tableWidget_columna_setEnabled(2,false);
    tableWidget_columna_setEnabled(3,false);
    tableWidget_columna_setEnabled(4,false);
    tableWidget_columna_setEnabled(5,true);
    tableWidget_columna_setEnabled(6,false);
    tableWidget_columna_setEnabled(7,false);
    tableWidget_columna_setEnabled(8,false);

    //=4*PI()*((2.39*0.125)/0.64)*(F4/B4)
*/
}

void MainWindow::on_radioButton_SP_clicked()
{
    USB_send_data_integer(USB_DATACODE_SET_SELECTOR, SELECTOR_SP);
    tableWidget_enable_for_SP();
}


void MainWindow::on_radioButton_NC_clicked()
{
    USB_send_data_integer(USB_DATACODE_SET_SELECTOR, SELECTOR_NC);
    tableWidget_enable_for_NC();
}

void MainWindow::on_radioButton_NL_clicked()
{
    USB_send_data_integer(USB_DATACODE_SET_SELECTOR, SELECTOR_NL);
    tableWidget_enable_for_NL();
}


void MainWindow::on_radioButton_L_clicked()
{
    USB_send_data_integer(USB_DATACODE_SET_SELECTOR, SELECTOR_L);
    tableWidget_enable_for_L();
}


void MainWindow::on_pushButton_Parar_clicked()
{
    //deshabilitar los controles
    ui->recorridoTotal->setEnabled(true);
    ui->intervalo->setEnabled(true);
    ui->pushButton_Reset->setEnabled(true);
    ui->pushButton_Motor->setEnabled(true);
    //
    ui->radioButton_SP->setEnabled(true);
    ui->radioButton_NC->setEnabled(true);
    ui->radioButton_NL->setEnabled(true);
    ui->radioButton_L->setEnabled(true);
    //
    ui->pushButton_Aceptar->setEnabled(true);

    ui->pushButton_Inicio->setEnabled(false);
    ui->pushButton_Pause->setEnabled(false);
    ui->pushButton_Parar->setEnabled(false);

    ui->pushButton_Reset->setEnabled(true);
    ui->pushButton_Motor->setEnabled(true);

    led_motor->setState(false);

    USB_send_data_integer(USB_DATACODE_SET_EXECUTION, PARAR);

    // if (tabla_numfila < TABLA_NUM_FILAS_TOTALES)
    // {
    //     tabla_update_cell_posicion(tabla_numfila);
    // }

    //
    qDebug()<<"on_pushButton_Parar_clicked"<< Qt::endl;

}

void MainWindow::on_pushButton_Inicio_clicked()
{
    ui->pushButton_Inicio->setEnabled(false);
    ui->pushButton_Pause->setEnabled(true);
    ui->pushButton_Parar->setEnabled(true);

    led_motor->setState(true);

    USB_send_data_integer(USB_DATACODE_SET_EXECUTION, INICIO);
    //
    qDebug()<<"on_pushButton_Inicio_clicked"<< Qt::endl;

}

void MainWindow::on_pushButton_Pause_clicked()
{
    ui->pushButton_Inicio->setEnabled(true);
    ui->pushButton_Pause->setEnabled(false);
    ui->pushButton_Parar->setEnabled(true);

    led_motor->setState(false);

    USB_send_data_integer(USB_DATACODE_SET_EXECUTION, PAUSA);

    tabla_update_cell_posicion(tabla_numfila);
    //
    qDebug()<<"on_pushButton_Pause_clicked"<< Qt::endl;

}


void MainWindow::on_pushButton_Reset_clicked()
{
    USB_send_data_integer(USB_DATACODE_SET_EXECUTION, RESET);

    //
    qDebug()<<"on_pushButton_Reset_clicked"<< Qt::endl;
}
void MainWindow::on_pushButton_Motor_clicked()
{
}

void MainWindow::on_pushButton_Motor_toggled(bool checked)
{
    if (checked == true)
    {
        USB_send_data_integer(USB_DATACODE_SET_MOTOR, MOTOR_ON);
        led_motor->setState(true);
        qDebug()<<"MOTOR_ON";
//added 15/06/26
//USB_send_data_integer(USB_DATACODE_CONTROL_ACTIVATED,0);
    }
    else
    {
        USB_send_data_integer(USB_DATACODE_SET_MOTOR, MOTOR_OFF);
        led_motor->setState(false);
        qDebug()<<"MOTOR_OFF";
    }

}


void MainWindow::on_pushButton_Exportar_clicked()
{
    char buff[20];

    QString filenameQString_datetimeStamp = QString("%1.xlsx").arg(QDateTime::currentDateTime().toString("ddMMyyyy-hh_mm_ss"));
    QString filenameQString = "ControlDiE-Exported" + filenameQString_datetimeStamp;
    //Se propone el filenameQString

    try
    {
        QString fileName2save = QFileDialog::getSaveFileName(this, tr("Guardar archivo"),filenameQString,tr("Excel (*.xlsx)"));

        //Bug fixed, no existe nombre de archivo porque el usuario cancelo el dialogo
        // Aquí verificas inmediatamente

        // Caso 1: el usuario canceló el diálogo -> no hacer nada
        if (fileName2save.isEmpty())
            return;  // El usuario canceló, no sigas exportando, cancela exportar...

        // Caso 2: el usuario eligió un archivo, pero no se puede abrir -> warning
/*        QFile file(fileName2save);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(this, tr("Error"),
                                 tr("No se pudo abrir el archivo para escribir."));
            return;
        }
*/
        // Opcional: comprobar permisos de escritura en la carpeta
        QFileInfo info(fileName2save);

        // Si el archivo ya existe: ¿puedo escribirlo?
        // Si no existe: ¿tengo permiso en la carpeta?
        if ((info.exists() && !info.isWritable()) ||
            (!info.exists() && !QFileInfo(info.absolutePath()).isWritable()))
        {
            QMessageBox::warning(this, tr("Error"),
                                 tr("No se puede escribir en la carpeta seleccionada."));
            return;
        }
        // Caso 3: todo bien -> ya puedes escribir
        QByteArray filenameQByteArray = fileName2save.toLocal8Bit();
        const char *filename = filenameQByteArray.data();
        //
        XLDocument doc;
        doc.create(filename, true);
        auto wks = doc.workbook().worksheet("Sheet1");

        doc.styles().numberFormats();

        wks.cell(1,1).value() = "POSICION";
        wks.cell(1,2).value() = "I\nmA";
        wks.cell(1,3).value() = "SP\nmV";
        wks.cell(1,4).value() = "Vnc\nmV";
        wks.cell(1,5).value() = "Vnl\nmV";
        wks.cell(1,6).value() = "Rnc\nΩ-m";
        wks.cell(1,7).value() = "Rnl\nΩ-m";


        for (int r=0; r < TABLA_NUM_FILAS_TOTALES; r++)
        {
            for (int col=0; col<TABLA_NUMBER_COLUMNAS; col++)
            {
                QString qstr_item = tableWidget->item(r, col)->text();
                double value = qstr_item.toDouble();
                // QByteArray str_item = qstr_item.toLocal8Bit();
                // strcpy(buff,str_item);
                //
                wks.cell(r+2,col+1).value() = value;//buff;
            }
        }

        doc.save();
        doc.close();

        QMessageBox::information(this, tr("Éxito"),
                                 tr("Archivo exportado correctamente:\n%1").arg(fileName2save));
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, tr("Error al exportar"), e.what());
    }
}

