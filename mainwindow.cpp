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
 *
 *RTM 3/09/2026 para entregar a Wilfredo
 */
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "config.h"
#include <QThread>
#include <QDateTime>
#include <QFileDialog>
#include <QString>
#include <OpenXLSX.hpp>
#include <QStandardPaths>
#include <QSaveFile>
#include <QTextStream>
#include "dialogorestaurarsesion.h"

using namespace OpenXLSX;

#define TABLA_NUMBER_COLUMNAS 7//9 se elimina Vl y Rl

#define TABLA_NUMBER_FILAS_ADICIONALES 0//5//filas adicionales a las que se crean

void MainWindow::buttons_disable()
{
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
    ui->pushButton_Reset->setEnabled(true);
    ui->pushButton_Motor->setEnabled(true);

    ui->pushButton_Inicio->setEnabled(false);
    ui->pushButton_Pause->setEnabled(false);
    ui->pushButton_Parar->setEnabled(true);
    ui->pushButton_Aceptar->setEnabled(true);

    ui->menuConfig->setEnabled(true);
    led_motor->setEnabled(true);

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

bool MainWindow::usbport_available(void)
{
    usbCDC_is_available = false;
    usbCDC_port_name = "";


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

    /*Sí. En tu arquitectura actual, lo más limpio es aprovechar que usbport_available() ya detecta el cambio de estado mediante usbport_status_last != usbport_status. Cuando aparece el USB abres el puerto y habilitas la GUI; cuando desaparece cierras el puerto y deshabilitas controles.

    Yo no mostraría inmediatamente un QMessageBox adicional del tipo “¿Desea restaurar?”. Ya tienes RestaurarSesion() → consulta del recorrido al micro → DialogoRestaurarSesion, que finalmente permite Restaurar/Cancelar. Así evitas preguntar dos veces.
    Aplicación arranca
          ↓
    USB aparece por primera vez
          ↓
    NO preguntar por restauración


    USB estaba conectado
          ↓
    se desconecta
          ↓
    marcamos:
    m_huboDesconexionUSB = true


    USB vuelve a conectarse
          ↓
    detectamos que fue una RECONEXIÓN
          ↓
    RestaurarSesion()
          ↓
    GET recorrido actual
          ↓
    micro responde
          ↓
    comparar sesión/hardware
          ↓
    [ Restaurar ] [ Cancelar ]
    *
    *
    *6. Hay un detalle importante en usbport_available()

Actualmente haces:

usbport_status = usbport_status_temp;


if (usbport_status_last != usbport_status)
{
    usbport_status_last = usbport_status;

Eso está bien para detectar flancos:

false → true    conexión
true  → false   desconexión

Pero al iniciar la aplicación tienes:

usbport_status = false;
usbport_status_last = usbport_status;

Por eso la primera conexión produce:

false → true

igual que una reconexión.

La bandera adicional:

m_huboDesconexionUSB

es precisamente la que distingue ambos casos:

ARRANQUE DEL SOFTWARE


status_last = false
status      = true
m_huboDesconexionUSB = false


→ conecta normalmente
→ NO pregunta restauración

Mientras que:

DESCONEXIÓN


true → false


m_huboDesconexionUSB = true

y luego:

RECONEXIÓN


false → true
m_huboDesconexionUSB = true


→ sí inicia RestaurarSesion()

Eso responde exactamente a tu requisito de sólo hacerlo cuando la aplicación ya estaba activa y hubo una desconexión real.
     */
    if (usbport_status_last != usbport_status )
    {
        usbport_status_last = usbport_status;

        if (usbport_status == true)// conectar
        {

            usbCDC->setPortName(usbCDC_port_name);

            if (!usbCDC->open(QSerialPort::ReadWrite))
            {
                qDebug() << "No se pudo abrir el puerto"
                         << usbCDC_port_name
                         << usbCDC->errorString();
                // NO limpiar m_huboDesconexionUSB
                return false;
            }

            //usbCDC = new QSerialPort; //bug... se traslada directo al constructor pero con this


            //usbCDC->open(QSerialPort::ReadWrite);
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

            //esta conexion solo debi hacerlo 1 sola vez, este bug tamb. se corrige
            //QObject::connect(usbCDC, SIGNAL(readyRead()), this, SLOT(readSerial()));

            //ui->status->setText("Conexión OK: Tarjeta de control encontrado");
            qDebug() << "Conexión OK: Tarjeta de control encontrado";
            //
            led_enlace->setState(true);

            USB_send_data_integer(USB_DATACODE_SET_SELECTOR, SELECTOR_SP);
            tableWidget_enable_for_SP();
            //

            buttons_enable();

            //added
            // configurar puerto...
            // ----------------------------------------
            // ¿Es una reconexión después de una caída?
            // ----------------------------------------
            if (m_huboDesconexionUSB)
            {
                m_huboDesconexionUSB = false;

                qDebug()
                    << "USB reconectado. Verificando restauración de sesión...";

                /*
                 * No lo ejecutamos directamente dentro del manejo
                 * de conexión. Dejamos terminar usbport_available()
                 * y después iniciamos el proceso de restauración.
                 *
                 * 4. ¿Por qué uso QTimer::singleShot(100, ...)?

No porque necesites “esperar” al micro bloqueando el programa.

Es justamente lo contrario.

Cuando detectas la conexión estás dentro de:
usbport_available()
y acabas de:

usbCDC->open(...)
configurar el puerto, habilitar botones y enviar incluso:

USB_DATACODE_SET_SELECTOR

En vez de meter inmediatamente toda la restauración dentro de esa misma función, hacemos:

QTimer::singleShot(100, this, ...);
que significa:

termina usbport_available()
        ↓
Qt vuelve al event loop
        ↓
procesa eventos seriales pendientes
        ↓
100 ms después
        ↓
inicia restauración

No bloquea absolutamente nada.

Incluso podrías usar:

QTimer::singleShot(0, ...)
pero en una reconexión USB física prefiero dejar un pequeño margen antes de enviar el GET al AVR.


Dentro del singleShot, tienes:

if (usbport_status &&
    usbCDC->isOpen() &&
    ExisteSesion())
{
    RestaurarSesion();
}

Eso evita tres casos problemáticos:

1. el USB volvió a desaparecer durante esos 100 ms
2. QSerialPort no quedó realmente abierto
3. no existe session.json

Y si no existe sesión, yo no mostraría ningún mensaje al reconectarse. Simplemente continúas normalmente; no hay nada que restaurar.

Con estos cambios tienes una recuperación bastante natural:

USB se desconecta
       ↓
GUI indica desconectado
       ↓
no se bloquea la aplicación
       ↓
USB vuelve
       ↓
puerto se abre
       ↓
se solicita automáticamente
posición actual al AVR
       ↓
se compara contra session.json
       ↓
DialogoRestaurarSesion


"Recorrido guardado: 3.00 m
 Recorrido actual:    3.27 m
 Diferencia:          0.27 m


 ¿Desea continuar con la restauración?"


[ Restaurar ] [ Cancelar ]

Y tanto si selecciona Restaurar como Cancelar, podemos aplicar después lo que comentaste: mantener ui->recorridoActual mostrando el valor real recibido del micro. Ese sería el siguiente ajuste que haría sobre ProcesarRecorridoActualMicro().
                 */

                QTimer::singleShot(100, this, [this]()
                                   {
                                       if (usbport_status &&
                                           usbCDC->isOpen() &&
                                           ExisteSesion())
                                       {
                                           RestaurarSesion();
                                       }
                                   });
            }
        }
        else // desconectar
        {
            /*
             * Si estábamos esperando una respuesta del micro,
             * esa transacción ya no puede completarse.
             *
             * Esto último es importante por otro motivo: actualmente tienes un timeout asociado a la consulta del recorrido. El timer se crea como singleShot y está conectado con TimeoutRecorridoMicro()
             * Si la tarjeta desaparece mientras:

                esperandoRecorridoMicro == true

                no tiene sentido dejar los 500 ms corriendo para después mostrar:

                La tarjeta de control no respondió...

                Ya sabemos por qué no respondió: se desconectó.

                Por eso conviene cancelar inmediatamente:
                esperandoRecorridoMicro = false;
                timerRespuestaRecorrido->stop();
             */
            if (esperandoRecorridoMicro)
            {
                esperandoRecorridoMicro = false;

                if (timerRespuestaRecorrido)
                    timerRespuestaRecorrido->stop();
            }

            m_restaurandoSesion = false;//porque esa operación de restauración quedó abortada.

            //if (usbCDC)//ya no es necesario preguntar si el puntero es diferente de null porque nunca lo debi setear asi
            if (usbCDC->isOpen())
                usbCDC->close();

            //QMessageBox::warning(this, "Port error", "Tarjeta de control no encontrado");
            //ui->status->setText("Port error, Tarjeta de control no encontrado");
            qDebug() << "Port error, Tarjeta de control no encontrado";

            led_enlace->setState(false);

            buttons_disable();

            // Recordamos que hubo una desconexión real.
            m_huboDesconexionUSB = true;
        }
    }

    return usbCDC_is_available;
}

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
        {
            bool foundit = false;
            double recorridoactual = ui->recorridoActual->value();

            for (int i=0; i<tabla_numfila_actual_marcado; i++)
            {
                QTableWidgetItem *item = tableWidget->item(i, 0);
                if (item)
                {
                    //QString qstr_item = tableWidget->item(i, 0)->text();
                    QString qstr_item = item->text();
                    double value = qstr_item.toDouble();

                    if (value == recorridoactual)//existe
                    {
                        // Resaltar la fila actual
                        tableWidget->selectRow(i);
                        tableWidget->setFocus(Qt::OtherFocusReason);
                        foundit = true;

                        //ui->pushButton_Inicio->setEnabled(true);

                        break;
                    }
                }
            }

            if ( (tabla_numfila_actual_marcado < TABLA_NUM_FILAS_TOTALES) && (!foundit))
            {

                //QString str_recorrido_actual = QString::number(recorridoactual,'f',2);
                //QTableWidgetItem* text = new QTableWidgetItem();
                //text->setText(str_recorrido_actual);
                //tableWidget->setItem(tabla_numfila_actual_marcado,0, text);

                QTableWidgetItem *item = tableWidget->item(tabla_numfila_actual_marcado, 0);//obtener el puntero actual
                if (item)
                {
                    item->setText(QString::number(recorridoactual,'f',2));
                    tableWidget_columna_setEnabled(0,false);

                    // Resaltar la fila actual
                    tableWidget->selectRow(tabla_numfila_actual_marcado);
                    tableWidget->setFocus(Qt::OtherFocusReason);

                    //ui->pushButton_Inicio->setEnabled(true);

                    tabla_numfila_actual_marcado++;
                }
            }

            ui->pushButton_Pause->setEnabled(false);
            ui->pushButton_Parar->setEnabled(true);

            //nuevo
            //--------------------------------------------------------------------------
            //forward
            // [INICIO]-> led motor ON

            // Si boton ACEPTAR está disable, significa que es controlado por INICIO, entonces en cada intervalo debe de desactivarse el led_motor


            // //reverse
            // cuando [INICIO] esta deshabiltado previamente, y presiona buton motor y al pasar por el intervalo no debe de deshabilitarse el led motor, debe permanecer ON

            // Si boton ACEPTAR esta EANBLE, entonces el led_motor no apaga en los intervalos
            // todo depende del sentido de giro puesto por el operario
            //
            //--------------------------------------------------------------------------
            if ((!ui->pushButton_Aceptar->isEnabled()) && (ui->pushButton_Inicio->isEnabled()) )//added
            {
                //led_motor->setState(true);
                qDebug()<<"led_motor->setState(true);"<< Qt::endl;
            }

            if ( (!ui->pushButton_Aceptar->isEnabled()) && (!ui->pushButton_Inicio->isEnabled()) ) //added
            {
                led_motor->setState(false);
                qDebug()<<"led_motor->setState(false);"<< Qt::endl;
                //
                ui->pushButton_Inicio->setEnabled(true);//volver habilitar [INICIO]


            }

            qDebug()<<"USB_DATACODE_INTERVALO_COMPLETO: "<<Qt::endl;

        break;
        }
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
        case USB_DATACODE_CONTROL_ACTIVATED://no estoy enviando desde el MCU
            payload_i = atoi(USB_payload_char);
            qDebug()<<"USB_DATACODE_CONTROL_ACTIVATED"<< payload_i<< Qt::endl;

            break;

        case USB_DATACODE_RESP_RECORRIDO_ACTUAL:
        {
            double recorridoMicro = atof(USB_payload_char);

            qDebug() << "Recorrido solicitado al micro:"
                     << recorridoMicro;

            if (esperandoRecorridoMicro)
            {
                esperandoRecorridoMicro = false;
                timerRespuestaRecorrido->stop();

                ProcesarRecorridoActualMicro(recorridoMicro);
            }

            break;
        }

        default: break;
    }
}

