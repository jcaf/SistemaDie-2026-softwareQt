#ifndef SESSIONDATA_H
#define SESSIONDATA_H

    #include <QVector>
    //added x QJson
    #include <QJsonObject>
    #include <QJsonArray>
    #include <QJsonDocument>
    #include <QFile>

    enum class TipoRegistro
    {
        SP,
        NC,
        NL,
        L
    };

    struct Configuracion
    {
        double recorridoTotal = 0.0;
        double intervalo = 0.0;
        TipoRegistro tipoRegistro = TipoRegistro::SP;
        int encoderPPR = 0;
        double longitudArco = 0.0;
    };

    struct Estado
    {

        int filaActual = 0;
        //
        double recorridoActual = 0.0;
        bool recorridoTotal_isEnabled = false;
        //
        bool intervalo_isEnabled = false;
        //
        bool pushButton_Reset_isEnabled = false;

        bool pushButton_Motor_isChecked = false;
        bool pushButton_Motor_isEnabled = false;
        bool led_motor_state = false;
        bool led_motor_isEnabled = false;
        //
        bool pushButton_Inicio_isEnabled = false;
        bool pushButton_Pausa_isEnabled= false;
        bool pushButton_Parar_isEnabled = false;
        bool pushButton_Aceptar_isEnabled = false;


    };

    struct FilaMedicion
    {
        double posicion = 0;
        double corriente = 0;
        double sp = 0;
        double vnc = 0;
        double vnl = 0;
        double rnc = 0;
        double rnl = 0;

    };
/*
 * toJson()                         fromJson()
────────────────────            ────────────────────

NO static                        static

Tiene this                       NO tiene this

Trabaja sobre                    Tiene que crear
un SessionData existente         un SessionData

sessionData.toJson()             SessionData::fromJson(root)

Dentro:                          Dentro:

config.xxx                       data.config.xxx
estado.xxx                       data.estado.xxx
tabla                            data.tabla
 * */
    struct SessionData
    {
        int version = 1;
        Configuracion config;
        Estado estado;
        QVector<FilaMedicion> tabla;

        //add:
        QJsonObject toJson() const;

        static SessionData fromJson(const QJsonObject &root);
        void clear();
        bool isEmpty() const;
        //

    };
#endif // SESSIONDATA_H