static QString str_acc = "";
int yy;

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

    config = new Config(this);
    connect(ui->actionRestaurar_ultima, &QAction::triggered, this, &MainWindow::abrirDialogoRestaurarSesion);

    // -------------------------------------------------------------
    // Conexión explícita usando la sintaxis moderna de punteros a función
    // -------------------------------------------------------------
    //en el GUI, quedo con el nombre actionConstantes a la opcion "Constantes" y es de tipo QAction
    connect(ui->actionConstantes, &QAction::triggered, this, &MainWindow::abrirDialogoConstantes);

    LeerConfiguracionTXT();
    // -------------------------------------------------------------
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

    //added 2026 Con esto, al llamar a selectRow() se resaltará toda la fila.
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    QStringList headers;

    //headers << "POSICION" << "I\nmA" << "SP\nmV" << "Vnc\nmV" << "Vnl\nmV" << "Vl\nmV" << "Rnc\nΩ-m" << "Rnl\nΩ-m" << "Rl\nΩ-m";
    headers << "POSICION" << "I\nmA" << "SP\nmV" << "Vnc\nmV" << "Vnl\nmV" << "Rnc\nΩ-m" << "Rnl\nΩ-m";

    tableWidget->setHorizontalHeaderLabels(headers);

    //bug fixed 2026
    usbCDC = new QSerialPort(this);
    QObject::connect(usbCDC, SIGNAL(readyRead()), this, SLOT(readSerial()));
    led_motor = new LedIndicator(this);//bug fixed, add this
    led_enlace = new LedIndicator(this);//bug fixed, add this

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


    //+------------------------- Session manager 2026
    m_spinBoxes    = findChildren<QDoubleSpinBox*>();
    m_radioButtons = findChildren<QRadioButton*>();
    m_pushButtons  = findChildren<QPushButton*>();
    // m_lineEdits    = findChildren<QLineEdit*>();
    // m_checkBoxes   = findChildren<QCheckBox*>();


    //-------------------------------------------------
    m_timerAutoSave = new QTimer(this);
    m_timerAutoSave->setSingleShot(true);
    connect(m_timerAutoSave,
            &QTimer::timeout,
            this,
            &MainWindow::GuardarSesion);
    //-------------------------------------------------
    ConfigurarAutoSave();
    //----------------------------+ Session manager 2026

    timerRespuestaRecorrido = new QTimer(this);
    timerRespuestaRecorrido->setSingleShot(true);

    connect(timerRespuestaRecorrido,
            &QTimer::timeout,
            this,
            &MainWindow::TimeoutRecorridoMicro);
}

MainWindow::~MainWindow()
{
    delete ui;

    //Bug fixed 5
    /*Pero nunca liberas usbCDC, timer, tableWidget, ni los LedIndicator. Esto es una fuga de memoria
     * */
    // if (usbCDC)
    // {
    //     if (usbCDC->isOpen())
    //     {
    //         usbCDC->close();
    //     }
    //     delete usbCDC;
    // }
    // delete led_motor;
    // delete led_enlace;

    delete timer;
    delete tableWidget;

}
void MainWindow::abrirDialogoConstantes()
{
    config->setConfiguracionToGUI(configuracionSistema);

    if (config->exec() != QDialog::Accepted)
        return;

    const auto nuevaConfiguracion = config->getConfiguracionFromGUI();

    if (nuevaConfiguracion.encoderPPR <= 0)
    {
        QMessageBox::warning(
            this,
            tr("Configuración inválida"),
            tr("Los pulsos por revolución del encoder deben ser mayores que 0.")
            );

        return;
    }

    if (nuevaConfiguracion.longitudArco <= 0.0f)
    {
        QMessageBox::warning(
            this,
            tr("Configuración inválida"),
            tr("La longitud de arco debe ser mayor que 0.")
            );

        return;
    }

    // Sólo ahora reemplazamos la configuración activa.
    configuracionSistema = nuevaConfiguracion;

    USB_send_data_integer(
        USB_DATACODE_SET_ENCODER_PPR,
        configuracionSistema.encoderPPR);

    USB_send_data_float(
        USB_DATACODE_SET_LONGITUD_ARCO,
        configuracionSistema.longitudArco);

    GuardarConfiguracionTXT();
    NotificarCambioEstado();
}
/*
 * RestaurarSesion()
       ↓
Leer JSON
       ↓
convertir a SessionData
       ↓
guardar temporalmente esa sesión
       ↓
pedir recorrido al AVR
       ↓
return


Después:

respuesta del AVR
       ↓
usar exactamente la SessionData
que ya habíamos leído
       ↓
comparar
       ↓
mostrar diálogo
 */
void MainWindow::abrirDialogoRestaurarSesion(void)
{
    // DialogoRestaurarSesion dlg(this);
    // dlg.setDiferencias(diferencias);
    // if (dlg.exec() != QDialog::Accepted)
    //     return;
    // //AplicarSessionData(sessionData);
    RestaurarSesion();

}
bool MainWindow::GuardarConfiguracionTXT()
{
    QSaveFile file("config.txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);//QTextStream ya tiene sobrecargado el operador de inserción << para la gran mayoría de tipos nativos de C++ (int, float, double, uint32_t, etc.).

    // Opcional: Para asegurar que el float se guarde con 2 decimales sin notación científica
    // out << Qt::fixed << qSetRealNumberPrecision(2);
    out << configuracionSistema.encoderPPR << Qt::endl;
    out << configuracionSistema.longitudArco;

    //return true;
    // Confirmar los cambios atómicamente en disco
    return file.commit();
}


bool MainWindow::LeerConfiguracionTXT(void)
{
    bool codret=false;

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
        configuracionSistema.encoderPPR = encoderPPR.toInt();
        qDebug()<< "encoderPPR" << encoderPPR;

        //
        QString LongitudArco = in.readLine();
        configuracionSistema.longitudArco = LongitudArco.toFloat();
        qDebug()<< "LongitudArco" << LongitudArco;

        config->setConfiguracionToGUI(configuracionSistema);
        codret = true;
    }


    file.close();
    return codret;
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

}

void MainWindow::CrearTabla(double recorrido, double intervalo)
{
    TABLA_NUM_FILAS_TOTALES =(int) (recorrido /intervalo) + TABLA_NUMBER_FILAS_ADICIONALES;

    tableWidget->setRowCount(TABLA_NUM_FILAS_TOTALES);
    tableWidget->setColumnCount(TABLA_NUMBER_COLUMNAS);

    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    //excusa para poder tener un puntero despues con tableWidget->item(r,0)
    //En realidad, para QTableWidget es necesario inicicializar todos los items como "texto vacio" para que cuando se exporte
    //al leer el valor de celdas vacias, no de error. Es como si cada celda necesita previamente tener un valor aun asi no se use

    // QTableWidget no crea automáticamente un QTableWidgetItem para cada celda.
    // Después de definir el número de filas y columnas, todas las celdas contienen nullptr.
    //
    // Como en el resto del programa se accede directamente mediante:
    //
    //     tableWidget->item(r, c)->text();
    //
    // inicializamos previamente todas las celdas con un QTableWidgetItem vacío para
    // evitar comprobar continuamente si el puntero es nullptr.

    // Decisión de diseño:
    //
    // Preferimos que todas las celdas tengan siempre un QTableWidgetItem válido,
    // aunque su texto esté vacío. De esta forma el resto del código puede acceder
    // directamente a tableWidget->item(r,c) sin realizar comprobaciones de nullptr.

    // Inicializamos todas las celdas para garantizar que tableWidget->item(r,c)
    // nunca devuelva nullptr. Esto simplifica el resto del código al evitar
    // comprobaciones repetitivas antes de acceder al contenido de una celda.


    // Entonces el motivo real es:
    // No es para "tener un puntero".
    // El puntero ya existe.
    // Lo que ocurre es que apunta a nullptr.
    // Lo que necesitas es que cada celda tenga un objeto asociado.

    for (int r=0; r < TABLA_NUM_FILAS_TOTALES; r++)
    {
        for (int col=0; col<TABLA_NUMBER_COLUMNAS; col++)
        {
            // QTableWidgetItem* text = new QTableWidgetItem();
            // text->setText("");//realmente es innecesaria. QTableWidgetItem recién creado ya contiene un texto vacío.

            auto *item = new QTableWidgetItem;
            tableWidget->setItem(r, col, item);
        }
    }
    //Posicion disable
    tableWidget_columna_setEnabled(0,false);

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


}

void MainWindow::ConfigurarEstadoInicialEnsayo()
{
    //deshabilitar los controles
    ui->recorridoTotal->setEnabled(false);
    ui->intervalo->setEnabled(false);
    ui->pushButton_Reset->setEnabled(false);
    ui->pushButton_Aceptar->setEnabled(false);
    ui->pushButton_Inicio->setEnabled(true);
    ui->pushButton_Parar->setEnabled(true);
    ui->pushButton_Reset->setEnabled(false);

    //added 2026
    //     ui->pushButton_Motor->setChecked(true);

    // if (ui->pushButton_Motor->isChecked())
    // {
    //     ui->pushButton_Motor->setChecked(false);//tengo que enviar la orden al micro que pare
    //     led_motor->setState(false);
    // }
}
void MainWindow::EnviarConfiguracionAlMicro()
{
    USB_send_data_float(USB_DATACODE_SET_RECORRIDO_TOTAL, RECORRIDOTOTAL );//rt);
    //QThread::msleep(10);
    USB_send_data_float(USB_DATACODE_SET_INTERVALO, itv);
    //QThread::msleep(10);
}

void MainWindow::on_pushButton_Aceptar_clicked()
{
    RECORRIDOTOTAL = ui->recorridoTotal->value();
    itv = ui->intervalo->value();

    QStringList errores;

    if (RECORRIDOTOTAL <= 0.0)
        errores << tr("El Recorrido Total debe ser mayor que 0.");

    if (itv <= 0.0)
        errores << tr("El intervalo debe ser mayor que 0.");

    if (!errores.isEmpty())
    {
        QMessageBox::warning(
            this,
            tr("Configuración inválida"),
            errores.join("\n"));

        return;
    }

    BloquearSenalesGUI(true);

    CrearTabla(RECORRIDOTOTAL, itv);

    tabla_numfila_actual_marcado = 0; //reset
    //--------------------------------------------
    ConfigurarEstadoInicialEnsayo();

    EnviarConfiguracionAlMicro();

    tableWidget->clearSelection();
    tableWidget->clearFocus();
    //--------------------------------------------
    BloquearSenalesGUI(false);


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
void MainWindow::USB_send_data_selector(TipoRegistro tipo)
{
    USB_send_data_integer(USB_DATACODE_SET_SELECTOR, static_cast<int>(tipo));
}
void MainWindow::on_radioButton_SP_clicked()
{
    //USB_send_data_integer(USB_DATACODE_SET_SELECTOR, SELECTOR_SP);
    sessionData.config.tipoRegistro = TipoRegistro::SP;
    USB_send_data_selector(sessionData.config.tipoRegistro);
    tableWidget_enable_for_SP();
}

void MainWindow::on_radioButton_NC_clicked()
{
    //USB_send_data_integer(USB_DATACODE_SET_SELECTOR, SELECTOR_NC);
    sessionData.config.tipoRegistro = TipoRegistro::NC;
    USB_send_data_selector(sessionData.config.tipoRegistro);
    tableWidget_enable_for_NC();
}

void MainWindow::on_radioButton_NL_clicked()
{
    //USB_send_data_integer(USB_DATACODE_SET_SELECTOR, SELECTOR_NL);
    sessionData.config.tipoRegistro = TipoRegistro::NL;
    USB_send_data_selector(sessionData.config.tipoRegistro);
    tableWidget_enable_for_NL();
}

void MainWindow::on_radioButton_L_clicked()
{
    //USB_send_data_integer(USB_DATACODE_SET_SELECTOR, SELECTOR_L);
    sessionData.config.tipoRegistro =  TipoRegistro::L;
    USB_send_data_selector(sessionData.config.tipoRegistro);
    tableWidget_enable_for_L();
}


void MainWindow::MCU_motor_set_state(bool state)
{
    if (state == true)
    {
        USB_send_data_integer(USB_DATACODE_SET_MOTOR, MOTOR_ON);
        qDebug()<<"USB_send_data_integer(USB_DATACODE_SET_MOTOR, MOTOR_ON);";
    }
    else
    {
        USB_send_data_integer(USB_DATACODE_SET_MOTOR, MOTOR_OFF);
        qDebug()<<"USB_DATACODE_SET_MOTOR, MOTOR_OFF";
    }
}
void MainWindow::GUI_ledmotor_set_state(bool state)
{
    if (state == true)
    {
        led_motor->setState(true);
        qDebug()<<"led_motor->setState(true);";
    }
    else
    {
        led_motor->setState(false);
        qDebug()<<"led_motor->setState(false);";
    }
}

void MainWindow::on_pushButton_Parar_clicked()
{

    ui->recorridoTotal->setEnabled(true);
    ui->intervalo->setEnabled(true);
    ui->pushButton_Reset->setEnabled(true);
    ui->pushButton_Motor->setEnabled(true);
    ui->pushButton_Motor->setChecked(false);
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


    qDebug()<<"on_pushButton_Parar_clicked"<< Qt::endl;

}

void MainWindow::on_pushButton_Inicio_clicked()
{
    ui->pushButton_Inicio->setEnabled(false);
    ui->pushButton_Pause->setEnabled(true);
    ui->pushButton_Parar->setEnabled(true);


    //new added 2026

    ui->pushButton_Motor->blockSignals(true);
    ui->pushButton_Motor->setChecked(false);//solo si conmuta el estado del boton, disparara intencionalmente a on_pushButton_Motor_toggled(bool checked)
    ui->pushButton_Motor->setEnabled(false);
    ui->pushButton_Motor->blockSignals(false);

    //MCU_motor_set_state(true);//firmware activa el motor al recibir INICIO
    GUI_ledmotor_set_state(true);

    USB_send_data_integer(USB_DATACODE_SET_EXECUTION, INICIO);
    //
    qDebug()<<"on_pushButton_Inicio_clicked"<< Qt::endl;

    //el microcontrolador cuando recibe inicio levanta el flag control_recorrido = 1;
    //USB_send_data_integer(USB_DATACODE_CONTROL_ACTIVATED,0);
}

void MainWindow::on_pushButton_Pause_clicked()
{
    ui->pushButton_Inicio->setEnabled(true);
    ui->pushButton_Pause->setEnabled(false);
    ui->pushButton_Parar->setEnabled(true);

    led_motor->setState(false);

    USB_send_data_integer(USB_DATACODE_SET_EXECUTION, PAUSA);

    tabla_update_cell_posicion(tabla_numfila_actual_marcado);
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
    MCU_motor_set_state(checked);
    GUI_ledmotor_set_state(checked);
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

/*
 * Como ya tengo listas cacheadas como m_spinBoxes, m_radioButtons y probablemente m_pushButtons, es mejor reutilizarlas en vez de ejecutar findChildren<>() cada vez.
 * */
void MainWindow::BloquearSenalesGUI(bool bloquear)
{
    foreach (QDoubleSpinBox *spin, m_spinBoxes)
    {
        spin->blockSignals(bloquear);
    }

    foreach (QRadioButton *radio, m_radioButtons)
    {
        radio->blockSignals(bloquear);
    }

    foreach (QPushButton *button, m_pushButtons)
    {
        button->blockSignals(bloquear);
    }

    /*
    foreach (QDoubleSpinBox *spin,
             findChildren<QDoubleSpinBox*>())
    {
        spin->blockSignals(bloquear);
    }

    foreach (QRadioButton *radio,
             findChildren<QRadioButton*>())
    {
        radio->blockSignals(bloquear);
    }

    foreach (QPushButton *button,
             findChildren<QPushButton*>())
    {
        button->blockSignals(bloquear);
    }

    foreach (QLineEdit *edit,
             findChildren<QLineEdit*>())
    {
        edit->blockSignals(bloquear);
    }

    foreach (QCheckBox *check,
             findChildren<QCheckBox*>())
    {
        check->blockSignals(bloquear);
    }
    */
    tableWidget->blockSignals(bloquear);
}
void MainWindow::ConfigurarAutoSave()
{

    foreach (QDoubleSpinBox *spin, m_spinBoxes )
    {
        connect(spin,
                QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this,
                &MainWindow::NotificarCambioEstado);
    }
    foreach (QRadioButton *radio, m_radioButtons )
    {
        connect(radio,
                &QRadioButton::toggled,
                this,
                [this](bool checked)
                {
                    if (checked)
                        NotificarCambioEstado();
                });
    }

    foreach (QPushButton *button, m_pushButtons)
    {
        if (button == ui->pushButton_Exportar)
            continue;

        if (button->isCheckable())
        {
            connect(button,
                    &QPushButton::toggled,
                    this,
                    [this](bool)
                    {
                        NotificarCambioEstado();
                    });
        }
        else
        {
            connect(button,
                    &QPushButton::clicked,
                    this,
                    &MainWindow::NotificarCambioEstado);
        }
    }
    /*
     *     foreach (QDoubleSpinBox *spin, findChildren<QDoubleSpinBox*>() )
    {
        connect(spin,
                QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this,
                &MainWindow::NotificarCambioEstado);
    }
    foreach (QRadioButton *radio, findChildren<QRadioButton*>())
    {
        connect(radio,
                &QRadioButton::toggled,
                this,
                [this](bool checked)
                {
                    if (checked)
                        NotificarCambioEstado();
                });
    }

     */
    /*
    foreach(QLineEdit *edit,
             findChildren<QLineEdit*>())
    {
        connect(edit,
                &QLineEdit::editingFinished,
                this,
                &MainWindow::onEstadoModificado);
    }
*/
    /*
     *  //-------------------------------------------------
    // Todos los QSpinBox
    //-------------------------------------------------

    foreach(QSpinBox *spin,
            findChildren<QSpinBox*>())
    {
        connect(spin,
                QOverload<int>::of(&QSpinBox::valueChanged),
                this,
                &MainWindow::onEstadoModificado);
    }

    //-------------------------------------------------
    // Todos los QLineEdit
    //-------------------------------------------------

    foreach(QLineEdit *edit,
            findChildren<QLineEdit*>())
    {
        connect(edit,
                &QLineEdit::editingFinished,
                this,
                &MainWindow::onEstadoModificado);
    }

    //-------------------------------------------------
    // Todos los QRadioButton
    //-------------------------------------------------

    foreach(QRadioButton *radio,
            findChildren<QRadioButton*>())
    {
        connect(radio,
                &QRadioButton::toggled,
                this,
                &MainWindow::onEstadoModificado);
    }

    //-------------------------------------------------
    // Todos los QCheckBox
    //-------------------------------------------------

    foreach(QCheckBox *check,
            findChildren<QCheckBox*>())
    {
        connect(check,
                &QCheckBox::toggled,
                this,
                &MainWindow::onEstadoModificado);
    }

    //-------------------------------------------------
    // Todos los ComboBox
    //-------------------------------------------------

    foreach(QComboBox *combo,
            findChildren<QComboBox*>())
    {
        connect(combo,
                QOverload<int>::of(&QComboBox::currentIndexChanged),
                this,
                &MainWindow::onEstadoModificado);
    }
    */
    //-------------------------------------------------
    // Todas las tablas
    //-------------------------------------------------

    foreach(QTableWidget *table,
            findChildren<QTableWidget*>())
    {
        connect(table,
                &QTableWidget::itemChanged,
                this,
                &MainWindow::NotificarCambioEstado);
    }

}
//----------------------------------------------------
void MainWindow::NotificarCambioEstado()
{
    if (m_restaurandoSesion)
        return;  // Ignora cambios provocados por el propio programa

    m_sesionModificada = true;
    m_timerAutoSave->start(2000);   //reinicia el temporizador

    qDebug()<< "Estado modificado";
}

//-----------------------------------------------------------------
//No olvidar GuardarSesion() es disparado por
//connect(m_timerAutoSave,&QTimer::timeout,this,&MainWindow::GuardarSesion);
void MainWindow::GuardarSesion()
{
    qDebug() << "guardando sesion";

    if (m_restaurandoSesion)
    {
        qDebug() << "Autosave ignorado: restauración en curso";
        return;
    }

    if (!m_sesionModificada)
        return;

    SessionData data = ObtenerSessionData();

    if (EscribirArchivoSesion(data.toJson()))
    {
        m_sesionModificada = false;

        qDebug() << "Sesion guardada";
    }
}
//De GUI hacia structs ----------------------------------------------------
SessionData MainWindow::ObtenerSessionData()
{
    sessionData.config.recorridoTotal = ui->recorridoTotal->value();
    sessionData.config.intervalo = ui->intervalo->value();
    sessionData.config.longitudArco = configuracionSistema.longitudArco;
    sessionData.config.encoderPPR = configuracionSistema.encoderPPR;
    //sessionData.config.tipoRegistro en cada evento de seleccion es directamente acualizado
    //
    sessionData.estado.tabla_numfila_actual_marcado = tabla_numfila_actual_marcado;
    //
    sessionData.estado.recorridoActual = ui->recorridoActual->value();
    sessionData.estado.recorridoTotal_isEnabled = ui->recorridoTotal->isEnabled();
    //
    sessionData.estado.intervalo_isEnabled = ui->intervalo->isEnabled();

    sessionData.estado.pushButton_Reset_isEnabled= ui->pushButton_Reset->isEnabled();

    sessionData.estado.pushButton_Motor_isChecked = ui->pushButton_Motor->isChecked();
    sessionData.estado.pushButton_Motor_isEnabled = ui->pushButton_Motor->isEnabled();

    sessionData.estado.led_motor_state = led_motor->isOn();
    sessionData.estado.led_motor_isEnabled= led_motor->isEnabled();

    sessionData.estado.pushButton_Inicio_isEnabled = ui->pushButton_Inicio->isEnabled();
    sessionData.estado.pushButton_Pausa_isEnabled = ui->pushButton_Pause->isEnabled();
    sessionData.estado.pushButton_Parar_isEnabled = ui->pushButton_Parar->isEnabled();
    sessionData.estado.pushButton_Aceptar_isEnabled = ui->pushButton_Aceptar->isEnabled();




    FilaMedicion registro;
    //
    sessionData.tabla.clear();
    sessionData.tabla.reserve(TABLA_NUM_FILAS_TOTALES);
    //

    for (int r=0; r < tableWidget->rowCount(); r++)
    {
        //Cuando el orden en el que uno quiere que se grabe el json importa, utilizan un array.
        registro.posicion = tableWidget->item(r, 0)->text().toDouble();
        registro.corriente = tableWidget->item(r, 1)->text().toDouble();
        registro.sp = tableWidget->item(r, 2)->text().toDouble();
        registro.vnc= tableWidget->item(r, 3)->text().toDouble();
        registro.vnl = tableWidget->item(r, 4)->text().toDouble();
        registro.rnc = tableWidget->item(r, 5)->text().toDouble();
        registro.rnl= tableWidget->item(r, 6)->text().toDouble();

        sessionData.tabla.append(registro);
    }

    return sessionData;
}
//----------------------------------------------------
void MainWindow::ActualizarCelda(int fila , int col, double valor)
{
    //No crear un QTableWidgetItem nuevo cada vez
    //El problema es que ya habíamos creado todos los items cuando llamaste a CrearTabla().
    // Por lo tanto, aquí ya existe un QTableWidgetItem.
    // No hace falta crear otro.
    // QString str = QString::number(valor,'f',2);
    // auto text = new QTableWidgetItem();
    // text->setText(str);
    // tableWidget->setItem(fila, col, text);

    // Porque si haces   setItem()
    // Qt elimina automáticamente el anterior y coloca el nuevo.
    // Funciona.
    // Pero estás creando cientos o miles de objetos innecesariamente.

    // No crea memoria.
    // No destruye objetos.
    // Sólo modifica el texto.

    QTableWidgetItem *item = tableWidget->item(fila, col);
    if (item)
    {
        item->setText(QString::number(valor,'f',2));
    }
}



void MainWindow::AplicarConfiguracion(const Configuracion &configuracion)
{
    switch (configuracion.tipoRegistro)
    {
        case TipoRegistro::SP: ui->radioButton_SP->setChecked(true);
            break;

        case TipoRegistro::NC: ui->radioButton_NC->setChecked(true);
            break;

        case TipoRegistro::NL: ui->radioButton_NL->setChecked(true);
            break;

        case TipoRegistro::L: ui->radioButton_L->setChecked(true);
            break;
        default:break;
    }

    ui->recorridoTotal->setValue(configuracion.recorridoTotal);

    ui->intervalo->setValue(configuracion.intervalo);

    //
    configuracionSistema.encoderPPR = configuracion.encoderPPR;
    configuracionSistema.longitudArco = configuracion.longitudArco;
    config->setConfiguracionToGUI(configuracionSistema);
}

void MainWindow::AplicarTabla(const QVector<FilaMedicion> &tabla)
{
    for(int r=0; r< tabla.size(); r++)
    {
        const FilaMedicion &registro = tabla[r];

        ActualizarCelda(r, 0, registro.posicion);
        ActualizarCelda(r, 1, registro.corriente);
        ActualizarCelda(r, 2, registro.sp);
        ActualizarCelda(r, 3, registro.vnc);
        ActualizarCelda(r, 4, registro.vnl);
        ActualizarCelda(r, 5, registro.rnc);
        ActualizarCelda(r, 6, registro.rnl);
    }
}

void MainWindow::AplicarEstado(const Estado &estado)
{

    tabla_numfila_actual_marcado = estado.tabla_numfila_actual_marcado;

    if (tabla_numfila_actual_marcado > 0)
    {
        //Resaltar la fila actual
        tableWidget->selectRow(tabla_numfila_actual_marcado);
        tableWidget->setFocus(Qt::OtherFocusReason);
    }

    ui->recorridoTotal->setEnabled(estado.recorridoTotal_isEnabled);
    ui->intervalo->setEnabled(estado.intervalo_isEnabled);

    ui->pushButton_Reset->setEnabled(estado.pushButton_Reset_isEnabled);

    ui->pushButton_Motor->setChecked(estado.pushButton_Motor_isChecked);
    ui->pushButton_Motor->setEnabled(estado.pushButton_Motor_isEnabled);

    led_motor->setEnabled(estado.led_motor_isEnabled);
    led_motor->setState(estado.led_motor_state);

    //
    ui->pushButton_Inicio->setEnabled(estado.pushButton_Inicio_isEnabled);
    ui->pushButton_Pause->setEnabled(estado.pushButton_Pausa_isEnabled);
    ui->pushButton_Parar->setEnabled(estado.pushButton_Parar_isEnabled);
    ui->pushButton_Aceptar->setEnabled(estado.pushButton_Aceptar_isEnabled);

    //
    //ui->recorridoActual->setValue(estado.recorridoActual);


}
/*
 *
El patrón que buscamos es este:
m_restaurandoSesion = true;

BloquearSenalesGUI(true);

AplicarSessionData(sessionData);

BloquearSenalesGUI(false);

m_restaurandoSesion = false;

En tu código actual, AplicarSessionData() ya hace internamente el bloqueo y desbloqueo de señales:

El esquema real que tienes debería ser:

m_restaurandoSesion = true;

// consulta al micro, diálogo, etc.

AplicarSessionData(sessionData);
// dentro ya bloquea/desbloquea señales


m_restaurandoSesion = false;
 * */
bool MainWindow::AplicarSessionData(const SessionData &data)
{
    BloquearSenalesGUI(true);
    //-------------------------------------
    //if (!root.contains("config"))
    //{
    //    ok = false;break;
    //}

    AplicarConfiguracion(data.config);

    CrearTabla(data.config.recorridoTotal, data.config.intervalo);

    AplicarTabla(data.tabla);

    AplicarEstado(data.estado);

    BloquearSenalesGUI(false);


    return true;
}


/* Funciones libres o no-miembros de Mainwindow
 * Json solo entiende: texto, numero, boolean, array, objeto
 */
QString TipoRegistroToString(TipoRegistro tipo)
{
    switch (tipo)
    {
        case TipoRegistro::SP: return "SP";
        case TipoRegistro::NC: return "NC";
        case TipoRegistro::NL: return "NL";
        case TipoRegistro::L: return "L";
        default: return "SP";
    }
}
TipoRegistro StringToTipoRegistro(const QString &tipoReg)
{
    if (tipoReg == "SP")
        return TipoRegistro::SP;
    if (tipoReg == "NC")
        return TipoRegistro::NC;
    if (tipoReg == "NL")
        return TipoRegistro::NL;
    if (tipoReg == "L")
        return TipoRegistro::L;

    return TipoRegistro::SP;//x default
}

/*función miembro no estática de SessionData.
 * entro de ella puedes acceder directamente a:
    version
    config
    estado
    tabla
 * */
QJsonObject SessionData::toJson() const
{
    QJsonObject root;
    root["version"] = version;//jala a su variable miembro

    //-----------------------------
    QJsonObject configJson;
    configJson["recorridoTotal"] = config.recorridoTotal;
    configJson["intervalo"] = config.intervalo;
    configJson["pulsosEncoder"] = config.encoderPPR;
    configJson["longitudArco"] = config.longitudArco;
    configJson["tipoRegistro"] = TipoRegistroToString(config.tipoRegistro);
    //-----------------------------

    QJsonObject estadoJson;
    estadoJson["filaActual"] = estado.tabla_numfila_actual_marcado;
    estadoJson["recorridoActual"] = estado.recorridoActual;

    estadoJson["recorridoTotal_isEnabled"] = estado.recorridoTotal_isEnabled;
    estadoJson["intervalo_isEnabled"] =  estado.intervalo_isEnabled;


    estadoJson["pushButton_Reset_isEnable"] = estado.pushButton_Reset_isEnabled;

    estadoJson["pushButton_Motor_isChecked"] = estado.pushButton_Motor_isChecked;
    estadoJson["pushButton_Motor_isEnabled"] = estado.pushButton_Motor_isEnabled;

    estadoJson["led_motor_isEnabled"] = estado.led_motor_isEnabled;
    estadoJson["led_motor_state"] = estado.led_motor_state;

    estadoJson["pushButton_Inicio_isEnabled"] = estado.pushButton_Inicio_isEnabled;
    estadoJson["pushButton_Pausa_isEnabled"] = estado.pushButton_Pausa_isEnabled;
    estadoJson["pushButton_Parar_isEnabled"] = estado.pushButton_Parar_isEnabled;
    estadoJson["pushButton_Aceptar_isEnabled"] = estado.pushButton_Aceptar_isEnabled;



    //
    QJsonArray tablaJson;

    //range-based for "Recorre todos los elementos de la colección."
    for (const FilaMedicion &fila : tabla)
    {
        QJsonObject obj;
        obj["posicion"] = fila.posicion;
        obj["corriente"] = fila.corriente;
        obj["sp"] = fila.sp;
        obj["vnc"] = fila.vnc;
        obj["vnl"] = fila.vnl;
        obj["rnc"] = fila.rnc;
        obj["rnl"] = fila.rnl;

        tablaJson.append(obj);
    }

    root["config"] = configJson;
    root["estado"] = estadoJson;
    root["tabla"] = tablaJson;

    return root;
}

QString MainWindow::RutaArchivoSesion() const
{
    QString ruta = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);

    QDir().mkpath(ruta);

    return ruta + "/session.json";
}
bool MainWindow::ExisteSesion() const
{
    return QFile::exists(RutaArchivoSesion());
}
bool MainWindow::EscribirArchivoSesion(const QJsonObject &root)
{
    QSaveFile file(RutaArchivoSesion());

    if (!file.open(QIODevice::WriteOnly))
        return false;

    QJsonDocument documento(root);

    file.write(documento.toJson(QJsonDocument::Indented));

    if (!file.commit())
        return false;

    qDebug() << "Documento grabado en"
             << RutaArchivoSesion();

    return true;
}
bool MainWindow::LeerArchivoSesion(QJsonObject &root)
{
    QFile file(RutaArchivoSesion());

    if (!file.exists())
        return false;

    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray datos = file.readAll();

    file.close();

    QJsonParseError error;

    QJsonDocument documento = QJsonDocument::fromJson(datos, &error);

    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "JSON inválido:"
                 << error.errorString();

        return false;
    }

    root = documento.object();

    return true;
}


SessionData SessionData::fromJson(const QJsonObject &root)
{
    SessionData data;

    data.version = root["version"].toInt();

    //--------------------------------------------------------
    QJsonObject config = root["config"].toObject();
    data.config.recorridoTotal = config["recorridoTotal"].toDouble();
    data.config.intervalo = config["intervalo"].toDouble();
    data.config.encoderPPR = config["pulsosEncoder"].toInt();
    data.config.longitudArco = config["longitudArco"].toDouble();
    data.config.tipoRegistro =StringToTipoRegistro(config["tipoRegistro"].toString());

    //--------------------------------------------------------
    QJsonObject estado = root["estado"].toObject();
    data.estado.tabla_numfila_actual_marcado = estado["filaActual"].toInt();
    data.estado.recorridoActual = estado["recorridoActual"].toDouble();
    data.estado.recorridoTotal_isEnabled = estado["recorridoTotal_isEnabled"].toBool();

    data.estado.intervalo_isEnabled = estado["intervalo_isEnabled"].toBool();

    data.estado.pushButton_Reset_isEnabled = estado["pushButton_Reset_isEnable"].toBool();
    //
    data.estado.pushButton_Motor_isEnabled = estado["pushButton_Motor_isEnabled"].toBool();
    data.estado.pushButton_Motor_isChecked= estado["pushButton_Motor_isChecked"].toBool();
    //
    data.estado.led_motor_isEnabled= estado["led_motor_isEnabled"].toBool();
    data.estado.led_motor_state= estado["led_motor_state"].toBool();
    //
    data.estado.pushButton_Inicio_isEnabled = estado["pushButton_Inicio_isEnabled"].toBool();
    data.estado.pushButton_Pausa_isEnabled = estado["pushButton_Pausa_isEnabled"].toBool();
    data.estado.pushButton_Parar_isEnabled = estado["pushButton_Parar_isEnabled"].toBool();
    data.estado.pushButton_Aceptar_isEnabled = estado["pushButton_Aceptar_isEnabled"].toBool();

    //--------------------------------------------------------
    QJsonArray tabla = root["tabla"].toArray();
    for (const QJsonValue &value : tabla)
    {
        QJsonObject fila = value.toObject();

        FilaMedicion registro;

        registro.posicion = fila["posicion"].toDouble();
        registro.corriente = fila["corriente"].toDouble();
        registro.sp = fila["sp"].toDouble();
        registro.vnc = fila["vnc"].toDouble();
        registro.vnl = fila["vnl"].toDouble();
        registro.rnc = fila["rnc"].toDouble();
        registro.rnl = fila["rnl"].toDouble();

        data.tabla.append(registro);
    }

    return data;
}


/*Se agrego a la condición de usbport_!status
 * !usbCDC->isOpen()
 *porque usbport_status te dice que el dispositivo ha sido detectado, pero para mandar una consulta lo realmente importante es que QSerialPort esté abierto.

Además:
if (esperandoRecorridoMicro)
    return false;
evita lanzar una segunda consulta mientras la primera todavía está pendiente.
 */
bool MainWindow::SolicitarRecorridoActualMicro()
{
    if (!usbport_status || !usbCDC->isOpen())
    {
        QMessageBox::warning(
            this,
            tr("Restaurar sesión"),
            tr("No existe comunicación con la tarjeta de control."));
        return false;
    }

    if (esperandoRecorridoMicro)
        return false;

    esperandoRecorridoMicro = true;

    USB_send_data_integer(USB_DATACODE_GET_RECORRIDO_ACTUAL, 0);

    timerRespuestaRecorrido->start(500);

    return true;
}
/*
 * RestaurarSesion()
    = cargar sesión + iniciar consulta

ProcesarRecorridoActualMicro()
    = ya tengo sesión + ya tengo hardware
      → comparar y decidir


Menú Restaurar
      │
      ▼
RestaurarSesion()
      │
      ├── leer session.json
      ├── sessionData = fromJson(...)
      │
      └── SolicitarRecorridoActualMicro()
                     │
                     ├── esperando = true
                     ├── enviar @Q0
                     └── timer 500 ms

                         ↓

             AVR responde @R3.92

                         ↓

                   readyRead()
                         ↓
                   readSerial()
                         ↓
                  USB_commands()
                         ↓
             RESP_RECORRIDO_ACTUAL
                         │
                         ├── esperando = false
                         ├── stop timer
                         │
                         ▼
          ProcesarRecorridoActualMicro()
                         │
                         ├── comparar con sessionData
                         ├── mostrar diferencias
                         │
                  Cancelar│Aceptar
                         │
                         ▼
                AplicarSessionData()
*/

/*
 *                   session.json
                       │
              recorrido = 3.00
                       │
                       ▼
                  comparación
                       ▲
                       │
Micro ────────► recorrido = 3.27
                       │
                       ├────► GUI muestra 3.27
                       │
                       ▼
                ¿Desea restaurar?
                  /           \
             Cancelar       Restaurar
                │               │
                │               ├─ configuración
                │               ├─ tabla
                │               ├─ fila actual
                │               └─ demás estado
                │
                └─────────────────────────┐
                                          ▼
                              GUI continúa mostrando
                                      3.27
*/

/* así estaría quedando
 *                     RESTAURACIÓN
                         │
                         ▼
              m_restaurandoSesion = true
                         │
                         ├── detener autosave pendiente
                         │
                         ├── leer respuesta del micro
                         │
                         ├── modificar recorridoActual
                         │        ↑
                         │        └─ NO genera guardado
                         │
                         ├── mostrar diálogo
                         │
                  ┌──────┴──────────┐
                  │                 │
               Cancelar          Restaurar
                  │                 │
                  └──────┬──────────┘
                         ▼
              m_restaurandoSesion = false


                    y ademas:
                    timeout / desconexión
                            ↓
                    m_restaurandoSesion = false
 *

El patrón que buscamos es este:
m_restaurandoSesion = true;

BloquearSenalesGUI(true);

AplicarSessionData(sessionData);

BloquearSenalesGUI(false);

m_restaurandoSesion = false;

En tu código actual, AplicarSessionData() ya hace internamente el bloqueo y desbloqueo de señales:
*/
bool MainWindow::RestaurarSesion()
{
    QJsonObject root;

    if (!LeerArchivoSesion(root))
    {
        QMessageBox::warning(
            this,
            tr("Restaurar sesión"),
            tr("No se pudo leer la sesión guardada."));

        return false;
    }

    sessionData = SessionData::fromJson(root);

    // Desde este momento no permitimos que el autosave sobrescriba la sesión que estamos evaluando.
    // Congelar autosave durante todo el proceso.
    m_restaurandoSesion = true;

    // Cancelar cualquier guardado automático que hubiera
    // quedado pendiente antes de iniciar la restauración.
    // detener autosave pendiente
    if (m_timerAutoSave)
        m_timerAutoSave->stop();

    /*
     *  Ahora cuando llegue:

        ui->recorridoActual->setValue(recorridoMicro);

        sí se producirá el valueChanged(), pero:

        void MainWindow::NotificarCambioEstado()
        {
            if (m_restaurandoSesion)
                return;

        lo absorberá y no iniciará el autosave.
    */
    if (!SolicitarRecorridoActualMicro())
    {
        m_restaurandoSesion = false;
        return false;
    }


    return true;
}

bool MainWindow::RecorridoCoincide(const SessionData &data, double recorridoMicro)
{
    const double tolerancia = 0.005;

    double diferencia = qAbs(recorridoMicro - data.estado.recorridoActual);

    return diferencia <= tolerancia;
}

//Es llamado cuando responde el micro con el recorrido actual
bool MainWindow::ProcesarRecorridoActualMicro( double recorridoMicro)
{
    // La GUI siempre debe mostrar la posición real actual del equipo

    // Estamos todavía dentro del proceso de restauración,
    // así que esta modificación NO debe activar autosave.
    ui->recorridoActual->setValue(recorridoMicro);

    const bool coinciden = RecorridoCoincide(sessionData, recorridoMicro);

    QString diferencias = CompararSesionConHardware(sessionData, recorridoMicro);

    DialogoRestaurarSesion dlg(this);

    dlg.setDiferencias(diferencias);

    if (coinciden)
    {
        dlg.setWindowTitle(
            tr("Restaurar sesión"));
    }
    else
    {
        dlg.setWindowTitle(
            tr("Advertencia al restaurar sesión"));
    }

    /*  RestaurarSesion()
          ↓
        m_restaurandoSesion = true
              ↓
        micro responde
              ↓
        aparece diálogo
              ↓
        usuario pulsa Cancelar
              ↓
        return false
              ↓
        m_restaurandoSesion SIGUE EN TRUE
     * */
    if (dlg.exec() != QDialog::Accepted)
    {
        // Canceló la restauración,
        // pero recorridoActual ya quedó actualizado
        // con el valor real recibido del micro.

        m_restaurandoSesion = false;

        return false;
    }


    if (!AplicarSessionData(sessionData))
    {
        m_restaurandoSesion = false;

        QMessageBox::warning(
            this,
            tr("Restaurar sesión"),
            tr("La sesión está dañada o es incompatible."));

        return false;
    }

    m_restaurandoSesion = false;

    qDebug() << "Sesión restaurada.";

    return true;
}

QString MainWindow::CompararSesionConHardware(
    const SessionData &data,
    double recorridoMicro)
{
    double recorridoSesion =
        data.estado.recorridoActual;

    double diferencia =
        qAbs(recorridoMicro - recorridoSesion);

    if (diferencia <= 0.005)
    {
        return QString(
            "La posición actual del equipo coincide "
            "con la sesión guardada.");
    }

    return QString(
               "Se detectó una diferencia entre la sesión guardada "
               "y la posición actual del equipo.\n\n"
               "Recorrido guardado: %1 m\n"
               "Recorrido actual: %2 m\n"
               "Diferencia: %3 m\n\n"
               "¿Desea continuar con la restauración?")
        .arg(recorridoSesion, 0, 'f', 2)
        .arg(recorridoMicro, 0, 'f', 2)
        .arg(diferencia, 0, 'f', 2);
}
void MainWindow::TimeoutRecorridoMicro()
{
    if (!esperandoRecorridoMicro)
        return;

    esperandoRecorridoMicro = false;

    // Terminó el intento de restauración.
    m_restaurandoSesion = false;

    QMessageBox::warning(
        this,
        tr("Restaurar sesión"),
        tr("La tarjeta de control no respondió "
           "a la consulta del recorrido actual."));
}
